#include <iostream>
#include <unistd.h>

#include "RPCServer.h"

int main() {

    RPCServer srv("127.0.0.1:4444");
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
