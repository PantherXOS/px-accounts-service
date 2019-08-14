//
// Created by Reza Alizadeh Majd on 2019-08-14.
//

#ifndef PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H
#define PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H

#include <interface/Secret.capnp.h>

#include <map>
#include <string>
#include <iostream>
#include <list>

using namespace std;


class SecretSimulator final : public RPCSecretService::Server {
private:
    map<string, map<string, map<string, string> > > m_wallets;

protected:
    kj::Promise<void> addParam(AddParamContext context) override;

    kj::Promise<void> getParams(GetParamsContext context) override;

    kj::Promise<void> getParam(GetParamContext context) override;

    kj::Promise<void> editParam(EditParamContext context) override;

    kj::Promise<void> delParam(DelParamContext context) override;

    kj::Promise<void> delApplication(DelApplicationContext context) override;

};

#endif //PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H
