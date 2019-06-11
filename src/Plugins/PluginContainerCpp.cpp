//
// Created by Reza Alizadeh Majd on 2019-06-10.
//

#include "PluginContainerCpp.h"

PluginContainerCpp::PluginContainerCpp(const PluginInfo &info) :
        _loader(info.path) {

    _loader.DLOpenLib();

    _info = info;
    _inited = true;
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
