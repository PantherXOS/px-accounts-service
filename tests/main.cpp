//
// Created by Reza Alizadeh Majd on 11/24/18.
//


#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <sys/stat.h>

#include <RPCServer.h>
#include <RPCHandler.h>
#include <AccountUtils.h>
#include <Secret/SecretManager.h>

#include "test_common.h"

#ifdef __linux__
#else
#include "PasswordSimulator.h"
#endif

#ifdef __linux__
#define CPP_PLUGIN_PATH "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-test.so"
#else
#define CPP_PLUGIN_PATH "../cpp-test-plugin/libpx-accounts-service-plugin-cpp-test.dylib"
#endif


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

    setvbuf( stdout, nullptr, _IONBF, 0 );

    gLogger.setLevel(Logger::LVL_INF);

    register_test_plugin("px-accounts-service-plugin-cpp-test",
                         "0.0.1",
                         "cpp",
                         CPP_PLUGIN_PATH);
    register_test_plugin("px-accounts-service-plugin-python-test",
                         "0.0.1",
                         "python",
                         ".");



#ifdef __linux__
//    system("for pid in $(ps x | grep -v grep | grep px_pass_service | awk '{print $1}'); do echo \"killing $pid\"; kill $pid; done;");
//    system("px_pass_service 123 &");
//    sleep(2);
#else
    RPCServer<PasswordSimulator> passSvc(PASSWORD_SIMULATOR_PATH);
    passSvc.start();
#endif


//    PasswordHandler::Init(PASSWORD_SIMULATOR_PATH, "123");
//    SecretManager::Init(PXUTILS::FILE::abspath("~/.userdata/rpc/secret"));
    SecretManager::Init("unix:/root/.userdata/rpc/secret");

    RPCServer<RPCHandler> srv(SERVER_ADDRESS);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();

#ifdef __linux__
//    system("for pid in $(ps x | grep -v grep | grep px_pass_service | awk '{print $1}'); do echo \"killing $pid\"; kill $pid; done;");
#else
    passSvc.stop();
#endif

    return result;
}

