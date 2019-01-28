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

struct ServiceParam {
    string key;
    string val;
    bool   is_required;
    bool   is_protected;
    string default_val;
};
typedef vector<ServiceParam> ServiceParamList;


class AccountService : public StrStrMap {

public:
    bool verified() { return _verified; }

    void applyVerification(const ServiceParamList &params);

    bool isProtected(string key) const;
    bool isRequired(string key) const;

protected:
    bool _verified = false;
    map<string, bool> _requiredDict;
    map<string, bool> _protectedDict;
};

struct AccountObject {
    string title;
    string provider;
    bool is_active;
    StrStrMap settings;
    map<string, AccountService> services;
};

enum AccountStatus {
    AC_NONE,
    AC_ONLINE,
    AC_OFFLINE,
    AC_ERROR
};
extern map<AccountStatus, string> AccountStatusString;


#endif //PX_ACCOUNTS_SERVICE_ACCOUNTDEFINITIONS_H
