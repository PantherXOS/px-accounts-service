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


struct AccountObject {
    typedef map<string, string> ParamDict;
    string title;
    string provider;
    bool is_active;
    ParamDict settings;
    map<string, ParamDict> services;
};

enum AccountStatus {
    AC_NONE,
    AC_ONLINE,
    AC_OFFLINE,
    AC_ERROR
};
extern map<AccountStatus, string> AccountStatusString;

typedef vector<string> StringList;
typedef map<string, string> StrStrMap;

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
