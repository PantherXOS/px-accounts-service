//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H

#include "AccountParser.h"

class AccountManager {
public:
    typedef vector<string> ProviderFilters_t;
    typedef vector<string> ServiceFilters_t;

public:
    static AccountManager &Instance();

    vector<string> getErrors();

protected:
    void resetErrors();

    void addError(const string &msg);

    bool verifyAccount(const AccountObject &act);


public:

    bool createAccount(const AccountObject &act);

    bool modifyAccount(const string &accountName, const AccountObject &act);

    bool deleteAccount(const string &accountName);

    vector<string> listAccounts(ProviderFilters_t providerFilter, ServiceFilters_t serviceFilter);

    bool readAccount(const string &accountName, AccountObject *account);

    bool setStatus(const string &accountName, AccountStatus stat);

    AccountStatus getStatus(const string &accountName);


private:
    explicit AccountManager();

protected:
    map<string, AccountStatus> m_statDict;
    vector<string> m_errorList;

};


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTMANAGER_H
