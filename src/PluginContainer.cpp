//
// Created by Reza Alizadeh Majd on 2018-12-02.
//

#include "PluginContainer.h"
#include "AccountUtils.h"

PYBIND11_EMBEDDED_MODULE(PluginFramework, m) {

    m.doc() = "PantherX Online Accounts Plugin Framework for python";

    py::class_<Plugin, PyPlugin>(m, "Plugin")
            .def("test", &Plugin::verify)
            .def_readwrite("title", &Plugin::title);

    py::bind_map<StrStrMap>(m, "StrStrMap");
    py::bind_vector<StringList>(m, "StringList");
    py::bind_vector<ServiceParamList>(m, "ServiceParamList");

    py::class_<ServiceParam>(m, "ServiceParam")
            .def(py::init())
            .def_readwrite("key",          &ServiceParam::key)
            .def_readwrite("val",          &ServiceParam::val)
            .def_readwrite("is_required",  &ServiceParam::is_required)
            .def_readwrite("is_protected", &ServiceParam::is_protected)
            .def_readwrite("default_val",  &ServiceParam::default_val);

    py::class_<VerifyResult>(m, "VerifyResult")
            .def(py::init())
            .def_readwrite("verified", &VerifyResult::verified)
            .def_readwrite("params",   &VerifyResult::params)
            .def_readwrite("errors",   &VerifyResult::errors);

    py::class_<AuthResult>(m, "AuthResult")
            .def(py::init())
            .def_readwrite("authenticated", &AuthResult::authenticated)
            .def_readwrite("tokens", &AuthResult::tokens)
            .def_readwrite("errors", &AuthResult::errors);

}

VerifyResult PyPlugin::verify(const StrStrMap &params) {
    PYBIND11_OVERLOAD_PURE(VerifyResult, Plugin, verify, params);
}

AuthResult PyPlugin::authenticate(const ServiceParamList &params) {
    PYBIND11_OVERLOAD_PURE(AuthResult, Plugin, authenticate, params);
}


PluginContainer::PluginContainer(const string &title, const string &path) {

    py::dict locals;
    locals["module_name"] = py::cast(PXUTILS::PLUGIN::package2module(title));
    if (!path.empty()) {
        locals["path"] = py::cast(path);
    }
    py::exec(R"(
import sys
sys.path.append('.')

import importlib
new_module = importlib.import_module(module_name)

#new_module = None
#try:
#   path
#except NameError:
#   import importlib
#   new_module = importlib.import_module(module_name)
#else:
#   if sys.version_info[0] < 3:
#      import imp
#      new_module = imp.load_module(module_name, open(path), path, ('py', 'U', imp.PY_SOURCE))
#   else:
#      import importlib.machinery
#      new_module = importlib.machinery.SourceFileLoader(module_name, path).load_module()
)",
             py::globals(), locals);

    _module = locals["new_module"];
    py::object PluginClass = _module.attr("Plugin");
    _plugin = PluginClass();
    _inited = true;
}

string PluginContainer::getTitle() {
    return _plugin.attr("title").cast<string>();
}

VerifyResult PluginContainer::verify(const StrStrMap &params) {
    auto res = _plugin.attr("verify")(params);
    return  res.cast<VerifyResult>();
}

AuthResult PluginContainer::authenticate(const ServiceParamList &params) {
    auto res = _plugin.attr("authenticate")(params);
    return res.cast<AuthResult>();
}