//
// Created by Reza Alizadeh Majd on 2018-12-02.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include "AccountDefinitions.h"

#include <pybind11/embed.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;
using namespace py::literals;


struct ServiceParam {
    string key;
    string val;
    bool   is_required;
    bool   is_protected;
    string default_val;
};
typedef vector<ServiceParam> ServiceParamList;

struct VerifyResult {
    bool             verified;
    ServiceParamList params;
    StringList       errors;

    VerifyResult(): verified(false) {
    }
};


struct AuthResult {
    bool       authenticated;
    StrStrMap  tokens;
    StringList errors;
};


PYBIND11_MAKE_OPAQUE(StrStrMap);
PYBIND11_MAKE_OPAQUE(StringList);
PYBIND11_MAKE_OPAQUE(ServiceParamList);


class PluginContainer {

public:
    explicit PluginContainer() = default;
    explicit PluginContainer(const string &title, const string &path = string());
    virtual ~PluginContainer() = default;

    string getTitle();

    VerifyResult verify(const StrStrMap &);
    AuthResult authenticate(const ServiceParamList &);

protected:
    bool             _inited = false;
    pybind11::object _module;
    pybind11::object _plugin;
};

// ============================================================================
class Plugin {

public:
    explicit Plugin() = default;
    virtual ~Plugin() = default;

    virtual VerifyResult verify(const StrStrMap &params) = 0;
    virtual AuthResult authenticate(const ServiceParamList &) = 0;

public:
    string title;
};

// ============================================================================
class PyPlugin final : public Plugin {
public:
    virtual VerifyResult verify(const StrStrMap &params) override;
    virtual AuthResult authenticate(const ServiceParamList &) override;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H
