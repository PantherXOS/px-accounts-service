//
// Created by Reza Alizadeh Majd on 2019-01-20.
//

#ifndef PX_ACCOUNTS_SERVICE_PASSWORDHANDLER_H
#define PX_ACCOUNTS_SERVICE_PASSWORDHANDLER_H

#include "AccountDefinitions.h"
#include <iostream>
#include <kj/async-unix.h>
#include <capnp/rpc-twoparty.h>
//#include <capnp/ez-rpc.h>
#include <interface/password/interface.capnp.h>

using namespace std;

class ErrorReporter {

protected:
    StringList _errorList;

    inline void addError(const string &err) { _errorList.push_back(err); }
    inline void clearErrors() { _errorList.clear(); }

public:
    inline const StringList &ErrorList() { return  _errorList; }
};

struct PasswordStruct {
    bool inited = false;
    string act;
    string svc;
    string key;
    string val;
};

class PasswordHandler : public ErrorReporter {

protected:
    explicit PasswordHandler();

public:
    static PasswordHandler &Instance();
    inline static const StringList & LastErrors() { return _instance.ErrorList(); }
    ~PasswordHandler() = default;

    bool set(string act, string svc, string key, string val);
    PasswordStruct get(string act, string svc, string key);

protected:
    inline static string MAKE_USERNAME(const string &svc, const string &key) { return svc + string("_") + key; }

private:
    static PasswordHandler _instance;

    std::string _rpcPath;

//    kj::AsyncIoContext*            _ctx;
//    kj::Own<capnp::TwoPartyClient> _rpcClient;
//    PasswordInterface::Client      _client;

};


#endif //PX_ACCOUNTS_SERVICE_PASSWORDHANDLER_H
