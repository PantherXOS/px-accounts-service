#include <iostream>
#include <unistd.h>
#include <CLI11/CLI11.hpp>

#include "RPCServer.h"
#include "RPCHandler.h"
#include "PasswordHandler.h"
#include "AccountUtils.h"

#define RPC_SERVER_PATH         "~/.userdata/rpc/accounts"
#define RPC_CLIENT_PASS_PATH    "~/.userdata/rpc/password"


int main(int argc, char* argv[]) {

//    CLI::App app { "guix-tools: Additional tools to work with Guix Package Manager" };
//
//    CLI::App* pkgApp =app.add_subcommand("package", "Guix 'package' related commands");

    setvbuf( stdout, nullptr, _IONBF, 0 );

    string pass;
    string rpcActPath = string("unix:") + PXUTILS::FILE::abspath(RPC_SERVER_PATH);
    string rpcPassPath = string("unix:") + PXUTILS::FILE::abspath(RPC_CLIENT_PASS_PATH);

    CLI::App app { "px-accounts-service: Online Accounts Management Service" };
    app.add_option("-p,--password", pass, "px-accounts-pass related password");
    app.add_option("--pass-path", rpcPassPath, "modify px-pass-service rpc path");

    CLI11_PARSE(app, argc, argv);

    PasswordHandler::Init(rpcPassPath, pass);

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
