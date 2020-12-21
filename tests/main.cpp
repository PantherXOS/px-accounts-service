//
// Created by Reza Alizadeh Majd on 11/24/18.
//


#define CATCH_CONFIG_RUNNER

#include <catch2/catch.hpp>
#include <sys/stat.h>

#include <RPCServer.h>
#include <RPCHandler.h>
#include <Accounts/AccountUtils.h>
#include <Secret/SecretManager.h>

#include "test_common.h"
#include "SecretSimulator.h"
#include "simulators/EventSimulator.h"

#define CPP_PLUGIN_PATH "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-test.so"
#define CPP_CUSTOM_PLUGIN_PATH "../cpp-custom-plugin/libpx-accounts-service-plugin-cpp-custom.so"

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


int main(int argc, char *argv[]) {

    setvbuf(stdout, nullptr, _IONBF, 0);
    // GLOG_INIT(LogTarget::SYSLOG, LogLevel::INF);
    GLOG_INIT(LogTarget::CONSOLE, LogLevel::INF);


    register_test_plugin("px-accounts-service-plugin-cpp-test",
                         "0.0.1",
                         "cpp",
                         CPP_PLUGIN_PATH);
    register_test_plugin("px-accounts-service-plugin-cpp-custom",
                         "0.0.1",
                         "cpp",
                         CPP_CUSTOM_PLUGIN_PATH);
    register_test_plugin("px-accounts-service-plugin-protected-params",
                         "0.0.1",
                         "python",
                         ".");
    register_test_plugin("px-accounts-service-plugin-python-test",
                         "0.0.1",
                         "python",
                         ".");
    register_test_plugin("px-accounts-service-plugin-test-public-service",
                         "0.0.1",
                         "python",
                         ".");
    register_test_plugin("px-accounts-service-plugin-python-json",
                         "0.0.1",
                         "python",
                         ".");

    RPCServer<SecretSimulator> secretSimulator(SECRET_SIMULATOR_PATH);
    secretSimulator.start();

    EventSimulator evtSimulator;
    evtSimulator.start();
    evtSimulator.registerChannel("account");


    SecretManager::Init(SECRET_SIMULATOR_PATH);

    RPCServer<RPCHandler> srv(MAIN_SERVER_PATH);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();
    secretSimulator.stop();
    evtSimulator.stop();
    return result;
}
