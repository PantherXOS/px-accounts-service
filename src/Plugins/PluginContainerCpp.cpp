//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#include "PluginContainerCpp.h"

PluginContainerCpp::PluginContainerCpp(const PluginInfo &info) : _loader(info.path) {
    _info = info;
    _inited = _loader.DLOpenLib();
}

PluginContainerCpp::~PluginContainerCpp() {
    _loader.DLCloseLib();
}

string PluginContainerCpp::getTitle() {
    auto plugin = _loader.DLGetInstance();
    return plugin->title;
}

VerifyResult PluginContainerCpp::verify(const StrStrMap &params) {
    auto plugin = _loader.DLGetInstance();
    return plugin->verify(params);
}

AuthResult PluginContainerCpp::authenticate(const ServiceParamList &params) {
    auto plugin = _loader.DLGetInstance();
    return plugin->authenticate(params);
}

StrStrMap PluginContainerCpp::read(const string &id) {
    auto plugin = _loader.DLGetInstance();
    return  plugin->read(id);
}

string PluginContainerCpp::write(VerifyResult &vResult, AuthResult &aResult) {
    auto plugin = _loader.DLGetInstance();
    return  plugin->write(vResult, aResult);
}

bool PluginContainerCpp::remove(const string &id) {
    auto plugin = _loader.DLGetInstance();
    return plugin->remove(id);
}
