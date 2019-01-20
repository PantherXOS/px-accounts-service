//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#include "ProviderHandler.h"
#include "AccountUtils.h"
#include <yaml-cpp/yaml.h>

#define PROVIDER_KEY          "provider"
#define PROVIDER_KEY_TITLE    "title"
#define PROVIDER_KEY_PLUGINS "plugins"


ProviderHandler ProviderHandler::_instance;

ProviderHandler::ProviderHandler() {
    init();
}

bool ProviderHandler::init() {

    string storePath = PXUTILS::FILE::abspath(PROVIDER_STORE_PATH);
    string userPath = PXUTILS::FILE::abspath(PROVIDER_USER_PATH);

    for (const auto &provider : PXUTILS::FILE::dirfiles(storePath, ".yaml")) {
        if (!initProvider(storePath + provider)) {
            return false;
        }
    }

    for (const auto &provider : PXUTILS::FILE::dirfiles(userPath, ".yaml")) {
        if (!initProvider(userPath + provider)) {
            return false;
        }
    }
    return true;
}

bool ProviderHandler::initProvider(string providerPath) {

    if (!PXUTILS::FILE::exists(providerPath)) {
        addError(string("Provider file not found: ") + providerPath );
        return false;
    }

    try {
        ProviderStruct provider;

        YAML::Node root = YAML::LoadFile(providerPath);
        provider.title = root[PROVIDER_KEY][PROVIDER_KEY_TITLE].as<string>();

        for (const auto &it : root[PROVIDER_KEY][PROVIDER_KEY_PLUGINS]) {
            for (const auto &svc : it) {
                string svcName = svc.first.as<string>();
                const YAML::Node &params = svc.second;
                for (const auto &p : params) {
                    string pkey = p.first.as<string>();
                    string pval = p.second.as<string>();
                    provider.plugins[svcName][pkey] = pval;
                }
            }
        }
        _providers[provider.title] = provider;
    }
    catch(const YAML::Exception &ex){
        addError(ex.what());
        cout << ex.what() << endl;
        return false;
    }
    return true;
}

ProviderHandler &ProviderHandler::Instance() {
    return _instance;
}

ProviderStruct &ProviderHandler::operator[](const string &title) {
    return _providers[title];
}

bool ProviderHandler::exists(const string &title) {
    return _providers.find(title) != _providers.end();
}

map<string, ProviderStruct> &ProviderHandler::providers() {
    return _providers;
}
