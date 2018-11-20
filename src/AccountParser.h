//
// Created by Reza Alizadeh Majd on 11/7/18.
//

#ifndef PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
#define PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H

#define BASE_ACCOUNT_PATH    "~/.userdata/accounts/"

#include "AccountUtils.h"

#include <iostream>
#include <map>

using namespace std;


namespace PXParser {

    struct AccountObject {
        typedef  map<string, string> ParamDict;
        string title;
        string provider;
        bool   is_active;
        ParamDict settings;
        map<string, ParamDict> services;
    };

    enum AccountStatus {
        AC_NONE,
        AC_ONLINE,
        AC_OFFLINE,
        AC_ERROR
    };

    bool read(const string &acName, AccountObject *ac);

    bool write(const string &acName, const AccountObject &ac);

    bool remove(const string &acName);

    inline string accountsPath() {
        string acPath = PXUTILS::FILE::abspath(BASE_ACCOUNT_PATH);
        if (!PXUTILS::FILE::exists(acPath)) {
            system((string("mkdir -p ") + acPath).c_str());
        }
        return acPath;
    }

    inline string fullPath(const string &acName) {
        return accountsPath() + acName + string(".yaml");
    }

    void print_account(const AccountObject& act);
}

#endif //PX_ACCOUNTS_SERVICE_ACCOUNTPARSER_H
