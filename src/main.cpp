#include <iostream>
#include <unistd.h>
#include <cassert>
#include <CLI11/CLI11.hpp>

#include "RPCServer.h"
#include "RPCHandler.h"
#include "EventManager.h"
#include "Secret/SecretManager.h"
#include "Plugins/PluginManager.h"
#include "Accounts/AccountUtils.h"
#include "Accounts/AccountManager.h"


Logger gLogger("accounts");

void IntHandler(int dummy) {
    puts("Server Terminated.");
    exit(0);
}

int main(int argc, char *argv[]) {

    setvbuf(stdout, nullptr, _IONBF, 0);
    signal(SIGINT, IntHandler);

    string username = PXUTILS::SYSTEM::current_user();
    assert(username != "");
    string rpcBasePath = "/tmp/" + username + "/rpc";
    assert(PXUTILS::FILE::mkpath(rpcBasePath));

    bool isDebug = false;
    string rpcActPath = string("unix:") + rpcBasePath + "/accounts";
    string rpcEventPath = string("ipc://") + rpcBasePath + "/events";
    // default path for secret service: https://git.pantherx.org/development/applications/px-secret-service/-/issues/34#note_30058
    string rpcSecretPath = string("unix:") + rpcBasePath + "/secret";

    LogTarget logTarget = LogTarget::SYSLOG;
    std::vector<std::string> userAccountPaths;
    std::vector<std::string> readonlyAccountsPath;

    std::vector<std::pair<std::string, LogTarget> > logTargetMapping = {
            {"syslog",  LogTarget::SYSLOG},
            {"console", LogTarget::CONSOLE}
    };

    CLI::App app{"px-accounts-service: Online Accounts Management Service"};
    app.add_flag("-d,--debug", isDebug, "Run px-accounts-service in debug mode");
    app.add_option("--secret-path", rpcSecretPath, "set RPC path for px-secret-service");
    app.add_option("--events-path", rpcEventPath, "set RPC path for px-events-service");
    app.add_option("-t,--log-target", logTarget, "Target for application logs to publish")
            ->transform(CLI::CheckedTransformer(logTargetMapping, CLI::ignore_case));
    app.add_option("--user-account-dir", userAccountPaths, "path for account files to read/write");
    app.add_option("--readonly-account-dir", readonlyAccountsPath, "read-only path for account files to read");

    CLI11_PARSE(app, argc, argv);

    LogLevel logLevel = (isDebug ? LogLevel::INF : LogLevel::WRN);
    GLOG_INIT(logTarget, logLevel);

    // Init AccountManager:
    if (userAccountPaths.empty()) {
        string dataDir = getenv("XDG_DATA_HOME");
        if (dataDir.empty()) {
            dataDir = PXUTILS::FILE::abspath("~/.local/share");
        }
        string userDefaultPath = dataDir + "/px-accounts-service/accounts";
        userAccountPaths.push_back(userDefaultPath);
    }
    if (!AccountManager::Init(userAccountPaths, readonlyAccountsPath)) {
        GLOG_ERR("AccountManager Initiation Failed.");
        return EXIT_FAILURE;
    }


    EventManager::Init(rpcEventPath);
    SecretManager::Init(rpcSecretPath);
    PluginManager::Instance();

    RPCServer<RPCHandler> srv(rpcActPath);
    srv.start();

    GLOG_INF_FORCE("User Accounts Path:");
    for (const auto &path: userAccountPaths) {
        GLOG_INF_FORCE("\t - ", path);
    }
    GLOG_INF_FORCE("Readonly Accounts Path:");
    for (const auto &path: readonlyAccountsPath) {
        GLOG_INF_FORCE("\t - ", path);
    }
    GLOG_INF_FORCE("========================================");
    GLOG_INF_FORCE("px-accounts-service started.");
    GLOG_INF("debug mode enabled");
    GLOG_INF("log target: [", (logTarget == LogTarget::SYSLOG ? "syslog" : "console"), "]");


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
#pragma clang diagnostic pop

    return 0;
}
