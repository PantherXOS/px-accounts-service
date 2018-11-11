//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCHANDLER_H
#define PX_ACCOUNTS_SERVICE_RPCHANDLER_H

#include <interface/AccountReader.capnp.h>
#include <interface/AccountWriter.capnp.h>
#include "AccountParser.h"


class RPCHandler final: public AccountWriter::Server {

    // AccountReader Methods :
public:
    kj::Promise<void> list(ListContext ctx) override;
    kj::Promise<void> get(GetContext ctx) override;
    kj::Promise<void> setStatus(SetStatusContext ctx) override;
    kj::Promise<void> getStatus(GetStatusContext ctx) override;


    //AccountWriter Methods:
public:
    kj::Promise<void> add(AddContext ctx) override;
    kj::Promise<void> edit(EditContext ctx) override;
    kj::Promise<void> remove(RemoveContext ctx) override;

protected:
    bool parse(const Account::Reader &rpcAccount, PXParser::AccountObject *pAccount);
};


#endif //PX_ACCOUNTS_SERVICE_RPCHANDLER_H
