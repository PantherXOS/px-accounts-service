//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#include "AccountObject.h"

#include <Plugins/PluginManager.h>
#include <ProviderHandler.h>
#include <Secret/SecretManager.h>

/**
 * check whether  account details and services are verified
 * for saving to disk
 *
 * @return account verification status
 */
bool AccountObject::verify() {
    GLOG_INF("verify account with id: ", this->idAsString());
    this->resetErrors();
    if (this->title.empty()) {
        GLOG_WRN("Missing required parameter: 'title'");
        this->addError("'title' is required");
        return false;
    }

    if (!this->provider.empty() && !this->_appendProviderParams()) {
        GLOG_WRN("Could not update provider details.");
        this->addError("Could not update provider details.");
        return false;
    }

    bool verified = true;
    for (auto &kv : this->services) {
        string svcName = kv.first;
        PluginContainerBase *plugin = PluginManager::Instance()[svcName];
        if (plugin == nullptr) {
            GLOG_WRN("Unknown service: ", svcName);
            addError(string("Unknown service '") + svcName + string("'"));
            verified = false;
        }
        bool svcVerified = kv.second.verify();
        if (!svcVerified) {
            this->addErrorList(kv.second.getErrors());
        }
        verified &= svcVerified;
    }
    if (!verified) {
        for (const auto &kv : this->services) {
            if (!kv.second.removeSecrets()) {
                GLOG_WRN("Failed to remove secrets for:", this->idAsString(), "[", kv.first, "]");
            }
        }
    }
    GLOG_INF("Account verification status: ", verified);
    return verified;
}

bool AccountObject::cleanup(const AccountObject &newAccount) {
    vector<string> newSvcNames;
    for (const auto &kv : newAccount.services) {
        newSvcNames.push_back(kv.first);
    }

    for (const auto &kv : this->services) {
        string svcName = kv.first;
        const auto& svc = kv.second;
        if (std::find(newSvcNames.begin(), newSvcNames.end(), svcName) == newSvcNames.end()) {
            // svc is not available anymore, so we can remove secret params
            svc.removeSecrets();
        }
    }
    return true;
}

AccountObject::AccountObject() { uuid_clear(this->id); }

bool AccountObject::performAccountRemoval() {
    for (auto &kv : this->services) {
        if (!kv.second.performServiceCustomRemoval()) {
            this->addErrorList(kv.second.getErrors());
            return false;
        }
    }
    if (!SecretManager::Instance().removeAccount(this->id)) {
        addError("Could not remove account secrets.");
    }
    return true;
}

/**
 * update parameters need to add to account services, based on defined provider
 *
 * @return Provider update status
 */
bool AccountObject::_appendProviderParams() {
    if (!ProviderHandler::Instance().exists(this->provider)) {
        this->addError(string("Unknown provider: '") + this->provider + string("'"));
        GLOG_ERR("Unknown provider: ", this->provider);
        return false;
    }

    GLOG_INF("Updating provider '", this->provider, "' params:");
    ProviderStruct &curProvider = ProviderHandler::Instance()[this->provider];
    for (auto &kv : this->services) {
        auto svcName = kv.first;
        if (curProvider.plugins.find(svcName) != curProvider.plugins.end()) { // matching plugin found.
            const auto pluginParams = curProvider.plugins[svcName];
            for (const auto &kv : pluginParams) {
                const auto &paramKey = kv.first;
                const auto &paramVal = kv.second;
                if (this->services[svcName].find(paramKey) == this->services[svcName].end()) { // param is not added before
                    this->services[svcName][paramKey] = paramVal;
                    GLOG_INF("\t[", svcName, "][", paramKey, "] = ", paramVal);
                }
            }
        }
    }
    return true;
}
