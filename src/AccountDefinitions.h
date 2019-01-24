//
// Created by Reza Alizadeh Majd on 2018-12-23.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
using namespace std;

typedef vector<string> StringList;
typedef map<string, string> StrStrMap;

struct AccountObject {
    string title;
    string provider;
    bool is_active;
    StrStrMap settings;
    map<string, StrStrMap> services;
};

enum AccountStatus {
    AC_NONE,
    AC_ONLINE,
    AC_OFFLINE,
    AC_ERROR
};
extern map<AccountStatus, string> AccountStatusString;


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
