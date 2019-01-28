#include <iostream>
#include <unistd.h>

#include "RPCServer.h"
#include "RPCHandler.h"
#include "PasswordHandler.h"
#include "AccountUtils.h"

#define RPC_SERVER_PATH         "~/.userdata/rpc/accounts"
#define RPC_CLIENT_PASS_PATH    "~/.userdata/rpc/password"


int main() {

    string rpcActPath = string("unix:") + PXUTILS::FILE::abspath(RPC_SERVER_PATH);
    string rpcPassPath = string("unix:") + PXUTILS::FILE::abspath(RPC_CLIENT_PASS_PATH);


    PasswordHandler::Init(rpcPassPath);

    RPCServer<RPCHandler> srv(rpcActPath);
    srv.start();
    std::cout << "Server started" << std::endl;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        sleep(1);
    }
#pragma clang diagnostic pop

    return 0;
}
