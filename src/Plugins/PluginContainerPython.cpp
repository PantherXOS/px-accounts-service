//
// Created by Reza Alizadeh Majd on 2019-06-09.
//

#include "PluginContainerPython.h"
#include "../Accounts/AccountUtils.h"

// ===PLUGIN DEFINITION MACRO =================================================

PYBIND11_EMBEDDED_MODULE(PluginFramework, m) {

    m.doc() = "PantherX Online Accounts Plugin Framework for python";

    py::class_<IPlugin, PythonPlugin>(m, "Plugin")
            .def(py::init())
            .def_readwrite("title", &PythonPlugin::title);

    py::bind_map<StrStrMap>(m, "StrStrMap");
    py::bind_vector<StringList>(m, "StringList");
    py::bind_vector<ServiceParamList>(m, "ServiceParamList");
    py::bind_vector<SecretTokenList>(m, "SecretTokenList");

    py::class_<ServiceParam>(m, "ServiceParam")
            .def(py::init())
            .def_readwrite("key", &ServiceParam::key)
            .def_readwrite("val", &ServiceParam::val)
            .def_readwrite("is_required", &ServiceParam::is_required)
            .def_readwrite("is_protected", &ServiceParam::is_protected)
            .def_readwrite("default_val", &ServiceParam::default_val);

    py::class_<VerifyResult>(m, "VerifyResult")
            .def(py::init())
            .def_readwrite("verified", &VerifyResult::verified)
            .def_readwrite("params", &VerifyResult::params)
            .def_readwrite("errors", &VerifyResult::errors);

    py::class_<SecretToken>(m, "SecretToken")
            .def(py::init())
            .def_readwrite("label", &SecretToken::label)
            .def_readwrite("secret", &SecretToken::secret)
            .def_readwrite("attributes", &SecretToken::attributes);

    py::class_<AuthResult>(m, "AuthResult")
            .def(py::init())
            .def_readwrite("authenticated", &AuthResult::authenticated)
            .def_readwrite("tokens", &AuthResult::tokens)
            .def_readwrite("errors", &AuthResult::errors);
}

VerifyResult PythonPlugin::verify(const StrStrMap &params) {
    PYBIND11_OVERLOAD_PURE(VerifyResult, PythonPlugin, verify, params);
}

AuthResult PythonPlugin::authenticate(const ServiceParamList &params) {
    PYBIND11_OVERLOAD_PURE(AuthResult, PythonPlugin, authenticate, params);
}

// ============================================================================


PluginContainerPython::PluginContainerPython(const PluginInfo &info) {
    _info = info;
}

bool PluginContainerPython::init() {
    try {
        py::dict locals;
        locals["module_name"] = py::cast(PXUTILS::PLUGIN::package2module(_info.name));
        auto modulePath = this->loadPath();
        if (!modulePath.empty()) {
            locals["module_path"] = py::cast(modulePath);
        }
        py::exec(R"(
import sys
if module_path in sys.path:
   sys.path.remove(module_path)
sys.path.insert(0, module_path)
# for p in sys.path:
#    print(p)

import importlib
new_module = importlib.import_module(module_name)
        )",
                 py::globals(), locals);

        _module = locals["new_module"];
        py::object PluginClass = _module.attr("Plugin");
        _plugin = PluginClass();
        _inited = true;
    } catch (const std::exception &e) {
        GLOG_ERR(e.what());
        _inited = false;
    }
    return _inited;
}

string PluginContainerPython::getTitle() {
    return _plugin.attr("title").cast<string>();
}

VerifyResult PluginContainerPython::verify(const StrStrMap &params) {
    VerifyResult result;
    try {
        auto res = _plugin.attr("verify")(params);
        result = res.cast<VerifyResult>();
    }
    catch(py::error_already_set& e) {
        GLOG_ERR("Plugin [", this->getName(), "]: verification failed");
        GLOG_WRN(e.what());
        result.verified = false;
        result.errors.push_back(e.what());
        e.discard_as_unraisable(__func__);
    }
    return result;
}

AuthResult PluginContainerPython::authenticate(const ServiceParamList &params) {
    AuthResult result;
    try {
        auto res = _plugin.attr("authenticate")(params);
        result = res.cast<AuthResult>();
    }
    catch (py::error_already_set &e) {
        GLOG_ERR("Plugin [", this->getName(), "]: authentication failed");
        GLOG_WRN(e.what());
        result.authenticated = false;
        result.errors.push_back(e.what());
        e.discard_as_unraisable(__func__);
    }
    return result;
}

StrStrMap PluginContainerPython::read(const string &id) {
    if (py::hasattr(_plugin, "read")) {
        try {
            auto res = _plugin.attr("read")(id);
            return res.cast<StrStrMap>();
        } catch (py::error_already_set &e) {
            GLOG_ERR("Plugin [", this->getName(), "]: read failed");
            GLOG_WRN(e.what());
            e.discard_as_unraisable(__func__);
            throw std::logic_error(e.what());
        }
    } else {
        throw std::logic_error("read not found");
    }
}

string PluginContainerPython::write(VerifyResult &vResult, AuthResult &aResult) {
    if (py::hasattr(_plugin, "write")) {
        try {
            auto res = _plugin.attr("write")(vResult, aResult);
            return res.cast<string>();
        }
        catch (py::error_already_set &e) {
            GLOG_ERR("Plugin [", this->getName(), "]: write failed");
            GLOG_WRN(e.what());
            e.discard_as_unraisable(__func__);
            throw std::logic_error(e.what());
        }
    } else {
        throw std::logic_error("write not found");
    }
}

bool PluginContainerPython::remove(const string &id) {
    if (py::hasattr(_plugin, "remove")) {
        try {
            auto res = _plugin.attr("remove")(id);
            return res.cast<bool>();
        }
        catch (py::error_already_set &e) {
            GLOG_ERR("Plugin [", this->getName(), "]: remove failed");
            GLOG_WRN(e.what());
            e.discard_as_unraisable(__func__);
            throw std::logic_error(e.what());
        }
    } else {
        throw std::logic_error("remove not found");
    }
}
