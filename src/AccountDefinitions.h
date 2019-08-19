//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H

#include <iostream>
#include <string>
#include <map>

#include "Plugins/PluginInterface.h"

#define BASE_USER_PATH   "~/.guix-profile"
#define BASE_SYSTEM_PATH "/run/current-system/profile"

using namespace std;

///@brief Object holds detail of an Account Service
class AccountService : public StrStrMap {

public:
    /// getter to check if service is verified
    bool verified() { return _verified; }

    /// @brief apply plugin verification results
    void applyVerification(const ServiceParamList &params);

    /// @brief check if provided key is protected
    bool isProtected(const string &key) const;

    /// @brief check if provided key is required
    bool isRequired(const string &key) const;

protected:
    bool _verified = false;
    map<string, bool> _requiredDict;
    map<string, bool> _protectedDict;
};

/// @brief Object holding Account details
struct AccountObject {
    string title; ///< @brief title of account
    string provider; ///< @brief defined Provider for account
    bool is_active; ///< @brief shows whether the account is active or not
    StrStrMap settings; ///< @brief Key-Value Map of settings related to an account
    map<string, AccountService> services; ///< @brief Map of Services defined inside an account
};

/// @brief Account Status Enumeration
enum AccountStatus {
    AC_NONE,    ///< initial value for account status, mean no status is set for account yet
    AC_ONLINE,  ///< status which shows that account is Online
    AC_OFFLINE, ///< status which shows that account is Offline
    AC_ERROR    ///< Error status which set's for account in case of any error occurrence
};

/// @brief mapping between AccountStatus values and their string representation
extern map<AccountStatus, string> AccountStatusString;


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
