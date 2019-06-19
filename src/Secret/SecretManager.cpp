//
// Created by Reza Alizadeh Majd on 2019-06-15.
//

#include "SecretManager.h"
#include "../AccountUtils.h"


SecretManager::SecretManager() {
    this->_currentUserr = PXUTILS::SYSTEM::current_user();
}

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

bool SecretManager::Init(const string &path) {

    Instance()._path = path;
    Instance()._rpcClient = new RPCClient<RPCSecretService, RPCSecretService::Client>(path);
    return true;
}

bool SecretManager::IsExists(const string &act, const string &svc, const string &key) const {
    bool result = false;
    try {
        string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
        string result = this->getParam(_currentUserr, act, paramKey);
        result = true;
    } catch (std::exception &err) {
        result = false;
    }
    return result;
}

bool SecretManager::Set(const string &act, const string &svc, const string &key, const string &val) {
    string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
    if (!this->IsExists(act, svc, key)) {
        return this->editParam(_currentUserr, act, paramKey, val);
    } else {
        return this->addParam(_currentUserr, act, paramKey, val);
    }
}

string SecretManager::Get(const string &act, const string &svc, const string &key) {
    string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
    try {
        return this->getParam(_currentUserr, act, paramKey);
    } catch (std::exception &) {
        return string();
    }

}

bool SecretManager::Remove(const string &act, const string &svc, const string &key) {
    bool result = true;
    if (this->IsExists(act, svc, key)) {
        string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
        result = this->delParam(_currentUserr, act, paramKey);
    }
    return result;
}

bool SecretManager::removeAccount(const string &act) {
    return this->delApplication(_currentUserr, act);
}

bool SecretManager::addParam(const string &wlt, const string &app, const string &key, const string &val) const {
    // addParam @7 (wallet : Text, application: Text, param : RPCSecretParam) -> (result: RPCSecretResult);
    LOG_INF("wlt: %s - app: %s - key: %s - val: %s", wlt.c_str(), app.c_str(), key.c_str(), val.c_str());
    bool isSucceed = false;
    string errString;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.addParamRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        auto param = req.initParam();
        param.setKey(key);
        param.setValue(val);
        req.send()
                .then([&](auto &&result) {
                    isSucceed = result.getResult().getSuccess();
                    errString = result.getResult().getError().cStr();
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        LOG_ERR("request failed: %s", errString.c_str());
    }
    return isSucceed;
}

bool SecretManager::editParam(const string &wlt, const string &app, const string &key, const string &val) const {
    // editParam @8 (wallet : Text, application   : Text, param : RPCSecretParam) -> (result: RPCSecretResult);
    LOG_INF("wlt: %s - app: %s - key: %s - val: %s", wlt.c_str(), app.c_str(), key.c_str(), val.c_str());
    bool isSucceed = false;
    string errString;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.editParamRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        auto param = req.initParam();
        param.setKey(key);
        param.setValue(val);
        req.send()
                .then([&](auto &&resp) {
                    isSucceed = resp.getResult().getSuccess();
                    errString = resp.getResult().getError().cStr();
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        LOG_ERR("editParam failed: %s", errString.c_str());
    }
    return isSucceed;
}

string SecretManager::getParam(const string &wlt, const string &app, const string &key) const {
    // getParam @3 (wallet : Text, application : Text, paramKey : Text)-> (paramValue  : Text);
    LOG_INF("wlt: %s - app: %s - key: %s", wlt.c_str(), app.c_str(), key.c_str());
    bool isSucceed = false;
    string paramVal;
    string errString;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.getParamRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        req.setParamKey(key);
        req.send()
                .then([&](auto &&result) {
                    isSucceed = true;
                    paramVal = result.getParamValue().cStr();
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    LOG_ERR("getParam failed: %s", err.getDescription().cStr());
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        throw new std::logic_error(errString);
    }
    return paramVal;
}

bool SecretManager::delParam(const string &wlt, const string &app, const string &key) const {
    // delParam @6 (wallet : Text, application: Text, paramKey : Text) -> (result: RPCSecretResult);
    LOG_INF("wlt: %s - app: %s - key: %s", wlt.c_str(), app.c_str(), key.c_str());
    bool isSucceed = false;
    string errString;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.delParamRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        req.setParamKey(key);
        req.send()
                .then([&](auto &&resp) {
                    isSucceed = resp.getResult().getSuccess();
                    errString = resp.getResult().getError().cStr();
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        LOG_ERR("delParam error: %s", errString.c_str());
    }
    return isSucceed;
}

bool SecretManager::delApplication(const string &wlt, const string &app) const {
    // delApplication @5 (wallet : Text, application   : Text) -> (result: RPCSecretResult);
    LOG_INF("wlt: %s - app: %s", wlt.c_str(), app.c_str());
    bool isSucceed = false;
    string errString;
    _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.delApplicationRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        req.send()
                .then([&](auto &&resp) {
                    isSucceed = resp.getResult().getSuccess();
                    errString = resp.getResult().getError().cStr();
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!isSucceed) {
        LOG_ERR("delApplication failed: %s", errString.c_str());
    }
    return isSucceed;
}
