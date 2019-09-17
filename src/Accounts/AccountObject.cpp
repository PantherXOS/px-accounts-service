//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#include "AccountObject.h"
#include <Plugins/PluginManager.h>
#include <ProviderHandler.h>


/**
 * check whether  account details and services are verified
 * for saving to disk
 *
 * @return account verification status
 */
bool AccountObject::verify() {
    this->resetErrors();
    if (this->title.empty()) {
        this->addError("'title' is required");
        return false;
    }

    if (!this->provider.empty() && !this->_appendProviderParams()) {
        this->addError("update provider params failed");
        return false;
    }

    bool verified = true;
    for (auto &kv : this->services) {
        string svcName = kv.first;
        PluginContainerBase *plugin = PluginManager::Instance()[svcName];
        if (plugin == nullptr) {
            addError(string("unknown service '") + svcName + string("'"));
            verified = false;
        }
        bool svcVerified = kv.second.verify();
        if (!svcVerified) {
            this->addErrorList(kv.second.getErrors());
        }
        verified &= svcVerified;
    }
    return verified;
}

bool AccountObject::performAccountCustomRemoval() {
    for (auto &kv : this->services) {
        if (!kv.second.performServiceCustomRemoval()) {
            this->addErrorList(kv.second.getErrors());
            return false;
        }
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
        this->addError(string("unknown provider: '") + this->provider + string("'"));
        GLOG_ERR("unknown provider: ", this->provider);
        return false;
    }

    GLOG_INF("Updating provider '", this->provider, "' params:");
    ProviderStruct &curProvider = ProviderHandler::Instance()[this->provider];
    for (const auto &plg : curProvider.plugins) {
        const string &plgName = plg.first;
        const auto &plgParams = plg.second;

        for (const auto &prm : plgParams) {
            const auto &pkey = prm.first;
            const auto &pval = prm.second;
            this->services[plgName][pkey] = pval;
            GLOG_INF("\t[", plgName, "][", pkey, "] = ", pval);
        }
    }
    return true;
}
