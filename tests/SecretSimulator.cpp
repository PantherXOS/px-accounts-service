//
// Created by Reza Alizadeh Majd on 2019-08-14.
//

#include <kj/debug.h>
#include "SecretSimulator.h"

kj::Promise<void> SecretSimulator::addParam(RPCSecretService::Server::AddParamContext context) {
    //    addParam         @7 (wallet : Text, application   : Text, param : RPCSecretParam)-> (result      : RPCSecretResult);
    RPCSecretService::AddParamParams::Reader requestParams = context.getParams();

            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");
            KJ_ASSERT(requestParams.hasParam(), "required param");
            KJ_ASSERT(requestParams.getParam().hasKey(), "required param");
            KJ_ASSERT(requestParams.getParam().hasValue(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();
    auto key = requestParams.getParam().getKey().cStr();
    auto val = requestParams.getParam().getValue().cStr();

    m_wallets[wallet][application][key] = val;

    RPCSecretResult::Builder res = context.getResults().initResult();
    res.setSuccess(true);
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::getParam(RPCSecretService::Server::GetParamContext context) {
    //     getParam         @3 (wallet : Text, application   : Text, paramKey : Text)       -> (paramValue  : Text);
    RPCSecretService::GetParamParams::Reader requestParams = context.getParams();


            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");
            KJ_ASSERT(requestParams.hasParamKey(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();
    auto key = requestParams.getParamKey().cStr();

            KJ_ASSERT(m_wallets.find(wallet) != m_wallets.end(), "not found");
            KJ_ASSERT(m_wallets[wallet].find(application) != m_wallets[wallet].end(), "not found");
            KJ_ASSERT(m_wallets[wallet][application].find(key) != m_wallets[wallet][application].end(), "not found");

    auto val = m_wallets[wallet][application][key];
    RPCSecretService::GetParamResults::Builder res = context.getResults();
    res.setParamValue(val);
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::editParam(RPCSecretService::Server::EditParamContext context) {
    //    editParam        @8 (wallet : Text, application   : Text, param : RPCSecretParam)-> (result      : RPCSecretResult);
    RPCSecretService::EditParamParams::Reader requestParams = context.getParams();

            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");
            KJ_ASSERT(requestParams.hasParam(), "required param");
            KJ_ASSERT(requestParams.getParam().hasKey(), "required param");
            KJ_ASSERT(requestParams.getParam().hasValue(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();
    auto key = requestParams.getParam().getKey().cStr();
    auto val = requestParams.getParam().getValue().cStr();

            KJ_ASSERT(m_wallets.find(wallet) != m_wallets.end(), "not found");
            KJ_ASSERT(m_wallets[wallet].find(application) != m_wallets[wallet].end(), "not found");
            KJ_ASSERT(m_wallets[wallet][application].find(key) != m_wallets[wallet][application].end(), "not found");

    m_wallets[wallet][application][key] = val;
    RPCSecretResult::Builder res = context.getResults().initResult();
    res.setSuccess(true);
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::delParam(RPCSecretService::Server::DelParamContext context) {
    //    delParam         @6 (wallet : Text, application   : Text, paramKey : Text)       -> (result      : RPCSecretResult);
    RPCSecretService::DelParamParams::Reader requestParams = context.getParams();

            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");
            KJ_ASSERT(requestParams.hasParamKey(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();
    auto key = requestParams.getParamKey().cStr();

            KJ_ASSERT(m_wallets.find(wallet) != m_wallets.end(), "not found");
            KJ_ASSERT(m_wallets[wallet].find(application) != m_wallets[wallet].end(), "not found");
            KJ_ASSERT(m_wallets[wallet][application].find(key) != m_wallets[wallet][application].end(), "not found");

    auto pos = m_wallets[wallet][application].find(key);
    m_wallets[wallet][application].erase(pos);
    RPCSecretResult::Builder res = context.getResults().initResult();
    res.setSuccess(true);
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::delApplication(RPCSecretService::Server::DelApplicationContext context) {
    //    delApplication   @5 (wallet : Text, application   : Text)                        -> (result      : RPCSecretResult);
    RPCSecretService::DelApplicationParams::Reader requestParams = context.getParams();

            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();

            KJ_ASSERT(m_wallets.find(wallet) != m_wallets.end(), "not found");
            KJ_ASSERT(m_wallets[wallet].find(application) != m_wallets[wallet].end(), "not found");

    auto pos = m_wallets[wallet].find(application);
    m_wallets[wallet].erase(pos);
    RPCSecretResult::Builder res = context.getResults().initResult();
    res.setSuccess(true);
    return kj::READY_NOW;
}

kj::Promise<void> SecretSimulator::getParams(RPCSecretService::Server::GetParamsContext context) {
    //    getParams        @2 (wallet : Text, application   : Text)                        -> (params      : List(Text));
    RPCSecretService::GetParamsParams::Reader requestParams = context.getParams();

            KJ_ASSERT(requestParams.hasWallet(), "required param");
            KJ_ASSERT(requestParams.hasApplication(), "required param");

    auto wallet = requestParams.getWallet().cStr();
    auto application = requestParams.getApplication().cStr();

            KJ_ASSERT(m_wallets.find(wallet) != m_wallets.end(), "not found");
            KJ_ASSERT(m_wallets[wallet].find(application) != m_wallets[wallet].end(), "not found");

    auto res = context.getResults().initParams(m_wallets[wallet][application].size());
    int i = 0;
    for (const auto &kv : m_wallets[wallet][application]) {
        res.set(i++, kv.first);
    }
    return kj::READY_NOW;
}
