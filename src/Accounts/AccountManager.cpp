//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"

#include <algorithm>

#include "EventManager.h"
#include "Secret/SecretManager.h"

AccountManager AccountManager::_instance;  // NOLINT(cert-err58-cpp)

AccountManager::AccountManager() {
    auto userPaths = PXUTILS::PATH::extract_path_str(std::string(ACCOUNT_PATHS));
    for (const auto &path : userPaths) {
        if (PXUTILS::FILE::exists(path)) {
            auto *parser = new AccountParser(path, false);
            m_parsers.push_back(parser);
        } else {
            GLOG_WRN("invalid user path provided:", path);
        }
    }
    if (!std::string(READONLY_ACCOUNT_PATHS).empty()) {
        auto readonlyPaths = PXUTILS::PATH::extract_path_str(std::string(READONLY_ACCOUNT_PATHS));
        for (const auto &path : readonlyPaths) {
            if (PXUTILS::FILE::exists(path)) {
                auto *parser = new AccountParser(path, true);
                m_parsers.push_back(parser);
            } else {
                GLOG_WRN("invalid readonly path provided:", path);
            }
        }
    }
}

AccountManager::~AccountManager() {
    for (const auto *parser : m_parsers) {
        if (parser) {
            delete parser;
        }
    }
}

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
StringList &AccountManager::LastErrors() { return _instance.getErrors(); }

/**
 * Save Provided AccountObject to disk
 *
 * @param[in,out] act AccountObject we want to create
 * @param existenceCheck   check whether account is already exists during account creation
 * @param emitCreateEvent  configure event emission during account creation
 * @return Account creation status
 */
bool AccountManager::createAccount(AccountObject &act, bool existenceCheck, bool emitCreateEvent) {
    if (!act.verify()) {
        addErrorList(act.getErrors());
        addError("Account verification failed");
        return false;
    }
    if (existenceCheck) {
        auto accountList = listAccounts();
        if (std::find(accountList.begin(), accountList.end(), act.title) != accountList.end()) {
            string err = "account with title='" + act.title + "' already exists.";
            GLOG_ERR(err);
            addError(err);
            return false;
        }
    }
    string accountName = PXUTILS::ACCOUNT::title2name(act.title);
    auto *parser = this->findParser(accountName, true);
    if (!parser) {
        parser = this->findParser(string(), true);  // get parser for new accounts
        if (!parser) {
            addError("there is no writable parser found!");
            return false;
        }
    }
    if (!parser->write(accountName, act)) {
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

    if (!this->createAccount(act, false, false)) {
        SecretManager::Instance().RemoveAccount(act.title);
        return false;
    }

    if (titleChanged) {
        // remove old account related data
        return SecretManager::Instance().RemoveAccount(oldAct.title) && this->deleteAccount(accountName);
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
    auto *parser = this->findParser(accountName, true);
    if (!parser) {
        addError("Account Parser not found");
        return false;
    }
    AccountObject act;
    if (!parser->read(accountName, act)) {
        addError("unable to read account before delete");
        return false;
    }
    if (!act.performAccountRemoval()) {
        addErrorList(act.getErrors());
        return false;
    }
    if (!parser->remove(accountName)) {
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
vector<string> AccountManager::listAccounts(const ProviderFilters_t &providerFilter,
                                            const ServiceFilters_t &serviceFilter) {
    vector<string> titles;
    for (auto *parser : m_parsers) {
        for (const auto &act : parser->list()) {
            bool accepted = providerFilter.empty() && serviceFilter.empty();
            for (const auto &provider : providerFilter) {
                if (act.provider == provider) {
                    accepted = true;
                }
            }
            for (const auto service : serviceFilter) {
                for (const auto &kv : act.services) {
                    if (service == kv.first) {
                        accepted = true;
                        break;
                    }
                }
            }
            if (accepted) {
                titles.push_back(act.title);
            }
        }
    }
    return titles;
}

/**
 * read account details
 *
 * @param[in] accountName name of account we want to read
 * @param[out] account AccountObject that we fill it's details during account read procedure
 * @return read account status
 */
bool AccountManager::readAccount(const string &accountName, AccountObject *account) {
    auto *parser = this->findParser(accountName, false);
    if (!parser) {
        addError("Can't find suitable parser");
        return false;
    }
    if (!parser->read(accountName, *account)) {
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
    if (m_statDict.find(accountName) == m_statDict.end()) {
        return AC_NONE;
    }
    return m_statDict[accountName];
}

AccountParser *AccountManager::findParser(const string &actName, bool onlyWritables) {
    for (auto *parser : m_parsers) {
        if (!onlyWritables || !parser->isReadonly()) {
            if (parser->hasAccount(actName) || actName.empty()) {
                return parser;
            }
        }
    }
    return nullptr;
}
