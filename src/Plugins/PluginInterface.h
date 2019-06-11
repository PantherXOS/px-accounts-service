//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H
#define PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H

#include <string>
#include <vector>
#include <map>
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

struct VerifyResult {
    bool verified;
    ServiceParamList params;
    StringList errors;

    VerifyResult() : verified(false) {
    }
};

struct AuthResult {
    bool authenticated;
    StrStrMap tokens;
    StringList errors;
};


class IPlugin {
public:
    explicit IPlugin() = default;

    explicit IPlugin(const string &title) : title(title) {}

    virtual ~IPlugin() = default;

    virtual VerifyResult verify(const StrStrMap &params) = 0;

    virtual AuthResult authenticate(const ServiceParamList &params) = 0;

public:
    string title;
};

#endif //PX_ACCOUNTS_SERVICE_PLUGIN_INTERFACE_H