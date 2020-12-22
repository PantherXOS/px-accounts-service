//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H

#include <iostream>
#include <string>
#include <map>
#include <list>

#include "AccountObject.h"
#include "AccountService.h"

using namespace std;

#define BASE_USER_PATH   "~/.guix-profile"
#define BASE_SYSTEM_PATH "/run/current-system/profile"

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
