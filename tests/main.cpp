//
// Created by Reza Alizadeh Majd on 11/24/18.
//


#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <RPCServer.h>
#include <RPCHandler.h>
#include <AccountUtils.h>
#include <PasswordHandler.h>

#include "test_common.h"

#ifdef __linux__
#else
#include "PasswordSimulator.h"
#endif



int main(int argc, char *argv[]) {

    setvbuf( stdout, nullptr, _IONBF, 0 );


#ifdef __linux__
    system("for pid in $(ps x | grep -v grep | grep px_pass_service | awk '{print $1}'); do echo \"killing $pid\"; kill $pid; done;");
    system("px_pass_service 123 &");
    sleep(2);
#else
    RPCServer<PasswordSimulator> passSvc(PASSWORD_SIMULATOR_PATH);
    passSvc.start();
#endif


    PasswordHandler::Init(PASSWORD_SIMULATOR_PATH, "123");

    RPCServer<RPCHandler> srv(SERVER_ADDRESS);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();

#ifdef __linux__
    system("for pid in $(ps x | grep -v grep | grep px_pass_service | awk '{print $1}'); do echo \"killing $pid\"; kill $pid; done;");
#else
    passSvc.stop();
#endif

    return result;
}

