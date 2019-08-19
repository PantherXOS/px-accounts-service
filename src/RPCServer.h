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


/**
 * @brief Template class for handling RPC requests
 * @tparam HNDLR RPCHandler class type
 */
template<class HNDLR>
class RPCServer {

public:
    /**
     * @brief constructor for creating new server
     *
     * @param addr address that server needs to initated on
     */
    explicit RPCServer(std::string addr) : address(std::move(addr)) {
        if (PXUTILS::PATH::isunix(address)) {
            string rpcPath = PXUTILS::PATH::unix2path(address);
            if (PXUTILS::FILE::exists(rpcPath)) {
                PXUTILS::FILE::remove(rpcPath);
            }
        }
    }

    /// @brief server destructor
    virtual ~RPCServer() {
        this->stop();
    }

public:
    /// @brief start processing RPC requests
    void start() {
        if (!isRunning) {
            tServer = std::thread([](void *param) {
                auto instance = static_cast<RPCServer *>(param);

                kj::AsyncIoContext ctx = kj::setupAsyncIo();
                auto &waitScope = ctx.waitScope;
                auto &network = ctx.provider->getNetwork();
                auto addr = network.parseAddress(instance->address).wait(waitScope);
                auto listener = addr->listen();
                capnp::TwoPartyServer server(kj::heap<HNDLR>());
                auto serverPromise = server.listen(*listener);

                instance->isRunning = true;

                while (instance->isRunning) {
                    waitScope.poll();
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
            }, this);
        }
    }

    /// @brief stop processing RPC requests
    void stop() {
        if (isRunning) {
            isRunning = false;
            tServer.join();
        }
    }

    /// @brief restart the server
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
