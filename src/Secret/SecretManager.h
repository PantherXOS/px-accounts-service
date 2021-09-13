//
// Created by Reza Alizadeh Majd on 2019-06-15.
//

#ifndef PX_ACCOUNTS_SERVICE_SECRETMANAGER_H
#define PX_ACCOUNTS_SERVICE_SECRETMANAGER_H

#include <Utils/Utils.h>
#include <interface/Secret.capnp.h>

#include <iostream>
#include <list>

#include "../RPCClient.h"

using namespace std;

struct SecretItemBase {
    string label;
    StrStrMap secrets;
    StrStrMap attributes;

    string schema() const {
        auto it = attributes.find("schema");
        if (it != attributes.end()) {
            return it->second;
        }
        return string();
    }

    void updateAttributes(const string &accountId, const string &service, const StrStrMap &others);

    bool is(const string &type) const { return this->schema() == type; }

    template <typename T>
    shared_ptr<T> as() {
        auto *childPtr = dynamic_cast<T *>(this);
        return std::make_shared<T>(*childPtr);
    }

    string toString(bool pretty = false) const;

    explicit SecretItemBase() = default;
    virtual ~SecretItemBase() = default;

   protected:
    string getSecret(const string &key) const;
    void setSecret(const string &key, const string &val);
};

typedef std::shared_ptr<SecretItemBase> SecretItemPtr;
typedef list<SecretItemPtr> SecretItemPtrList;

struct PasswordSecret : public SecretItemBase {
    // explicit PasswordSecret() = default;
    explicit PasswordSecret() { attributes["schema"] = "password"; }

    string password() const { return getSecret(_passwordKey); }
    void setPassword(const string &value) { setPassword(_passwordKey); }

   private:
    string _passwordKey = "password";
};

struct DualPasswordSecret : public SecretItemBase {
    explicit DualPasswordSecret() { attributes["schema"] = "dual_password"; }

    string userPassword() const { return getSecret(_userPasswordKey); }
    void setUserPassword(const string &value) { setSecret(_userPasswordKey, value); }
    bool isUserPassword(const string &key) const {
        // return EXISTS(attributes, "user_password_key") && attributes.find("user_password_key"] == key;
        return attributes.find("user_password_key") != attributes.end() &&
               attributes.find("user_pasword_key")->second == key;
    }

    string servicePassword() const { return this->getSecret(this->_servicePasswordKey); }
    void setServicePassword(const string &value) { this->setSecret(this->_servicePasswordKey, value); }
    bool isServicePassword(const string &key) const {
        return attributes.find("service_password_key") != attributes.end() &&
               attributes.find("service_password_key")->second == key;
        // return EXISTS(attributes, "service_password_key") && attributes["service_password_key"] == key;
    }

    string getMatchingPassword(const string &key) {
        if (isUserPassword(key)) {
            return userPassword();
        } else if (isServicePassword(key)) {
            return servicePassword();
        } else {
            return string();
        }
    }

   private:
    string _userPasswordKey = "user_password";
    string _servicePasswordKey = "service_password";
};

struct OAuth2Secret : public SecretItemBase {
    explicit OAuth2Secret() { attributes["schema"] = "oauh2"; }

    string clientId() const { return getSecret(_clientIdKey); }
    void setClientId(const string &value) { setSecret(_clientIdKey, value); }

    string secretId() const { return getSecret(_secretIdKey); }
    void setSecretId(const string &value) { setSecret(_secretIdKey, value); }

    string accessToken() const { return getSecret(_accessTokenKey); }
    void setAccessToken(const string &value) { setSecret(_accessTokenKey, value); }

    string refreshToken() const { return getSecret(_refreshTokenKey); }
    void setRefreshToken(const string &value) { setSecret(_refreshTokenKey, value); }

   private:
    string _clientIdKey = "client_id";
    string _secretIdKey = "secret_id";
    string _accessTokenKey = "access_token";
    string _refreshTokenKey = "refresh_token";
};

/// @brief Base class for managing rpc interactions with Secret Service
class SecretManager {
   protected:
    explicit SecretManager() = default;
    virtual ~SecretManager();

   public:
    /// @brief static method to singleton instance of SecretManager
    static SecretManager &Instance();

    /// @brief init rpc instance to interact with secret service
    static bool Init(const string &path);

    StringList getSupportedAttributes();

    StringList getSupportedSchemas();

    // getSchemaKeys           @2(schema  : Text)                         -> (keys         : List(Text));
    StringList getSchemaKeys(const string &schemaName);

    // setSecret               @3(item : RPCSecretItem)                   -> (result       : RPCSecretResult);
    bool setSecret(const SecretItemBase &secret);

    // search                  @4(attributes :List(RPCSecretAttribute))   -> (items        : List(RPCSecretItem));
    SecretItemPtrList search(StrStrMap attributes);

    // deleteSecret            @5(attributes :List(RPCSecretAttribute))   -> (result       : RPCSecretResult);
    bool deleteSecret(StrStrMap attributes);

   public:
    SecretItemPtrList getAccountSecrets(const uuid_t &accountId);
    bool removeService(const uuid_t &accountId, const string &svcName);
    bool removeAccount(const uuid_t &accountId);

   private:
    string _path;
    RPCClient<RPCSecretService, RPCSecretService::Client> *_rpcClient = nullptr;
};

#endif  // PX_ACCOUNTS_SERVICE_SECRETMANAGER_H
