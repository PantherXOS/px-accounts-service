#include <utility>

//
// Created by Reza Alizadeh Majd on 2019-01-24.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCCLIENT_H
#define PX_ACCOUNTS_SERVICE_RPCCLIENT_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <chrono>
#include <functional>

using namespace std;

#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>

#include "Accounts/AccountUtils.h"

//#define EXEC_INTERVAL 100

/**
 * @brief Template class to create an RPCClient
 * @tparam TBase  RPC interface class
 * @tparam TClient  RPC interface client
 */
template <typename TBase, typename TClient>
class RPCClient {

public:
    /**
     * constructs an RPCClient with specified address
     * @param addr address that client wants to connect
     */
    explicit RPCClient(string addr) : rpcPath(std::move(addr)) {
    }

    /**
     * function that performs a RPCRequest
     * @param func function to run after connection
     * @return whether the RPC connection is successfully established or not
     */
    bool performRequest(std::function<void(kj::AsyncIoContext &ctx, TClient &client)> func) {

        bool _isSucceed = false;
        auto thClient = std::thread([&]() {
            try {
                auto ctx = kj::setupAsyncIo();
                auto netAddr = ctx.provider->getNetwork().parseAddress(rpcPath).wait(ctx.waitScope);
                auto stream = netAddr->connect().wait(ctx.waitScope);
                auto rpc = kj::heap<capnp::TwoPartyClient>(*stream);
                TClient client = rpc->bootstrap().template castAs<TBase>();
                func(ctx, client);
                _isSucceed = true;
            }
            catch (kj::Exception &ex) {
                GLOG_ERR("Error: ", ex.getDescription().cStr());
            }
        });
        thClient.join();
//        lastExec = chrono::high_resolution_clock::now();
        return _isSucceed;
    }

protected:
    string rpcPath;
//    chrono::high_resolution_clock::time_point lastExec;

};


#endif //PX_ACCOUNTS_SERVICE_RPCCLIENT_H
