//
// Created by Reza Alizadeh Majd on 2019-01-16.
//

#include "ProviderHandler.h"
#include "AccountUtils.h"
#include <yaml-cpp/yaml.h>

#define PROVIDER_KEY          "provider"
#define PROVIDER_KEY_TITLE    "title"
#define PROVIDER_KEY_PLUGINS "plugins"


ProviderHandler::ProviderHandler() {

    init(PXUTILS::FILE::abspath(PROVIDER_SYSTEM_PATH));
    init(PXUTILS::FILE::abspath(PROVIDER_APP_PATH));
    init(PXUTILS::FILE::abspath(PROVIDER_USER_PATH));
}

/**
 *
 * @param path base path for loading provider definition files
 *
 * @return privider handler initiation status
 */
bool ProviderHandler::init(const string& path) {

    for (const string& providerName: PXUTILS::FILE::dirfiles(path, ".yaml")) {
        if (!initProvider(string(path).append("/").append(providerName))) {
            return false;
        }
    }
    return true;
}

/**
 *
 * @param providerPath path to provider file
 *
 * @return initiation status
 */
bool ProviderHandler::initProvider(const string &providerPath) {

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
    static ProviderHandler instance;
    return instance;
}

/**
 * @param title provider title
 *
 * @return refrence to  provider with specified title
 */
ProviderStruct &ProviderHandler::operator[](const string &title) {
    return _providers[title];
}

/**
 * @param title provider title to search for
 * @return whether that the provider with specified title is exists or not
 */
bool ProviderHandler::exists(const string &title) {
    return _providers.find(title) != _providers.end();
}

/**
 * @return reference to all existing providers
 */
map<string, ProviderStruct> &ProviderHandler::providers() {
    return _providers;
}
