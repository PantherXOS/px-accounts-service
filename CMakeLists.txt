cmake_minimum_required(VERSION 3.12)
project(px_accounts_service)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS -pthread)

find_package(yaml-cpp)
find_package(CapnProto CONFIG REQUIRED)
capnp_generate_cpp( accountSources accountHeaders
        interface/Account.capnp
        interface/AccountReader.capnp
        interface/AccountWriter.capnp)

include_directories(${CapnProto_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${yaml-cpp_INCLUDE_DIRS})

link_libraries(CapnProto::capnp-rpc yaml-cpp)

add_executable(px_accounts_service ${accountSources} ${accountHeaders}
        src/main.cpp
        src/RPCServer.cpp
        src/RPCHandler.cpp
        src/AccountManager.cpp
        src/AccountUtils.cpp
        src/AccountParser.cpp)
