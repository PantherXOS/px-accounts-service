//
// Created by Reza Alizadeh Majd on 9/11/19.
//

#include "AccountService.h"

#include "../Accounts/AccountDefinitions.h"
#include "../Plugins/PluginManager.h"
#include "../Secret/SecretManager.h"

#define PLUGIN_ID_PARAM "PLUGIN_ID"

/**
 * initiate service parameters, assign related plugin, set _inited flag to true
 *
 * @param act pointer to related account
 * @param name service name
 * @return service initiation status
 */
bool AccountService::init(AccountObject *act, const string &name) {
    this->_name = name;
    this->_account = act;
    this->_plugin = PluginManager::Instance()[name];
    this->_inited = (this->_plugin != nullptr);
    return this->_inited;
}

/**
 * apply results received from plugin's `verify` method to service
 *
 * @param params: List of ServiceParam details processed inside plugin
 */
void AccountService::applyVerification(const ServiceParamList &params) {
    _verified = false;
    this->clearService();
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

PluginContainerBase *AccountService::plugin() { return _plugin; }

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
        GLOG_ERR("parameter verification failed");
        addError("parameter verification failed");
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
        GLOG_INF("custom write done, ID:", pluginId);
        this->clearService();
        this->operator[](PLUGIN_ID_PARAM) = pluginId;
    } catch (std::exception &ex) {
        string errMsg = ex.what();
        // bypass plugin's missing write method warning.
        if (errMsg != "write not found") {
            GLOG_WRN(errMsg);
        }
        // TODO: check if we need to return false in this case or not.
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
    if (this->_plugin == nullptr) {
        GLOG_INF("service plugin is not initiated.");
        return nullptr;
    }
    auto verifyResult = this->_plugin->verify(*this);
    if (!verifyResult.verified) {
        this->addErrorList(verifyResult.errors);
        return nullptr;
    }

    auto savedSecrets = SecretManager::Instance().getAccountSecrets(this->_account->id);
    for (auto &param : verifyResult.params) {
        if (param.is_protected && param.val.empty()) {
            for (auto secret : savedSecrets) {
                if (EXISTS(secret->attributes, "service") && secret->attributes["service"] == this->_name) {
                    if (secret->is("dual_password")) {
                        param.val = secret->as<DualPasswordSecret>()->getMatchingPassword(param.key);
                    } else if (secret->is("password")) {
                        param.val = secret->as<PasswordSecret>()->password();
                    }
                }
            }
            if (param.val.empty()) {
                GLOG_INF("ignore secret load for: ", param.key, " (related saved secret not found)");
            } else {
                GLOG_INF("protected param value loaded: ", param.key, " -> ", param.val);
            }
        }
    }
    this->applyVerification(verifyResult.params);

    GLOG_INF("parameters are verified:");
    for (const auto &param : verifyResult.params) {
        GLOG_INF("\t", param.key, " : ", param.val,
                 (param.is_protected ? " - PROTECTED" : ""),  // protected flag
                 (param.is_required ? " - REQUIRED" : ""));   // required flag
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
        GLOG_INF("\t", token.label, " : ", mask_string(token.secret));
        for (auto &attrib : token.attributes) {
            GLOG_INF("\t\t[", attrib.first, " : ", attrib.second, "]");
        }
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
    map<string, SecretItemBase> secretDict;
    GLOG_INF("saving protected params:", aResult->tokens.size());

    for (const auto &token : aResult->tokens) {
        if (EXISTS(token.attributes, "schema")) {
            auto key = token.label;
            auto schema = token.attributes.find("schema")->second;
            string secretKey = "password";
            if (schema == "oauth2" || schema == "dual_password") {
                secretKey = token.label;
                key = schema;
            }
            GLOG_INF("   NEW SET SECRET PARAM:", key, "->", schema, "(", secretKey, ")");
            secretDict[key].label = this->_account->title + "->" + this->_name + " [" + schema + "]";
            secretDict[key].secrets[secretKey] = token.secret;
            secretDict[key].updateAttributes(this->_account->idAsString(), this->_name, token.attributes);
        } else {
            GLOG_WRN("schema not set for protected param:", token.label);
            addError("schema not set for protected param: " + token.label);
        }
    }
    for (const auto &kv : secretDict) {
        GLOG_INF("save new secret:", kv.second.toString(true));
        if (!SecretManager::Instance().setSecret(kv.second)) {
            GLOG_ERR("saving secret failed");
            this->addError("unable to set protected tokens.");
            return false;
        }
    }
    return true;
}

bool AccountService::removeSecrets() const {
    if (!this->_account->isIdSet()) {
        return false;
    }
    return SecretManager::Instance().removeService(this->_account->id, this->_name);
}

bool AccountService::performCustomRead() {
    if (this->find(PLUGIN_ID_PARAM) == this->end()) {
        return false;  // plugin id parameter id not found.
    }
    try {
        auto params = this->plugin()->read(this->operator[](PLUGIN_ID_PARAM));
        this->clearService();
        for (const auto &kv : params) {
            this->operator[](kv.first) = kv.second;
        }
    } catch (std::exception &ex) {
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
