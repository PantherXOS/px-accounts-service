//
// Created by Reza Alizadeh Majd on 11/12/18.
//

#include "RPCServer.h"
#include "RPCHandler.h"

#include <capnp/rpc-twoparty.h>
#include <kj/async-unix.h>

using namespace std;

#define RPC_PATH          "~/.userdata/rpc"
#define RPC_SOCKET_PATH   RPC_PATH "/accounts"
#define RPC_MKDIR_CMD     "mkdir -p " RPC_PATH


RPCServer::RPCServer(const std::string &addr) {
    this->address = addr;
    if (addr.empty()) {

        system(RPC_MKDIR_CMD);
        string rpcPath =  PXUTILS::FILE::abspath(RPC_SOCKET_PATH);
        if (PXUTILS::FILE::exists(rpcPath)) {
            PXUTILS::FILE::remove(rpcPath);
        }
        this->address = string("unix:") + rpcPath;
    }
}

void RPCServer::start() {
    if (!isRunning) {
        tServer = std::thread([](void* param){
            auto instance = static_cast<RPCServer*>(param);

            kj::AsyncIoContext asyncIO = kj::setupAsyncIo();
            auto &waitScope = asyncIO.waitScope;
            auto &ioProvider = *asyncIO.provider;
            auto &network = ioProvider.getNetwork();
            auto addr = network.parseAddress(instance->address).wait(waitScope);
            auto listner = addr->listen();
            capnp::TwoPartyServer server(kj::heap<RPCHandler>());
            auto serverPromise = server.listen(*listner);

            instance->isRunning = true;

            while (instance->isRunning) {
                waitScope.poll();
            }
        }, this);
    }

}

void RPCServer::stop() {
    if (isRunning) {
        isRunning = false;
        tServer.join();
    }
}

void RPCServer::restart() {
    stop();
    start();
}