//
// Created by Reza Alizadeh Majd on 2019-06-15.
//

#include "SecretManager.h"

#include "../Accounts/AccountUtils.h"

// === SecretItemBase methods: ================================================

void SecretItemBase::updateAttributes(const string &accountId, const string &service, const StrStrMap &others) {
    /**
     * service        : attribute as string
     * username       : attribute as string
     * account_id     : attribute as string
     * schema         : attribute as string
     */
    attributes["service"] = service;
    attributes["account_id"] = accountId;
    attributes["schema"] = this->schema();
    for (const auto &attr : others) {
        attributes[attr.first] = attr.second;
    }
}

string SecretItemBase::getSecret(const string &key) const {
    auto res = secrets.find(key);
    if (res != secrets.end()) {
        return res->second;
    }
    return string();
}

void SecretItemBase::setSecret(const string &key, const string &val) { secrets[key] = val; }

string SecretItemBase::toString(bool pretty) const {
    string sep1 = pretty ? "\n" : " ";
    string sep2 = pretty ? "   " : " ";
    stringstream sstream;
    sstream << "Secret: " << sep1;
    sstream << "attributes: " << sep1;
    for (const auto &attr : attributes) {
        sstream << sep2 << "[" << attr.first << ":" << attr.second << "]" << sep1;
    }
    sstream << "secrets: " << sep1;
    for (const auto &s : secrets) {
        sstream << sep2 << "[" << s.first << ":" << s.second << "]" << sep1;
    }
    return sstream.str();
}
// ============================================================================
// ============================================================================

SecretManager::~SecretManager() {
    if (_rpcClient != nullptr) {
        delete _rpcClient;
        _rpcClient = nullptr;
    }
}

SecretManager &SecretManager::Instance() {
    static SecretManager instance;
    return instance;
}

/**
 *
 * @param path rpc path that secret service is listening to
 * @return initiation status
 */
bool SecretManager::Init(const string &path) {
    Instance()._path = path;
    Instance()._rpcClient = new RPCClient<RPCSecretService, RPCSecretService::Client>(path);
    return true;
}

