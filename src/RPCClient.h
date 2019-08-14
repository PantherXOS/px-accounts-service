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

#include "AccountUtils.h"

//#define EXEC_INTERVAL 100

template <typename TBase, typename TClient>
class RPCClient {

public:
    explicit RPCClient(string addr) : rpcPath(std::move(addr)) {
    }

    bool performRequest(std::function<void(kj::AsyncIoContext &ctx, TClient &client)> func) {
//        std::cout << "request ... ";

        // check for timeout between password requests.
        // todo: need to find a way to automatically handle this issue. (remove if everything was OK)
//        auto elapsedDuration = chrono::high_resolution_clock::now() - lastExec;
//        auto elapsedMS = chrono::duration_cast<chrono::milliseconds>(elapsedDuration).count();
//        if (elapsedMS < EXEC_INTERVAL) {
//            std::this_thread::sleep_for(chrono::milliseconds(EXEC_INTERVAL - elapsedMS));
//        }

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
                LOG_ERR("Error: %s", ex.getDescription().cStr());
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
