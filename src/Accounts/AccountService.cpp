//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#include "../Accounts/AccountDefinitions.h"
#include "../Plugins/PluginManager.h"
#include "../Secret/SecretManager.h"
#include "AccountService.h"

#define PLUGIN_ID_PARAM "PLUGIN_ID"

bool AccountService::init(AccountObject *act, const string &name) {
    this->_name = name;
    this->_account = act;
    this->_plugin = PluginManager::Instance()[name];
    this->_inited = true;
    return this->_inited;
}

/**
 * apply results received from plugin's `verify` method to service
 *
 * @param params: List of ServiceParam details processed inside plugin
 */
void AccountService::applyVerification(const ServiceParamList &params) {
    _verified = false;
    _requiredDict.clear();
    _protectedDict.clear();
    for (const auto &param : params) {
        this->operator[](param.key) = param.val;
        _requiredDict[param.key] = param.is_required;
        _protectedDict[param.key] = param.is_protected;
    }
    _verified = true;
}

/**
 * check whether provided key is protected parameter or not
 *
 * @param key name of parameter to check if is protected or not
 *
 * @return protected flag for provided key
 */
bool AccountService::isProtected(const string &key) const {
    if (_verified && _protectedDict.find(key) != _protectedDict.end()) {
        return _protectedDict.find(key)->second;
    }
    return false;
}

/**
 * check whether provided key is a required parameter or not
 *
 * @param key name of parameter to check if is required or not
 *
 * @return required flag for provided key
 */
bool AccountService::isRequired(const string &key) const {
    if (_verified && _requiredDict.find(key) != _requiredDict.end()) {
        return _requiredDict.find(key)->second;
    }
    return false;
}

PluginContainerBase *AccountService::plugin() {
    return _plugin;
}

/**
 * Verify AccountService, this method.
 * tasks need to perform during service verification:
 *  - verify about provided parameters
 *  - attach required protected params
 *  - authenticate service
 *  - save generated protected params and tokens
 *
 * @return Account service verification status
 */
bool AccountService::verify() {
    this->resetErrors();
    VerifyResultPtr verifyResult = this->_verifyParams();
    if (verifyResult == nullptr) {
        return false;
    }
    AuthResultPtr authResult = this->_authenticate(verifyResult);
    if (authResult == nullptr) {
        return false;
    }
    if (!this->_saveProtectedParams(verifyResult, authResult)) {
        return false;
    }

    try {
        string pluginId = this->plugin()->write(*(verifyResult.get()), *(authResult.get()));
        this->clearService();
        this->operator[](PLUGIN_ID_PARAM) = pluginId;
    }
    catch (std::exception &ex) {
        GLOG_WRN(ex.what());
    }
    return true;
}

/**
 * Call Plugin's parameter verification method to prepare service parameters
 * also load protected parameters needed for a service to be authenticated.
 *
 * @return          VerifyResult object
 */
VerifyResultPtr AccountService::_verifyParams() {
    auto verifyResult = this->_plugin->verify(*this);
    if (!verifyResult.verified) {
        this->addErrorList(verifyResult.errors);
        return nullptr;
    }

    for (auto &param: verifyResult.params) {
        if (param.is_protected && param.val.empty()) {
            param.val = SecretManager::Instance().Get(this->_account->title, this->_name, param.key);
            if (param.val.empty()) {
                this->addError("protected param not found: '" + param.key + "'");
                return nullptr;
            }
            GLOG_INF("protected param value loaded: ", param.key, " -> ", param.val);
        }
    }
    this->applyVerification(verifyResult.params);

    GLOG_INF("parameters are verified:");
    for (const auto &param : verifyResult.params) {
        GLOG_INF("\t", param.key, " : ", param.val,
                 (param.is_protected ? " - PROTECTED" : ""),
                 (param.is_required ? " - REQUIRED" : ""));
    }
    return make_shared<VerifyResult>(verifyResult);
}

/**
 * call plugin's authenticate method to check if an account service could
 * be authenticated or not. this method also provides list of warning
 * messages needs to reported to remote RPC client
 *
 * @param vResult   VerifyResult object that provided previously by plugin
 * @return          AuthResult object that holds authentication results
 */
AuthResultPtr AccountService::_authenticate(VerifyResultPtr &vResult) {
    auto authResult = this->plugin()->authenticate(vResult->params);
    this->addErrorList(authResult.errors);
    if (!authResult.authenticated) {
        return nullptr;
    }
    GLOG_INF("service authenticated:");
    for (const auto &token : authResult.tokens) {
        GLOG_INF("\t", token.first, " : ", token.second);
    }
    return make_shared<AuthResult>(authResult);
}

/**
 * save protected parameters and generated tokens to SecretManager
 *
 * @param vResult   pointer to plugin's `verify` method results
 * @param aResult   pointer to plugins's `authenticate` method results
 * @return          result of saving protected methods
 */
bool AccountService::_saveProtectedParams(VerifyResultPtr &vResult, AuthResultPtr &aResult) {
    for (const auto &param : vResult->params) {
        if (param.is_protected) {
            if (!SecretManager::Instance().Set(this->_account->title, this->_name, param.key, param.val)) {
                GLOG_ERR("saving secret failed");
                this->addError("unable to set protected params.");
                return false;
            }
        }
    }
    for (const auto &token : aResult->tokens) {
        const auto &key = token.first;
        const auto &val = token.second;
        if (!SecretManager::Instance().Set(this->_account->title, this->_name, key, val)) {
            GLOG_ERR("saving secret failed");
            this->addError("unable to set protected tokens.");
            return false;
        }
    }
    return true;
}


bool AccountService::performCustomRead() {

    if (this->find(PLUGIN_ID_PARAM) == this->end()) {
        return false; // plugin id parameter id not found.
    }
    try {
        auto params = this->plugin()->read(this->operator[](PLUGIN_ID_PARAM));
        this->clearService();
        for (const auto &kv : params) {
            this->operator[](kv.first) = kv.second;
        }
    }
    catch (std::exception &ex) {
        GLOG_ERR(ex.what());
        return false;
    }
    return true;
}


bool AccountService::performServiceCustomRemoval() {
    if (this->find(PLUGIN_ID_PARAM) == this->end()) {
        return true;
    }
    try {
        auto pluginId = this->operator[](PLUGIN_ID_PARAM);
        return this->plugin()->remove(pluginId);
    } catch (std::exception &ex) {
        GLOG_WRN(ex.what());
    }
    return true;
}
