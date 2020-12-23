//
// Created by Reza Alizadeh Majd on 2019-08-14.
//

#include "SecretSimulator.h"

#include <Accounts/AccountUtils.h>
#include <kj/debug.h>

SecretSimulator::SecretSimulator() {
    _attributes.push_back("service");
    _attributes.push_back("username");
    _attributes.push_back("account_id");
    _attributes.push_back("schema");

    list<string> passwordKeys = {"password"};
    list<string> dualPasswordKey = {"user_password", "service_password"};
    list<string> oauth2Keys = {"client_id", "secret_id", "access_token", "refresh_token"};

    _schemas["password"] = passwordKeys;
    _schemas["dual_password"] = dualPasswordKey;
    _schemas["oauth2"] = oauth2Keys;
}

kj::Promise<void> SecretSimulator::getSupportedAttributes(GetSupportedAttributesContext context) {
    // getSupportedAttributes  @0() -> (attributes   : List(Text));
    auto results = context.getResults().initAttributes(_attributes.size());
    int i = 0;
    for (const auto& attr : _attributes) {
        results.set(i++, attr);
    }
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::getSupportedSchemas(GetSupportedSchemasContext context) {
    // getSupportedSchemas @1() -> (schemas : List(Text));
    auto results = context.getResults().initSchemas(3);
    int i = 0;
    for (const auto& schema : _schemas) {
        results.set(i++, schema.first);
    }
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::getSchemaKeys(GetSchemaKeysContext context) {
    // getSchemaKeys           @2(schema  : Text)                         -> (keys         : List(Text));
    auto request = context.getParams();
    KJ_ASSERT(request.hasSchema());

    auto schema = request.getSchema().cStr();
    KJ_ASSERT(_schemas.find(request.getSchema().cStr()) != _schemas.end());

    auto results = context.getResults().initKeys(_schemas[schema].size());
    int i = 0;
    for (const auto& key : _schemas[schema]) {
        results.set(i++, key);
    }
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::setSecret(SetSecretContext context) {
    // setSecret               @3(item : RPCSecretItem)                   -> (result       : RPCSecretResult);
    auto request = context.getParams();
    KJ_ASSERT(request.hasItem());

    auto item = request.getItem();
    KJ_ASSERT(item.hasLabel());
    KJ_ASSERT(item.hasAttributes());
    KJ_ASSERT(item.hasSecrets());

    MockSecret secret;
    secret.label = item.getLabel().cStr();

    list<string> receivedAttributes;
    string schema;
    for (const auto& attr : item.getAttributes()) {
        receivedAttributes.push_back(attr.getKey().cStr());
        if (attr.getKey().cStr() == "schema") {
            schema = attr.getValue();
        }
        secret.attributes[attr.getKey().cStr()] = attr.getValue().cStr();
    }
    for (auto& attr : _attributes) {
        KJ_ASSERT(VEXISTS(receivedAttributes, attr));
    }

    list<string> receivedSecrets;
    for (const auto& s : item.getSecrets()) {
        receivedSecrets.push_back(s.getKey().cStr());
        secret.secrets[s.getKey().cStr()] = s.getValue().cStr();
    }
    for (auto& key : _schemas[schema]) {
        KJ_ASSERT(VEXISTS(receivedSecrets, key));
    }

    _secrets.push_back(secret);

    auto result = context.getResults().initResult();
    result.setSuccess(true);

    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::search(SearchContext context) {
    // search                  @4(attributes :List(RPCSecretAttribute))   -> (items        : List(RPCSecretItem));
    auto req = context.getParams();
    KJ_ASSERT(req.hasAttributes());

    list<MockSecret> filtered;
    for (auto& secret : _secrets) {
        auto accepted = true;
        for (const auto& attr : req.getAttributes()) {
            auto key = attr.getKey().cStr();
            auto val = attr.getValue().cStr();
            auto pos = secret.attributes.find(key);
            if (pos == secret.attributes.end() || pos->second != val) {
                accepted = false;
                break;
            }
        }
        if (accepted) {
            filtered.push_back(secret);
        }
    }

    auto results = context.getResults().initItems(filtered.size());
    int i = 0;
    for (const auto& secret : filtered) {
        results[i].setLabel(secret.label);
        int j = 0;
        auto rAttributes = results[i].initAttributes(secret.attributes.size());
        for (const auto& attr : secret.attributes) {
            rAttributes[j].setKey(attr.first);
            rAttributes[j++].setValue(attr.second);
        }
        j = 0;
        auto rSecrets = results[i++].initSecrets(secret.secrets.size());
        for (const auto& s : secret.secrets) {
            rSecrets[j].setKey(s.first);
            rSecrets[j++].setValue(s.second);
        }
    }
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::deleteSecret(DeleteSecretContext context) {
    // deleteSecret            @5(attributes :List(RPCSecretAttribute))   -> (result       : RPCSecretResult);
    auto req = context.getParams();
    KJ_ASSERT(req.hasAttributes());

    auto it = _secrets.begin();
    while (it != _secrets.end()) {
        auto allowDelete = true;
        for (const auto& attr : req.getAttributes()) {
            auto key = attr.getKey().cStr();
            auto val = attr.getValue().cStr();
            auto pos = it->attributes.find(key);
            if (pos == it->attributes.end() || pos->second != val) {
                allowDelete = false;
                break;
            }
        }
        if (allowDelete) {
            it = _secrets.erase(it);
        }
    }
    auto result = context.getResults().initResult();
    result.setSuccess(true);
    return kj::READY_NOW;
}
