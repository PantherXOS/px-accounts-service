//
// Created by Reza Alizadeh Majd on 2019-01-24.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCCLIENT_H
#define PX_ACCOUNTS_SERVICE_RPCCLIENT_H

#include <iostream>
#include <map>
#include <vector>
#include <thread>
#include <functional>

using namespace std;

#include <kj/async-io.h>
#include <capnp/rpc-twoparty.h>

template <typename TBase, typename TClient>
class RPCClient {

public:
    explicit RPCClient(const string &path) : rpcPath(string("unix:") + path) {
    }

    void performRequest(std::function<void(kj::AsyncIoContext& ctx, TClient &client)> func) {

        auto thClient = std::thread([&]() {
            auto ctx = kj::setupAsyncIo();
            auto netAddr = ctx.provider->getNetwork().parseAddress(rpcPath).wait(ctx.waitScope);
            auto stream = netAddr->connect().wait(ctx.waitScope);
            auto rpc = kj::heap<capnp::TwoPartyClient>(*stream);
            TClient client = rpc->bootstrap().template castAs<TBase>();
            func(ctx, client);
        });
        thClient.join();
    }

protected:
    string rpcPath;

};


#endif //PX_ACCOUNTS_SERVICE_RPCCLIENT_H
