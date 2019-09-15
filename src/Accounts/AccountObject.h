//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTOBJECT_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTOBJECT_H

#include <Utils/Utils.h>
#include <Utils/ErrorReporter.h>

#include "AccountService.h"


/// @brief Object holding Account details
class AccountObject : public ErrorReporter {
public:
    /// @brief verify Account
    bool verify();

    bool performAccountCustomRemoval();

protected:
    /// @brief update params related to defined provider for AccountObject
    bool _appendProviderParams();

public:
    string title; ///< @brief title of account
    string provider; ///< @brief defined Provider for account
    bool is_active; ///< @brief shows whether the account is active or not
    StrStrMap settings; ///< @brief Key-Value Map of settings related to an account
    map<string, AccountService> services; ///< @brief Map of Services defined inside an account
};


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTOBJECT_H
