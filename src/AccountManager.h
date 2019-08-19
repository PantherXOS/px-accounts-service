//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H

#include "AccountParser.h"

/**
 * @brief Base Class for Managing Online Accounts
 * @details AccountManager is a Singleton class that is responsible for managing
 *          Online Accounts for PantherX.
 */
class AccountManager {
public:
    typedef vector<string> ProviderFilters_t;
    typedef vector<string> ServiceFilters_t;

public:
    /// @brief static method in order to access AccountManager object
    static AccountManager &Instance();

    /// @brief static method for accessing last occurd errors during last operation
    static vector<string> &LastErrors();

protected:
    /// @brief reset previous operation errors
    void resetErrors();

    /// @brief add new error to list of occured errors
    void addError(const string &msg);

    /// @brief verify AccountObject
    bool verifyAccount(AccountObject &act);

    /// @brief update params related to defined provider for AccountObject
    bool updateProviderRelatedParams(AccountObject &act);

    /// @brief verify Account against provided service
    bool verifyAccountService(AccountObject &act, const string &svcName);

public:
    /// @brief save new account to disk
    bool createAccount(AccountObject &act);

    /// @brief modify existing account details
    bool modifyAccount(const string &accountName, AccountObject &act);

    /// @brief delete an account from disk
    bool deleteAccount(const string &accountName);

    /// @brief get list of saved accounts
    vector<string> listAccounts(const ProviderFilters_t &providerFilter, const ServiceFilters_t &serviceFilter);

    /// @brief read account details
    bool readAccount(const string &accountName, AccountObject *account);

    /// @brief set status for an account
    bool setStatus(const string &accountName, AccountStatus stat);

    /// @brief get status of an account
    AccountStatus getStatus(const string &accountName);

private:
    explicit AccountManager();

protected:
    static AccountManager _instance;

    map<string, AccountStatus> m_statDict;  ///< @brief Mapping between accounts and their status
    vector<string> m_errorList;             ///< @brief vector that hold last operation's errors
};

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
