//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"
#include "ProviderHandler.h"
#include "Secret/SecretManager.h"
#include "EventManager.h"

AccountManager AccountManager::_instance;

AccountManager::AccountManager() = default;

/**
 * @return initiated instance of AccountManager
 */
AccountManager &AccountManager::Instance() {
    _instance.resetErrors();
    return _instance;
}

/**
 * @return list of errors occurred during last operation
 */
vector<string> &AccountManager::LastErrors() {
    return _instance.m_errorList;
}

void AccountManager::resetErrors() {
    m_errorList.clear();
}

/**
 * add new message to list of current operation errors
 * @param msg error message to add.
 */
void AccountManager::addError(const string &msg) {
    m_errorList.push_back(msg);
}

void AccountManager::addErrorList(const StringList &errList) {
    for (const auto &err: errList) {
        addError(err);
    }
}

/**
 * check whether provided account details and services are verified
 * for saving to disk
 *
 * @param[in,out] act AccountObject to verify
 *
 * @return account verification status
 */
bool AccountManager::verifyAccount(AccountObject &act) {

    GLOG_INF("verifying Account: (title: '", act.title, "')");
    if (act.title.empty()) {
        addError("'title' is required");
        return false;
    }

    if (!act.provider.empty()) {
        if (!updateProviderRelatedParams(act)) {
            addError("update provider params failed");
            return false;
        }
    }

    bool verified = true;
    for (const auto &kv : act.services) {
        verified &= verifyService(act, kv.first);
    }
    return verified;
}

/**
 * update parameters need to add to account services, based on defined provider
 *
 * @param[in,out] act: AccountObject that needs to be updates
 *
 * @return Provider update status
 */
bool AccountManager::updateProviderRelatedParams(AccountObject &act) {

    if (!ProviderHandler::Instance().exists(act.provider)) {
        addError(string("unknown provider: '") + act.provider + string("'"));
        GLOG_ERR("unknown provider: ", act.provider);
        return false;
    }

    GLOG_INF("Updating provider '", act.provider, "' params:");
    ProviderStruct &provider = ProviderHandler::Instance()[act.provider];
    for (const auto &plg : provider.plugins) {
        const string &plgName = plg.first;
        const auto &plgParams = plg.second;

        for (const auto &prm : plgParams) {
            const auto &pkey = prm.first;
            const auto &pval = prm.second;
            act.services[plgName][pkey] = pval;
            GLOG_INF("\t[", plgName, "][", pkey, "] = ", pval);
        }
    }
    return true;
}

/**
 * Verify AccountObject against selected service.
 *
 * @param[in,out] act AccountObject we need to verify
 * @param[in] svcName service name we want to verify
 *
 * @return Account service verification status
 */
bool AccountManager::verifyService(AccountObject &act, const string &svcName) {
    GLOG_INF("verifying service: '", svcName,"'");

    PluginContainerBase *plugin = PluginManager::Instance()[svcName];
    if (plugin == nullptr) {
        addError(string("unknown service '") + svcName + string("'"));
        return false;
    }
    shared_ptr<VerifyResult> verifyResult = this->performServiceParamVerification(act, svcName, plugin);
    if (verifyResult == nullptr) {
        return false;
    }
    shared_ptr<AuthResult> authResult = this->performServiceAuthentication(verifyResult, plugin);
    if (authResult == nullptr) {
        return false;
    }
    return this->saveServiceProtectedParams(act, svcName, verifyResult, authResult);
}

/**
 * Call Plugin's parameter verification method to prepare service parameters
 * also load protected parameters needed for a service to be authenticated.
 *
 * @param act       AccountObject we want to verify a service from
 * @param svcName   name of service that needs to be verified
 * @param plugin    pointer to related plugin needs to be used for service verification
 * @return          VerifyResult object
 */
std::shared_ptr<VerifyResult> AccountManager::performServiceParamVerification(AccountObject &act,
                                                                              const string &svcName,
                                                                              PluginContainerBase *plugin) {
    auto verifyResult = plugin->verify(act.services[svcName]);
    if (!verifyResult.verified) {
        addErrorList(verifyResult.errors);
        return nullptr;
    }

    for (auto &param: verifyResult.params) {
        if (param.is_protected && param.val.empty()) {
            param.val = SecretManager::Instance().Get(act.title, svcName, param.key);
            if (param.val.empty()) {
                addError("protected param not found: '" + param.key + "'");
                return nullptr;
            }
            GLOG_INF("protected param value loaded: ", param.key, " -> ", param.val);
        }
    }
    act.services[svcName].applyVerification(verifyResult.params);

    GLOG_INF("parameters are verified:");
    for (const auto &param : verifyResult.params) {
        GLOG_INF("\t", param.key, " : ", param.val,
                 (param.is_protected ? " - PROTECTED" : ""),
                 (param.is_required ? " - REQUIRED" : ""));
    }
    return make_shared<VerifyResult>(verifyResult);
}

/**
 * call plugin's authenticate method to check if an account service could
 * be authenticated or not. this method also provides list of warning
 * messages needs to reported to remote RPC client
 *
 * @param vResult   VerifyResult object that provided previously by plugin
 * @param plugin    pointer to plugin object that is responsible for service verification
 * @return          AuthResult object that holds authentication results
 */
std::shared_ptr<AuthResult> AccountManager::performServiceAuthentication(shared_ptr<VerifyResult> vResult,
                                                                         PluginContainerBase *plugin) {
    auto authResult = plugin->authenticate(vResult->params);
    addErrorList(authResult.errors);
    if (!authResult.authenticated) {
        return nullptr;
    }
    GLOG_INF("service authenticated:");
    for (const auto &token : authResult.tokens) {
        GLOG_INF("\t", token.first, " : ", token.second);
    }
    return make_shared<AuthResult>(authResult);
}

