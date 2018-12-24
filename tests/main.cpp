//
// Created by Reza Alizadeh Majd on 11/24/18.
//


#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>
#include <RPCServer.h>

#include "test_common.h"


int main(int argc, char *argv[]) {

    RPCServer srv(SERVER_ADDRESS);
    srv.start();

    int result = Catch::Session().run(argc, argv);

    srv.stop();

    return result;
}

