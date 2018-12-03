//
// Created by Reza Alizadeh Majd on 2018-12-02.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H

#include <iostream>
#include <map>
#include <vector>
using namespace std;

#include <pybind11/embed.h>
#include <pybind11/stl_bind.h>
namespace py = pybind11;
using namespace py::literals;


typedef vector<string> StringList;
typedef map<string, string> RawParamMap;

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
};

PYBIND11_MAKE_OPAQUE(RawParamMap);
PYBIND11_MAKE_OPAQUE(StringList);
PYBIND11_MAKE_OPAQUE(ServiceParamList);


class PluginContainer {

public:
    explicit PluginContainer() = default;
    explicit PluginContainer(const string &title, const string &path = string());
    virtual ~PluginContainer() = default;

    string getTitle();

    VerifyResult verify(const map<string, string> &);

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

    virtual VerifyResult verify(const std::map<std::string, std::string> &params) = 0;

public:
    string title;
};

// ============================================================================
class PyPlugin final : public Plugin {
public:
    virtual VerifyResult verify(const map<string, string> &params);
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINER_H
