//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"
#include "AccountUtils.h"
#include "Plugins/PluginManager.h"
#include "ProviderHandler.h"
#include "Secret/SecretManager.h"
#include "EventManager.h"

AccountManager AccountManager::_instance;

AccountManager::AccountManager() = default;

AccountManager &AccountManager::Instance() {
    _instance.resetErrors();
    return _instance;
}

vector<string> &AccountManager::LastErrors() {
    return _instance.m_errorList;
}

void AccountManager::resetErrors() {
    m_errorList.clear();
}

void AccountManager::addError(const string &msg) {
    m_errorList.push_back(msg);
}

bool AccountManager::verifyAccount(AccountObject &act) {

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
        verified &= verifyAccountService(act, kv.first);
    }
    return verified;
}

bool AccountManager::updateProviderRelatedParams(AccountObject &act) {

    if (!ProviderHandler::Instance().exists(act.provider)) {
        addError(string("unknown provider: '") + act.provider + string("'"));
        LOG_ERR("unknown provider: %s", act.provider.c_str());
        return false;
    }

    LOG_INF("Updating provider '%s' params:", act.provider.c_str());
    bool verified = true;
    ProviderStruct &provider = ProviderHandler::Instance()[act.provider];
    for (const auto &plg : provider.plugins) {
        const string &plgName = plg.first;
        const auto &plgParams = plg.second;

        for (const auto &prm : plgParams) {
            const auto &pkey = prm.first;
            const auto &pval = prm.second;
            act.services[plgName][pkey] = pval;
            LOG_INF("\t[%s][%s] = %s", plgName.c_str(), pkey.c_str(), pval.c_str());
        }
    }
    return true;
}

bool AccountManager::verifyAccountService(AccountObject &act, const string &svcName) {
    LOG_INF("verifying '%s'", svcName.c_str());

    if (!PluginManager::Instance().exists(svcName)) {
        addError(string("unknown service '") + svcName + string("'"));
        return false;
    }

    AccountService &curService = act.services[svcName];
    PluginContainerBase &svcPlugin = PluginManager::Instance()[svcName];
    auto verifyResult = svcPlugin.verify(curService);
    if (!verifyResult.verified) {
        for (const auto &err : verifyResult.errors) {
            addError(err);
        }
        return false;
    }
    LOG_INF("%s", "parameters are verified:");
    for (const auto &param : verifyResult.params) {
        LOG_INF("\t%s : %s%s%s", param.key.c_str(), param.val.c_str(),
                (param.is_protected ? " - PROTECTED" : ""),
                (param.is_required ? " - REQUIRED": ""));
    }

    curService.applyVerification(verifyResult.params);

    auto authResult = svcPlugin.authenticate(verifyResult.params);
    if (!authResult.authenticated) {
        for (const auto &err : authResult.errors) {
            addError(err);
        }
        return false;
    }
    LOG_INF("%s", "service authenticated:");
    for (const auto &token : authResult.tokens) {
        LOG_INF("\t%s : %s", token.first.c_str(), token.second.c_str());
    }

    LOG_INF("saving protected params:");
    for (const auto &param : verifyResult.params) {
        if (param.is_protected) {
            if (!SecretManager::Instance().Set(act.title, svcName, param.key, param.val)) {
                LOG_ERR("saving secret failed");
                return false;
            }
        }
    }
    for (const auto &token : authResult.tokens) {
        const auto &key = token.first;
        const auto &val = token.second;
        if (!SecretManager::Instance().Set(act.title, svcName, key, val)) {
            LOG_ERR("saving secret failed");
            return false;
        }
    }

    return true;
}

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

bool AccountManager::modifyAccount(const string &accountName, AccountObject &act) {

    if (!verifyAccount(act)) {
        addError("Account verification failed");
        return false;
    }

    string newName = PXUTILS::ACCOUNT::title2name(act.title);
    if (accountName != newName) {       // title changed. -> check account title conflicts.

        for (const auto &name : listAccounts(ProviderFilters_t(), ServiceFilters_t())) {
            if (accountName != name && newName == name) {
                addError("account with specified 'title' already exists.");
                return false;
            }
        }
    }

    AccountObject oldAct;
    if (!readAccount(accountName, &oldAct)) {
        addError("Error on reading old Account details.");
        return false;
    }

    if (!createAccount(act)) {
        return false;
    }

    if (accountName != newName) {
        if (!deleteAccount(accountName)) {
            return false;
        }
    }
    return true;
}

bool AccountManager::deleteAccount(const string &accountName) {
    if (!PXParser::remove(accountName)) {
        return false;
    }
    m_statDict.erase(accountName);
    return true;
}

vector<string> AccountManager::listAccounts(ProviderFilters_t providerFilter, ServiceFilters_t serviceFilter) {

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

bool AccountManager::readAccount(const string &accountName, AccountObject *account) {
    if (!PXParser::read(accountName, account)) {
        addError("Error on reading account file: '" + accountName + "'.");
        return false;
    }
    return  true;
}

bool AccountManager::setStatus(const string &accountName, AccountStatus stat) {
    AccountStatus oldStat = m_statDict[accountName];
    m_statDict[accountName] = stat;
    if (oldStat != stat) {
        EventManager::EMIT_STATUS_CHANGE(accountName, oldStat, stat);
    }
    return true;
}

AccountStatus AccountManager::getStatus(const string &accountName) {
    if (m_statDict.find(accountName) == m_statDict.end())
        return AC_NONE;
    return m_statDict[accountName];
}
