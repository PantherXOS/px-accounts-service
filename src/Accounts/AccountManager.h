//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H

#include <Utils/ErrorReporter.h>
#include <Utils/Utils.h>

#include <memory>

#include "AccountParser.h"
#include "Plugins/PluginManager.h"


/**
 * @brief Base Class for Managing Online Accounts
 * @details AccountManager is a Singleton class that is responsible for managing
 *          Online Accounts for PantherX.
 */
class AccountManager : public ErrorReporter {
   public:
    typedef vector<string> ProviderFilters_t;
    typedef vector<string> ServiceFilters_t;

   private:
    explicit AccountManager(const vector<ParserPath> &parserPaths);
    virtual ~AccountManager();

   public:
    // prevent AccountManager to be cloneable
    AccountManager(AccountManager &other) = delete;

    // prevent AccountManager to be assignable
    void operator=(const AccountManager &) = delete;

    /// @brief static method to initiate paths for AccountManager
    static bool Init(const vector<string> &userPaths, const vector<string> &readonlyPaths = vector<string>());

    /// @brief static method in order to access AccountManager object
    static AccountManager &Instance();

    /// @brief static method for accessing last occurred errors during last operation
    static StringList &LastErrors();

   public:
    /// @brief save new account to disk
    bool createAccount(AccountObject &act);

    /// @brief modify existing account details
    bool modifyAccount(const uuid_t &id, AccountObject &act);

    /// @brief delete an account from disk
    bool deleteAccount(const uuid_t &id);

    /// @brief get list of saved accounts
    list<AccountObject> listAccounts(const ProviderFilters_t &providerFilter = ProviderFilters_t(),
                                     const ServiceFilters_t &serviceFilter = ServiceFilters_t());

    /// @brief read account details
    bool readAccount(const uuid_t &id, AccountObject *account);

    /// @brief set status for an account
    bool setStatus(const uuid_t &id, AccountStatus stat);

    /// @brief get status of an account
    AccountStatus getStatus(const uuid_t &id);

    AccountParser *findParser(const uuid_t &id, bool onlyWritables);

   protected:
    static AccountManager *_instance;
    map<string, AccountStatus> m_statDict;  ///< @brief Mapping between accounts and their status
    list<AccountParser *> m_parsers;
};

#endif  // PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
