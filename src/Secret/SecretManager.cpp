//
// Created by Reza Alizadeh Majd on 2019-06-15.
//

#include "SecretManager.h"
#include "../Accounts/AccountUtils.h"


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

/**
 *
 * @param act account name
 * @param svc service name
 * @param key parameter key
 * @return status indicates that if key is exists
 */
bool SecretManager::IsExists(const string &act, const string &svc, const string &key) const {
    string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
    return this->checkParam(_currentUserr, act, paramKey);
}

/**
 *
 * @param act account name
 * @param svc service name
 * @param key parameter key
 * @param val parameter value
 * @return set parameter status
 */
bool SecretManager::Set(const string &act, const string &svc, const string &key, const string &val) {
    string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
    if (this->IsExists(act, svc, key)) {
        return this->editParam(_currentUserr, act, paramKey, val);
    } else {
        return this->addParam(_currentUserr, act, paramKey, val);
    }
    GLOG_INF("new secret saved: [", act, "][", svc, "][", key, "]");
    return true;
}

/**
 *
 * @param act    account name
 * @param params string-based mapping for parameters to set
 * @return set status result for all account parameter
 */
bool SecretManager::SetAccount(const string &act, const map<string, string> &params) {
    bool result = true;
    for (const auto &kv : params) {
        if (this->checkParam(_currentUserr, act, kv.first)) {
            result = result && this->editParam(_currentUserr, act, kv.first, kv.second);
        } else {
            result = result && this->addParam(_currentUserr, act, kv.first, kv.second);
        }
    }
    return result;
}

/**
 *
 * @param act account name
 * @param svc service name
 * @param key parameter key
 * @return value of requested parameter
 */
string SecretManager::Get(const string &act, const string &svc, const string &key) {
    string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
    try {
        return this->getParam(_currentUserr, act, paramKey);
    } catch (std::exception &) {
        return string();
    }
}

/**
 *
 * @param act account name
 * @return string-based key-value mapping about all account parameters
 */
map<string, string> SecretManager::GetAccount(const string &act) {
    map<string, string> result;
    try {
        auto keyList = this->getParams(_currentUserr, act);
        for (const auto &key : keyList) {
            string val = this->getParam(_currentUserr, act, key);
            result[key] = val;
        }
    } catch (std::exception &) {
        result.clear();
    }
    return result;
}

/**
 *
 * @param act account name
 * @param svc service name
 * @param key parameter key
 * @return parameter removal status
 */
bool SecretManager::Remove(const string &act, const string &svc, const string &key) {
    bool result = true;
    if (this->IsExists(act, svc, key)) {
        string paramKey = SecretManager::MAKE_PARAM_KEY(svc, key);
        result = this->delParam(_currentUserr, act, paramKey);
    }
    return result;
}

/**
 *
 * @param act account name
 * @return account removal status
 */
bool SecretManager::RemoveAccount(const string &act) {
    bool result = true;
    if (this->checkApplication(_currentUserr, act)) {
        result = this->delApplication(_currentUserr, act);
    }
    return result;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @param key parameter key
 * @return parameter existence status
 */
bool SecretManager::checkParam(const string &wlt, const string &app, const string &key) const {
    bool result = false;
    try {
        this->getParam(wlt, app, key, true);
        result = true;
    } catch (const std::logic_error &err) {
        result = false;
    }
    return result;
}

/**
 * check if an application is exists, using `getParam` RPC interface.
 * @param wlt   wallet name
 * @param app   application
 * @return      application existence status
 */
bool SecretManager::checkApplication(const string &wlt, const string &app) const {
    bool result;
    try {
        this->getParams(wlt, app);
        result = true;
    } catch (const std::logic_error &err) {
        result = false;
    }
    return result;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @param key parameter key
 * @param val parameter value
 * @return add parameter result
 */
bool SecretManager::addParam(const string &wlt, const string &app, const string &key, const string &val) const {
    // addParam @7 (wallet : Text, application: Text, param : RPCSecretParam) -> (result: RPCSecretResult);
    GLOG_INF("wlt: ", wlt, " - app: ", app, " - key: ", key, " - val: ", val);
    bool isSucceed = false;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
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
    if (!requestSucceed || !isSucceed) {
        GLOG_ERR("request failed: ", errString);
    }
    return isSucceed;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @param key parameter key
 * @param val parameter value
 * @return parameter modification status
 */
bool SecretManager::editParam(const string &wlt, const string &app, const string &key, const string &val) const {
    // editParam @8 (wallet : Text, application   : Text, param : RPCSecretParam) -> (result: RPCSecretResult);
    GLOG_INF("wlt: ", wlt, " - app: ", app, " - key: ", key, " - val: ", val);
    bool isSucceed = false;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
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
    if (!requestSucceed || !isSucceed) {
        GLOG_ERR("editParam failed: ", errString);
    }
    return isSucceed;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @param key parameter key
 * @param ignoreExistance ignore writing error log if parameter is exists
 * @return
 */
string SecretManager::getParam(const string &wlt, const string &app, const string &key, bool ignoreExistance) const {
    // getParam @3 (wallet : Text, application : Text, paramKey : Text)-> (paramValue  : Text);
    GLOG_INF("wlt: ", wlt, " - app: ", app, " - key: ", key);
    bool isSucceed = false;
    string paramVal;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
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
                    if (!ignoreExistance) {
                        GLOG_ERR("getParam failed: %s", err.getDescription().cStr()); // NOLINT(bugprone-lambda-function-name)
                    }
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!requestSucceed || !isSucceed) {
        throw std::logic_error(errString);
    }
    return paramVal;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @throws std::logic_error in case of RPC request failure
 * @return list af application parameters
 */
list<string> SecretManager::getParams(const string &wlt, const string &app) const {
    // getParams        @2 (wallet : Text, application   : Text -> (params : List(Text));
    GLOG_INF("wlt: ", wlt, " - app: ", app);
    bool isSucceed = false;
    list<string> result;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
        auto req = client.getParamsRequest();
        req.setWallet(wlt);
        req.setApplication(app);
        req.send()
                .then([&](auto &&resp) {
                    isSucceed = true;
                    for (const auto &key : resp.getParams()) {
                        result.push_back(key.cStr());
                    }
                }, [&](kj::Exception &&err) {
                    isSucceed = false;
                    errString = err.getDescription().cStr();
                })
                .wait(ctx.waitScope);
    });
    if (!requestSucceed || !isSucceed) {
        throw std::logic_error(errString);
    }
    return result;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @param key parameter key
 * @return parameter deletion status
 */
bool SecretManager::delParam(const string &wlt, const string &app, const string &key) const {
    // delParam @6 (wallet : Text, application: Text, paramKey : Text) -> (result: RPCSecretResult);
    GLOG_INF("wlt: ", wlt, " - app: ", app, " - key: ", key);
    bool isSucceed = false;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
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
    if (!requestSucceed || !isSucceed) {
        GLOG_ERR("Error: ", errString);
    }
    return isSucceed;
}

/**
 *
 * @param wlt wallet name
 * @param app application name
 * @return application removal status
 */
bool SecretManager::delApplication(const string &wlt, const string &app) const {
    // delApplication @5 (wallet : Text, application   : Text) -> (result: RPCSecretResult);
    GLOG_INF("wlt: ", wlt, " - app: ", app);
    bool isSucceed = false;
    string errString;
    bool requestSucceed = _rpcClient->performRequest([&](kj::AsyncIoContext &ctx, RPCSecretService::Client &client) {
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
    if (!requestSucceed || !isSucceed) {
        GLOG_ERR("Error: ", errString);
    }
    return isSucceed;
}
