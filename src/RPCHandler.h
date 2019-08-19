//
// Created by Reza Alizadeh Majd on 11/5/18.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCHANDLER_H
#define PX_ACCOUNTS_SERVICE_RPCHANDLER_H

#include <interface/AccountReader.capnp.h>
#include <interface/AccountWriter.capnp.h>
#include "AccountParser.h"

/// @brief Handler class for receive RPC requests
class RPCHandler final: public AccountWriter::Server {

    // AccountReader Methods :
public:
    /// @brief RPC interface to get list of accounts
    kj::Promise<void> list(ListContext ctx) override;

    /// @brief RPC interface to get account details
    kj::Promise<void> get(GetContext ctx) override;

    /// @brief RPC interface to set account status
    kj::Promise<void> setStatus(SetStatusContext ctx) override;

    /// @brief RPC interface to get account status
    kj::Promise<void> getStatus(GetStatusContext ctx) override;


    //AccountWriter Methods:
public:
    /// @brief RPC interface to create new account
    kj::Promise<void> add(AddContext ctx) override;

    /// @brief RPC interface to modify existing account
    kj::Promise<void> edit(EditContext ctx) override;

    /// @brief RPC interface to remove existing account
    kj::Promise<void> remove(RemoveContext ctx) override;

public:
    /// @brief helper method to convert RPC formatted account to AccountObject
    static bool RPC2ACT(const Account::Reader &rpc, AccountObject &act);

    /// @brief helper method to convert AccountObject to RPC format
    static bool ACT2RPC(const AccountObject &act, Account::Builder &rpc);
};

#endif //PX_ACCOUNTS_SERVICE_RPCHANDLER_H
