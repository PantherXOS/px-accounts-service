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


class SecretManager {

protected:
    explicit SecretManager();

    virtual ~SecretManager();

public:
    static SecretManager &Instance();

    static bool Init(const string &path);


    bool IsExists(const string &act, const string &svc, const string &key) const;

    bool Set(const string &act, const string &svc, const string &key, const string &val);

    bool SetAccount(const string &act, const map<string, string> &params);

    string Get(const string &act, const string &svc, const string &key);

    map<string, string> GetAccount(const string &act);

    bool Remove(const string &act, const string &svc, const string &key);

    bool RemoveAccount(const string &act);


protected:
    bool checkParam(const string &wlt, const string &app, const string &key) const;

    bool addParam(const string &wlt, const string &app, const string &key, const string &val) const;

    bool editParam(const string &wlt, const string &app, const string &key, const string &val) const;

    string getParam(const string &wlt, const string &app, const string &key, bool ignoreExistance = false) const;

    list<string> getParams(const string &wlt, const string &app) const;

    bool delParam(const string &wlt, const string &app, const string &key) const;

    bool delApplication(const string &wallet, const string &application) const;

    inline static string MAKE_PARAM_KEY(const string &svc, const string &key) { return svc + string("_") + key; }

private:
    string _path;
    string _currentUserr;
    RPCClient<RPCSecretService, RPCSecretService::Client> *_rpcClient = nullptr;

};


#endif //PX_ACCOUNTS_SERVICE_SECRETMANAGER_H
