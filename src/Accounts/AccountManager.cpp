//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"

#include <algorithm>

#include "EventManager.h"
#include "Secret/SecretManager.h"

// AccountManager AccountManager::_instance;  // NOLINT(cert-err58-cpp)

AccountManager::AccountManager() {
    auto userPaths = PXUTILS::PATH::extract_path_str(std::string(ACCOUNT_PATHS));
    for (const auto &path : userPaths) {
        auto *parser = new AccountParser(path, false);
        if (parser) {
            m_parsers.push_back(parser);
        } else {
            GLOG_ERR("Unable to init parser: ", path);
        }
    }
    if (!std::string(READONLY_ACCOUNT_PATHS).empty()) {
        auto readonlyPaths = PXUTILS::PATH::extract_path_str(std::string(READONLY_ACCOUNT_PATHS));
        for (const auto &path : readonlyPaths) {
            if (PXUTILS::FILE::exists(path)) {
                auto *parser = new AccountParser(path, true);
                m_parsers.push_back(parser);
            } else {
                GLOG_ERR("Unable to init readonly parser: ", path);
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

AccountManager &AccountManager::_rawInstance() {
    static AccountManager instance; 
    return instance;
}

/**
 * @return initiated instance of AccountManager
 */
AccountManager &AccountManager::Instance() {
    _rawInstance().resetErrors();
    return _rawInstance();
}

/**
 * @return list of errors occurred during last operation
 */
StringList &AccountManager::LastErrors() { return _rawInstance().getErrors(); }

/**
 * Save Provided AccountObject to disk
 *
 * @param[in,out] act AccountObject we want to create
 * @return Account creation status
 */
bool AccountManager::createAccount(AccountObject &act) {
    uuid_t accountId;
    uuid_clear(accountId);

    auto *parser = this->findParser(accountId, true);  // get parser for new accounts
    if (!parser) {
        GLOG_ERR("Parser not found!");
        addError("Could not find writable parser!");
        return false;
    }
    uuid_generate(accountId);
    uuid_copy(act.id, accountId);

    if (!act.verify()) {
        addErrorList(act.getErrors());
        addError("Account verification failed");
        return false;
    }

    if (!parser->write(act)) {
        addError("Could not save account file.");
        return false;
    }
    setStatus(accountId, AC_NONE);
    EventManager::EMIT_CREATE_ACCOUNT(act);
    return true;
}

/**
 * modify existing account details
 *
 * @param accountName name of account we want to modify
 * @param act AccountObject that we want to selected account to be replaced with
 * @return account modification status
 */
bool AccountManager::modifyAccount(const uuid_t &id, AccountObject &act) {
    AccountObject oldAct;
    if (!readAccount(id, &oldAct)) {
        addError("Could not read old account details.");
        return false;
    }
    auto *parser = this->findParser(oldAct.id, true);  // get parser for new accounts
    if (!parser) {
        addError("Could not find writable parser!");
        return false;
    }
    
    uuid_copy(act.id, id);  // overwrite id to prevent modifying the account Id
    if (!act.verify()) {
        addErrorList(act.getErrors());
        addError("Account verification failed");
        return false;
    }
    if (!parser->write(act)) {
        addError("Could not save account file.");
        return false;
    }
    setStatus(oldAct.id, AC_NONE);
    EventManager::EMIT_MODIFY_ACCOUNT(oldAct);
    return true;
}

/**
 * delete existing account from disk
 *
 * @param accountName name of account we want to delete
 * @return account deletion status
 */
bool AccountManager::deleteAccount(const uuid_t &id) {
    auto *parser = this->findParser(id, true);
    if (!parser) {
        addError("Could not find suitable parser.");
        return false;
    }
    AccountObject act;
    if (!parser->read(id, act)) {
        addError("Could not read account before deletion.");
        return false;
    }
    if (!act.performAccountRemoval()) {
        addErrorList(act.getErrors());
        return false;
    }
    if (!parser->remove(id)) {
        addError("Could not remove account file.");
        return false;
    }
    m_statDict.erase(act.idAsString());
    EventManager::EMIT_DELETE_ACCOUNT(act);
    return true;
}

/**
 * search for existing account titles
 *
 * @param providerFilter filter list for accounts that use specific providers
 * @param serviceFilter filter list for accounts that use specific services
 * @return list of account titles that matched with provided filters
 */
list<AccountObject> AccountManager::listAccounts(const ProviderFilters_t &providerFilter,
                                                 const ServiceFilters_t &serviceFilter) {
    list<AccountObject> accountList;
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
                accountList.push_back(act);
            }
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
bool AccountManager::readAccount(const uuid_t &id, AccountObject *account) {
    auto *parser = this->findParser(id, false);
    if (!parser) {
        addError("Could not find suitable parser.");
        return false;
    }
    if (!parser->read(id, *account)) {
        addError("Error on reading account file: '" + uuid_as_string(id) + "'.");
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
bool AccountManager::setStatus(const uuid_t &id, AccountStatus stat) {
    // Pre-check to determine if account exists before setting the status
    AccountObject act;
    if (!readAccount(id, &act)) {
        return false;
    }

    AccountStatus oldStat = m_statDict[act.idAsString()];
    m_statDict[act.idAsString()] = stat;
    if (oldStat != stat) {
        EventManager::EMIT_STATUS_CHANGE(act, oldStat, stat);
    }
    return true;
}

/**
 * get status of specific account
 *
 * @param accountName title of account that we want to read it's status
 * @return status of account
 */
AccountStatus AccountManager::getStatus(const uuid_t &id) {
    auto strId = uuid_as_string(id);
    if (m_statDict.find(strId) == m_statDict.end()) {
        return AC_NONE;
    }
    return m_statDict[strId];
}

AccountParser *AccountManager::findParser(const uuid_t &id, bool onlyWritables) {
    GLOG_INF("Search", (onlyWritables ? "writable" : ""), "parser for:", uuid_as_string(id));
    for (auto *parser : m_parsers) {
        if (!onlyWritables || !parser->isReadonly()) {
            if (parser->hasAccount(id) || uuid_is_null(id)) {
                GLOG_INF("Parser selected: ", (parser->isReadonly() ? "[R-ONLY] " : ""), parser->path());
                return parser;
            }
        }
    }
    return nullptr;
}