/**
 * save protected parameters and generated tokens to SecretManager
 *
 * @param act       AccountObject that we want to save it's protected params
 * @param svcName   name of service
 * @param vResult   pointer to plugin's `verify` method results
 * @param aResult   pointer to plugins's `authenticate` method results
 * @return          result of saving protected methods
 */
bool AccountManager::saveServiceProtectedParams(AccountObject &act,
                                                const string &svcName,
                                                const shared_ptr<VerifyResult> &vResult,
                                                const shared_ptr<AuthResult> &aResult) {
    for (const auto &param : vResult->params) {
        if (param.is_protected) {
            if (!SecretManager::Instance().Set(act.title, svcName, param.key, param.val)) {
                GLOG_ERR("saving secret failed");
                addError("unable to set protected params.");
                return false;
            }
        }
    }
    for (const auto &token : aResult->tokens) {
        const auto &key = token.first;
        const auto &val = token.second;
        if (!SecretManager::Instance().Set(act.title, svcName, key, val)) {
            GLOG_ERR("saving secret failed");
            addError("unable to set protected tokens.");
            return false;
        }
    }
    return true;
}

/**
 * Save Provided AccountObject to disk
 *
 * @param[in,out] act AccountObject we want to create
 * @return Account creation status
 */
bool AccountManager::createAccount(AccountObject &act) {
    if (!verifyAccount(act)) {
        addError("Account verification failed");
        return false;
    }
    string accountName = PXUTILS::ACCOUNT::title2name(act.title);
    if (!PXParser::write(accountName, act)) {
        addError("Error on saving account file");
        return false;
    }
    setStatus(accountName, AC_NONE);
    return true;
}

/**
 * modify existing account details
 *
 * @param accountName name of account we want to modify
 * @param act AccountObject that we want to selected account to be replaced with
 * @return account modification status
 */
bool AccountManager::modifyAccount(const string &accountName, AccountObject &act) {

    AccountObject oldAct;
    if (!readAccount(accountName, &oldAct)) {
        addError("Error on reading old Account details.");
        return false;
    }
    bool titleChanged = (oldAct.title != act.title);
    map<string, string> oldActProtectedParams;
    string newName = PXUTILS::ACCOUNT::title2name(act.title);

    if (titleChanged) {
        // check for new title existance.
        for (const auto &name : listAccounts(ProviderFilters_t(), ServiceFilters_t())) {
            if (accountName != name && newName == name) {
                addError("account with specified 'title' already exists.");
                return false;
            }
        }
        // transfer old account's protected params
        oldActProtectedParams = SecretManager::Instance().GetAccount(oldAct.title);
        SecretManager::Instance().SetAccount(act.title, oldActProtectedParams);
    }

    if (!this->createAccount(act)) {
        SecretManager::Instance().RemoveAccount(act.title);
        return false;
    }

    if (titleChanged) {
        // remove old account related data
        return SecretManager::Instance().RemoveAccount(oldAct.title)
               && this->deleteAccount(accountName);
    }
    return true;
}

/**
 * delete existing account from disk
 *
 * @param accountName name of account we want to delete
 * @return account deletion status
 */
bool AccountManager::deleteAccount(const string &accountName) {
    if (!PXParser::remove(accountName)) {
        return false;
    }
    m_statDict.erase(accountName);
    return true;
}

/**
 * search for existing account titles
 *
 * @param providerFilter filter list for accounts that use specific providers
 * @param serviceFilter filter list for accounts that use specific services
 * @return list of account titles that matched with provided filters
 */
vector<string> AccountManager::listAccounts(const ProviderFilters_t &providerFilter, const ServiceFilters_t &serviceFilter) {

    vector<string> accounts;
    auto accountFiles = PXUTILS::FILE::dirfiles(PXParser::accountsPath(), ".yaml");

    for (const string &fname : accountFiles) {
        auto actName = fname.substr(0, fname.find(".yaml"));

        bool accepted = providerFilter.empty() && serviceFilter.empty();
        if (!accepted) {
            AccountObject act;
            if (PXParser::read(actName, &act)) {

                for (const auto &provider : providerFilter) {
                    if (act.provider == provider) {
                        accepted = true;
                        break;
                    }
                }
                for (const auto &service : serviceFilter) {
                    for (const auto &kv : act.services) {
                        if (service == kv.first) {
                            accepted = true;
                            break;
                        }
                    }
                }
            }
        }
        if (accepted) {
            accounts.push_back(actName);
        }
    }
    return accounts;
}

/**
 * read account details
 *
 * @param[in] accountName name of account we want to read
 * @param[out] account AccountObject that we fill it's details during account read procedure
 * @return read account status
 */
bool AccountManager::readAccount(const string &accountName, AccountObject *account) {
    if (!PXParser::read(accountName, account)) {
        addError("Error on reading account file: '" + accountName + "'.");
        return false;
    }
    return true;
}

/**
 * set provided status for an account
 *
 * @param accountName  title of account that we want to seti it's status
 * @param stat the status we want to set for an account
 * @return set status result
 */
bool AccountManager::setStatus(const string &accountName, AccountStatus stat) {
    AccountStatus oldStat = m_statDict[accountName];
    m_statDict[accountName] = stat;
    if (oldStat != stat) {
        EventManager::EMIT_STATUS_CHANGE(accountName, oldStat, stat);
    }
    return true;
}

/**
 * get status of specific account
 *
 * @param accountName title of account that we want to read it's status
 * @return status of account
 */
AccountStatus AccountManager::getStatus(const string &accountName) {
    if (m_statDict.find(accountName) == m_statDict.end())
        return AC_NONE;
    return m_statDict[accountName];
}