StringList SecretManager::getSupportedAttributes() {
    GLOG_INF("==========");
    bool isSucceed = false;
    StringList attributes;
    string errString;

    bool requestSucceeded = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        // getSupportedAttributes  @0() -> (attributes : List(Text));
        auto req = client.getSupportedAttributesRequest();
        req.send()
            .then(
                [&](RPCSecretService::GetSupportedAttributesResults::Reader &&result) {
                    isSucceed = true;
                    for (const auto &p : result.getAttributes()) {
                        attributes.push_back(p.cStr());
                    }
                },
                [&](kj::Exception &&ex) {
                    isSucceed = false;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    return attributes;
}

StringList SecretManager::getSupportedSchemas() {
    // getSupportedSchemas @1() -> (schemas : List(Text));
    GLOG_INF("==========");
    auto isSucceed = false;
    string errString;
    StringList schemas;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.getSupportedSchemasRequest();
        req.send()
            .then(
                [&](RPCSecretService::GetSupportedSchemasResults::Reader &&result) {
                    isSucceed = true;
                    for (const auto &s : result.getSchemas()) {
                        schemas.push_back(s.cStr());
                    }
                },
                [&](kj::Exception &&ex) {
                    isSucceed = false;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    return schemas;
}

StringList SecretManager::getSchemaKeys(const string &schemaName) {
    // getSchemaKeys @2(schema  : Text) -> (keys : List(Text));
    GLOG_INF("==========");
    auto isSucceed = false;
    string errString;
    StringList keys;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.getSchemaKeysRequest();
        req.send()
            .then(
                [&](RPCSecretService::GetSchemaKeysResults::Reader &&result) {
                    isSucceed = true;
                    for (const auto &k : result.getKeys()) {
                        keys.push_back(k.cStr());
                    }
                },
                [&](kj::Exception &&ex) {
                    isSucceed = false;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    return keys;
}

bool SecretManager::setSecret(const SecretItemBase &secret) {
    GLOG_INF("==========");
    // setSecret @3(item : RPCSecretItem) -> (result : RPCSecretResult);
    bool isSucceed = false;
    string errString;
    auto rpcResult = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.setSecretRequest();
        auto rpcSecret = req.initItem();
        rpcSecret.setLabel(secret.label);
        auto rSecrets = rpcSecret.initSecrets(secret.secrets.size());
        int i = 0;
        for (const auto &kv : secret.secrets) {
            rSecrets[i].setKey(kv.first);
            rSecrets[i++].setValue(kv.second);
        }
        auto rAttributes = rpcSecret.initAttributes(secret.attributes.size());
        i = 0;
        for (const auto &kv : secret.attributes) {
            rAttributes[i].setKey(kv.first);
            rAttributes[i++].setValue(kv.second);
        }
        req.send()
            .then(
                [&](RPCSecretService::SetSecretResults::Reader &&result) {
                    isSucceed = result.getResult().getSuccess();
                    if (!isSucceed) {
                        errString = result.getResult().getError().cStr();
                    }
                },
                [&](kj::Exception &&ex) {
                    isSucceed = false;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    return rpcResult && isSucceed;
}

SecretItemPtrList SecretManager::search(StrStrMap attributes) {
    GLOG_INF("==========");
    // search @4(attributes :List(RPCSecretAttribute)) -> (items : List(RPCSecretItem));
    bool isSucceed = false;
    string errString;
    SecretItemPtrList secretItems;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.searchRequest();
        auto rAttributes = req.initAttributes(attributes.size());
        int i = 0;
        for (const auto &kv : attributes) {
            rAttributes[i].setKey(kv.first);
            rAttributes[i++].setValue(kv.second);
        }
        req.send()
            .then(
                [&](RPCSecretService::SearchResults::Reader &&result) {
                    for (const auto &item : result.getItems()) {
                        string schema;
                        StrStrMap attributes;
                        StrStrMap secrets;
                        for (const auto &attr : item.getAttributes()) {
                            if (attr.getKey().cStr() == string("schema")) {
                                schema = attr.getValue().cStr();
                            }
                            attributes[attr.getKey().cStr()] = attr.getValue().cStr();
                        }
                        for (const auto &s : item.getSecrets()) {
                            secrets[s.getKey().cStr()] = s.getValue().cStr();
                        }
                        SecretItemPtr secret = nullptr;
                        if (schema == "password") {
                            secret = make_shared<PasswordSecret>();
                        } else if (schema == "dual_password") {
                            secret = make_shared<DualPasswordsecret>();
                        } else if (schema == "oauth2") {
                            secret = make_shared<OAuth2Secret>();
                        }

                        if (secret != nullptr) {
                            secret->label = item.getLabel().cStr();
                            secret->attributes = attributes;
                            secret->secrets = secrets;
                            secretItems.push_back(secret);
                        } else {
                            GLOG_ERR("secret is nullptr. schema: ", schema);
                        }
                    }
                    isSucceed = true;
                },
                [&](kj::Exception &&ex) {
                    isSucceed = true;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    GLOG_INF("search results: ", secretItems.size());
    for (const auto &secret : secretItems) {
        GLOG_INF(secret->toString());
    }
    return secretItems;
}

bool SecretManager::deleteSecret(StrStrMap attributes) {
    // deleteSecret @5(attributes :List(RPCSecretAttribute)) -> (result : RPCSecretResult);
    GLOG_INF("==========");
    bool isSucceed = false;
    string errString;
    auto rpcResult = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.deleteSecretRequest();
        auto rAttributes = req.initAttributes(attributes.size());
        req.send()
            .then(
                [&](RPCSecretService::DeleteSecretResults::Reader &&result) {
                    isSucceed = result.getResult().getSuccess();
                },
                [&](kj::Exception &&ex) {
                    isSucceed = false;
                    errString = ex.getDescription().cStr();
                })
            .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        GLOG_ERR("RPC ERROR: ", errString);
    }
    return rpcResult && isSucceed;
}

SecretItemPtrList SecretManager::getAccountSecrets(const uuid_t &accountId) {
    StrStrMap attributes;
    attributes["account_id"] = uuid_as_string(accountId);
    return this->search(attributes);
}

bool SecretManager::removeAccount(const uuid_t &accountId) {
    StrStrMap attributes;
    attributes["account_id"] = uuid_as_string(accountId);
    return this->deleteSecret(attributes);
}