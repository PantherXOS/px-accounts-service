//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#include "AccountManager.h"
#include "AccountUtils.h"
#include "PluginManager.h"
#include "EventManager.h"

AccountManager::AccountManager() = default;

AccountManager &AccountManager::Instance() {
    static AccountManager instance;
    instance.resetErrors();
    return instance;
}

vector<string> AccountManager::getErrors() {
    return m_errorList;
}

void AccountManager::resetErrors() {
    m_errorList.clear();
}

void AccountManager::addError(const string &msg) {
    m_errorList.push_back(msg);
}

bool AccountManager::verifyAccount(const PXParser::AccountObject &act) {

    if (act.title.empty()) {
        addError("'title' is required");
        return false;
    }

    bool verified = true;
    for (const auto &kv : act.services) {
        if (PluginManager::Instance().exists(kv.first)) {
            auto vResult = PluginManager::Instance()[kv.first].verify(kv.second);
            verified &= vResult.verified;
        } else {
            verified = false;
        }
    }
    return verified;
}

bool AccountManager::createAccount(const PXParser::AccountObject &act) {
    if (!verifyAccount(act)) {
        addError("Account verification failed");
        return false;
    }
    string accountName = PXUTILS::ACCOUNT::title2name(act.title);
    if (!PXParser::write(accountName, act)) {
        addError("Error on saving account file");
        return false;
    }
    setStatus(accountName, PXParser::AC_NONE);
    return true;
}

bool AccountManager::modifyAccount(const string &accountName, const PXParser::AccountObject &act) {

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

    PXParser::AccountObject oldAct;
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
            PXParser::AccountObject act;
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

bool AccountManager::readAccount(const string &accountName, PXParser::AccountObject *account) {
    if (!PXParser::read(accountName, account)) {
        addError("Error on reading account file.");
        return false;
    }
    return  true;
}

bool AccountManager::setStatus(const string &accountName, PXParser::AccountStatus stat) {
    PXParser::AccountStatus oldStat = m_statDict[accountName];
    m_statDict[accountName] = stat;
    if (oldStat != stat) {
        EventManager::EMIT_STATUS_CHANGE(accountName, oldStat, stat);
    }
    return true;
}

PXParser::AccountStatus AccountManager::getStatus(const string &accountName) {
    if (m_statDict.find(accountName) == m_statDict.end())
        return PXParser::AC_NONE;
    return m_statDict[accountName];
}
