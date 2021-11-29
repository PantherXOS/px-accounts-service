//
// Created by Reza Alizadeh Majd on 11/24/18.
//


#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <sys/stat.h>

#include <RPCServer.h>
#include <RPCHandler.h>
#include <Accounts/AccountUtils.h>
#include <Accounts/AccountManager.h>
#include <Secret/SecretManager.h>
#include <EventManager.h>

#include "test_common.h"
#include "SecretSimulator.h"
#include "simulators/EventSimulator.h"

#define CPP_PLUGIN_PATH "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-test.so"
#define CPP_CUSTOM_PLUGIN_PATH "../cpp-custom-plugin/libpx-accounts-service-plugin-cpp-custom.so"
#define CPP_AUTO_INIT_PLUGIN_PATH "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-autoinit.so"


Logger gLogger("account-tests");


void register_test_plugin(const string &name, const string &version, const string &type, const string &path) {
    mkdir("./plugins", S_IRWXU | S_IRWXG | S_IRWXO);
    string pluginFileName = "./plugins/" + name + ".yaml";
    if (PXUTILS::FILE::exists(pluginFileName)) {
        PXUTILS::FILE::remove(pluginFileName);
    }
    ofstream pluginStream;
    pluginStream.open(pluginFileName);
    pluginStream << "plugin:\n"
                 << "   name: " << name << "\n"
                 << "   version: " << version << "\n"
                 << "   type: " << type << "\n"
                 << "   path: " << path << "\n";
}

void register_cpp_plugin(const string &name,
                         const string &basePath = "../cpp-test-plugins",
                         const string &version = "0.0.1") {
    stringstream libStream;
    libStream << basePath << "/lib" << name << ".so";
    register_test_plugin(name, version, "cpp", libStream.str());
}

void register_python_plugin(const string &name, const string &basePath = ".",
                            const string &version = "0.0.1") {
    register_test_plugin(name, version, "python", basePath);
}

int main(int argc, char *argv[]) {

    setvbuf(stdout, nullptr, _IONBF, 0);
    // GLOG_INIT(LogTarget::SYSLOG, LogLevel::INF);
    GLOG_INIT(LogTarget::CONSOLE, LogLevel::INF);

    register_cpp_plugin("px-accounts-service-plugin-cpp-test");
    register_cpp_plugin("px-accounts-service-plugin-cpp-autoinit");
    register_cpp_plugin("px-accounts-service-plugin-cpp-custom",
                        "../cpp-custom-plugin");

    register_python_plugin("px-accounts-service-plugin-protected-params");
    register_python_plugin("px-accounts-service-plugin-python-test");
    register_python_plugin("px-accounts-service-plugin-test-public-service");
    register_python_plugin("px-accounts-service-plugin-python-json");
    register_python_plugin("px_accounts-service-plugin-python-autoinit");

    RPCServer<SecretSimulator> secretSimulator(SECRET_SIMULATOR_PATH);
    secretSimulator.start();

    EventSimulator evtSimulator;
    evtSimulator.start();
    evtSimulator.registerChannel("account");

    SecretManager::Init(SECRET_SIMULATOR_PATH);

    vector<string> userAccountsPath;
    userAccountsPath.push_back("/tmp/px-accounts-tests");
    AccountManager::Init(userAccountsPath);
    EventManager::Init(TEST_RPC_EVENT_PATH);

    vector<string> pluginPaths; 
    pluginPaths.push_back(PXUTILS::FILE::abspath("./plugins"));
    PluginManager::Init(pluginPaths);

    AccountManager::Instance().createAutoInitializingAccounts();

    RPCServer<RPCHandler> srv(MAIN_SERVER_PATH);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();
    secretSimulator.stop();
    evtSimulator.stop();
    return result;
}
