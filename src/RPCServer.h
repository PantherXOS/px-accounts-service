#include <utility>

//
// Created by Reza Alizadeh Majd on 11/12/18.
//

#ifndef PX_ACCOUNTS_SERVICE_RPCSERVER_H
#define PX_ACCOUNTS_SERVICE_RPCSERVER_H

#include <thread>
#include <capnp/rpc-twoparty.h>
#include <kj/async-unix.h>

#include "AccountUtils.h"



template <class T>
class RPCServer {

public:
    explicit RPCServer(std::string addr) : address(std::move(addr)) {}

public:
    void start() {
        if (!isRunning) {
            tServer = std::thread([](void* param){
                auto instance = static_cast<RPCServer*>(param);

                kj::AsyncIoContext ctx = kj::setupAsyncIo();
                auto &waitScope = ctx.waitScope;
                auto &network = ctx.provider->getNetwork();
                auto addr = network.parseAddress(instance->address).wait(waitScope);
                auto listener = addr->listen();
                capnp::TwoPartyServer server(kj::heap<T>());
                auto serverPromise = server.listen(*listener);

                instance->isRunning = true;

                while (instance->isRunning) {
                    waitScope.poll();
                }
            }, this);
        }
    }
    void stop() {
        if (isRunning) {
            isRunning = false;
            tServer.join();
        }
    }
    void restart() {
        stop();
        start();
    }

private:
    std::string address;
    std::thread tServer;
    bool isRunning = false;

};


#endif //PX_ACCOUNTS_SERVICE_RPCSERVER_H
