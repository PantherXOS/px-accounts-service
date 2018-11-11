#include <iostream>

#include "RPCHandler.h"
#include <capnp/ez-rpc.h>

int main() {

    capnp::EzRpcServer server(kj::heap<RPCHandler>(), "127.0.0.1", 4444);
    auto& waitScope = server.getWaitScope();
    kj::NEVER_DONE.wait(waitScope);

}
