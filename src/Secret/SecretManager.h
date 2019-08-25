//
// Created by Reza Alizadeh Majd on 2019-06-15.
//

#ifndef PX_ACCOUNTS_SERVICE_SECRETMANAGER_H
#define PX_ACCOUNTS_SERVICE_SECRETMANAGER_H

#include <iostream>
#include <list>
#include "../RPCClient.h"
#include <interface/Secret.capnp.h>

using namespace std;


/// @brief Base class for managing rpc interactions with Secret Service
class SecretManager {

protected:
    explicit SecretManager();

    virtual ~SecretManager();

public:
    /// @brief static method to singleton instance of SecretManager
    static SecretManager &Instance();

    /// @brief init rpc instance to interact with secret service
    static bool Init(const string &path);

    /// @brief check if key exists in secret service
    bool IsExists(const string &act, const string &svc, const string &key) const;

    /// @brief set data for a key
    bool Set(const string &act, const string &svc, const string &key, const string &val);

    /// @brief set all keys for an account
    bool SetAccount(const string &act, const map<string, string> &params);

    /// @brief get value of a key
    string Get(const string &act, const string &svc, const string &key);

    /// @brief get all keys for an account
    map<string, string> GetAccount(const string &act);

    /// @brief remove a key from secret service
    bool Remove(const string &act, const string &svc, const string &key);

    /// @brief remove all keys for an account
    bool RemoveAccount(const string &act);

protected:
    /// @brief low-level method to check if a parameter exists for an application inside specific wallet
    bool checkParam(const string &wlt, const string &app, const string &key) const;

    /// @brief low-level method to check if an application exists inside specific wallet
    bool checkApplication(const string &wlt, const string &app) const;

    /// @brief low-level method to add a new parameter to an application inside specific wallet
    bool addParam(const string &wlt, const string &app, const string &key, const string &val) const;

    /// @brief low-level method to modify existing parameter for an application inside specific wallet
    bool editParam(const string &wlt, const string &app, const string &key, const string &val) const;

    /// @brief low-level method to get value of a parameter of an application inside specific wallet
    string getParam(const string &wlt, const string &app, const string &key, bool ignoreExistance = false) const;

    /// @brief low-level method to get all parameter keys for an application inside specific wallet
    list<string> getParams(const string &wlt, const string &app) const;

    /// @brief low-level method to delete an existing parameter from an application inside specific wallet
    bool delParam(const string &wlt, const string &app, const string &key) const;

    /// @brief low-level method to delete all parameters of an application inside specific wallet
    bool delApplication(const string &wallet, const string &application) const;

    /// @brief helper method to generate a key for secret srvice based on service name and parameter key
    inline static string MAKE_PARAM_KEY(const string &svc, const string &key) { return svc + string("_") + key; }

private:
    string _path;
    string _currentUserr;
    RPCClient<RPCSecretService, RPCSecretService::Client> *_rpcClient = nullptr;
};

#endif //PX_ACCOUNTS_SERVICE_SECRETMANAGER_H
