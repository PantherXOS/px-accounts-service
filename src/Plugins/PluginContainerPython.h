//
// Created by Reza Alizadeh Majd on 2019-06-09.
//

#ifndef PX_ACCOUNTS_SERVICE_PLUGINCONTAINERPYTHON_H
#define PX_ACCOUNTS_SERVICE_PLUGINCONTAINERPYTHON_H

#include "PluginContainerBase.h"

#include <pybind11/embed.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;
using namespace py::literals;

PYBIND11_MAKE_OPAQUE(StrStrMap);
PYBIND11_MAKE_OPAQUE(StringList);
PYBIND11_MAKE_OPAQUE(ServiceParamList);

class PythonPlugin {

public:
    explicit PythonPlugin() = default;

    virtual ~PythonPlugin() = default;

    virtual VerifyResult verify(const StrStrMap &params);

    virtual AuthResult authenticate(const ServiceParamList &);

public:
    string title;
};


class PluginContainerPython : public PluginContainerBase {

public:
    explicit PluginContainerPython(const PluginInfo &info);

public:
    string getTitle() override;

    VerifyResult verify(const StrStrMap &params) override;

    AuthResult authenticate(const ServiceParamList &params) override;

protected:
    pybind11::object _module;
    pybind11::object _plugin;
};


#endif //PX_ACCOUNTS_SERVICE_PLUGINCONTAINERPYTHON_H
