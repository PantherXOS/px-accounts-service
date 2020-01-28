//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"
#include "Secret/SecretManager.h"
#include "EventManager.h"

AccountManager AccountManager::_instance; // NOLINT(cert-err58-cpp)

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
StringList &AccountManager::LastErrors() {
    return _instance.getErrors();
}

/**
 * Save Provided AccountObject to disk
 *
 * @param[in,out] act AccountObject we want to create
 * @return Account creation status
 */
bool AccountManager::createAccount(AccountObject &act, bool emitCreateEvent) {
    if (!act.verify()) {
        addErrorList(act.getErrors());
        addError("Account verification failed");
        return false;
    }
    string accountName = PXUTILS::ACCOUNT::title2name(act.title);
    if (!PXParser::write(accountName, act)) {
        addError("Error on saving account file");
        return false;
    }
    if (emitCreateEvent) {
        EventManager::EMIT_CREATE_ACCOUNT(act.title);
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
    string oldName = PXUTILS::ACCOUNT::title2name(oldAct.title);
    string newName = PXUTILS::ACCOUNT::title2name(act.title);

    if (titleChanged) {
        // check for new title existence.
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

    if (!this->createAccount(act, false)) {
        SecretManager::Instance().RemoveAccount(act.title);
        return false;
    }

    if (titleChanged) {
        // remove old account related data
        return SecretManager::Instance().RemoveAccount(oldAct.title)
               && this->deleteAccount(accountName);
    }
    EventManager::EMIT_MODIFY_ACCOUNT(oldAct.title, (titleChanged ? act.title : ""));
    return true;
}

/**
 * delete existing account from disk
 *
 * @param accountName name of account we want to delete
 * @return account deletion status
 */
bool AccountManager::deleteAccount(const string &accountName) {
    AccountObject act;
    if (!PXParser::read(accountName, &act)) {
        addError("unable to read account before delete");
        return false;
    }
    if (!act.performAccountRemoval()) {
        addErrorList(act.getErrors());
        return false;
    }
    if (!PXParser::remove(accountName)) {
        addError("unable to remove account file");
        return false;
    }
    m_statDict.erase(accountName);
    EventManager::EMIT_DELETE_ACCOUNT(act.title);
    return true;
}

/**
 * search for existing account titles
 *
 * @param providerFilter filter list for accounts that use specific providers
 * @param serviceFilter filter list for accounts that use specific services
 * @return list of account titles that matched with provided filters
 */
vector<string>
AccountManager::listAccounts(const ProviderFilters_t &providerFilter, const ServiceFilters_t &serviceFilter) {

    vector<string> accountList;
    auto accountFiles = PXUTILS::FILE::dirfiles(PXParser::accountsPath(), ".yaml");

    for (const string &fname : accountFiles) {
        auto accountName = fname.substr(0, fname.find(".yaml"));

        AccountObject account;
        bool accepted = PXParser::read(accountName, &account);
        if (accepted && (!providerFilter.empty() || !serviceFilter.empty())) {
            accepted = false;
            for (const auto &provider : providerFilter) {
                if (account.provider == provider) {
                    accepted = true;
                    break;
                }
            }
            for (const auto &svcName : serviceFilter) {
                for (const auto &kv : account.services) {
                    if (svcName == kv.first) {
                        accepted = true;
                        break;
                    }
                }
            }
        }
        if (accepted) {
            accountList.push_back(account.title);
        }
    }
    return accountList;
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
    for (auto &kv : account->services) {
        kv.second.performCustomRead();
    }
    return true;
}

/**
 * set provided status for an account
 *
 * @param accountName  title of account that we want to set it's status
 * @param stat the status we want to set for an account
 * @return set status result
 */
bool AccountManager::setStatus(const string &accountName, AccountStatus stat) {
    // Pre-check to determine if account exists before setting the status
    AccountObject act;
    if (!readAccount(accountName, &act)) {
        return false;
    }

    AccountStatus oldStat = m_statDict[accountName];
    m_statDict[accountName] = stat;
    if (oldStat != stat) {
        EventManager::EMIT_STATUS_CHANGE(act.title, oldStat, stat);
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
