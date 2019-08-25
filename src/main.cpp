#include <iostream>
#include <unistd.h>
#include <CLI11/CLI11.hpp>

#include "RPCServer.h"
#include "RPCHandler.h"
#include "Secret/SecretManager.h"
#include "Plugins/PluginManager.h"
#include "AccountUtils.h"

#define RPC_DIR "~/.userdata/rpc"
#define RPC_SERVER_PATH         RPC_DIR "/accounts"
#define RPC_CLIENT_SECRET_PATH  RPC_DIR "/secret"

#define RPC_MKPATH_CMD "mkdir -p " RPC_DIR


void IntHandler(int dummy) {
    puts("Server Terminated.");
    exit(0);
}

int main(int argc, char *argv[]) {


    setvbuf(stdout, nullptr, _IONBF, 0);
    signal(SIGINT, IntHandler);
    system(RPC_MKPATH_CMD);

    bool isDebug = false;
    string pass = "123";
    string rpcActPath = string("unix:") + PXUTILS::FILE::abspath(RPC_SERVER_PATH);
    string rpcSecretPath = string("unix:") + PXUTILS::FILE::abspath((RPC_CLIENT_SECRET_PATH));

    CLI::App app{"px-accounts-service: Online Accounts Management Service"};
    app.add_flag("-d,--debug", isDebug, "Run px-accounts-service in debug mode");
    app.add_option("-p,--password", pass, "px-accounts-pass related password");
    app.add_option("--secret-path", rpcSecretPath, "modify px-secret-service rpc path");

    CLI11_PARSE(app, argc, argv);

    if (isDebug) {
        gLogger.setLevel(Logger::LVL_INF);
        LOG_INF("=== DEBUG MODE ===");
    }

    SecretManager::Init(rpcSecretPath);
    PluginManager::Instance();

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
