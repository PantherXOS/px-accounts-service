//
// Created by Reza Alizadeh Majd on 11/12/18.
//

#include "RPCServer.h"
//#include "RPCHandler.h"
//#include "AccountUtils.h"

//#include <capnp/rpc-twoparty.h>
//#include <kj/async-unix.h>

//using namespace std;


//RPCServer::RPCServer(const std::string &addr) {
//
//    if (PXUTILS::PATH::isunix(addr)) {
//        string addrPath = PXUTILS::PATH::unix2path(addr);
//        string baseDir = PXUTILS::FILE::basedir(addrPath);
//
//        string mkpathCMD = string("mkdir -p ") + baseDir;
//        system(mkpathCMD.c_str());
//
//        if (PXUTILS::FILE::exists(addrPath)) {
//            PXUTILS::FILE::remove(addrPath);
//        }
//    }
//    this->address = addr;
//}

//void RPCServer::start() {
//    if (!isRunning) {
//        tServer = std::thread([](void* param){
//            auto instance = static_cast<RPCServer*>(param);
//
//            kj::AsyncIoContext ctx = kj::setupAsyncIo();
//            auto &waitScope = ctx.waitScope;
//            auto &network = ctx.provider->getNetwork();
//            auto addr = network.parseAddress(instance->address).wait(waitScope);
//            auto listener = addr->listen();
//            capnp::TwoPartyServer server(kj::heap<RPCHandler>());
//            auto serverPromise = server.listen(*listener);
//
//            instance->isRunning = true;
//
//            while (instance->isRunning) {
//                waitScope.poll();
//            }
//        }, this);
//    }
//}

//void RPCServer::stop() {
//    if (isRunning) {
//        isRunning = false;
//        tServer.join();
//    }
//}

//void RPCServer::restart() {
//    stop();
//    start();
//}
