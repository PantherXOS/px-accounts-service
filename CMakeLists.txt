cmake_minimum_required(VERSION 3.12)
project(px_accounts_service)

set(CMAKE_CXX_STANDARD 14)

find_package(yaml-cpp)
find_package(CapnProto CONFIG REQUIRED)
capnp_generate_cpp( accountSources accountHeaders
        interface/Account.capnp
        interface/AccountReader.capnp
        interface/AccountWriter.capnp)

add_executable(px_accounts_service ${accountSources} ${accountHeaders}
        src/main.cpp
        src/RPCHandler.cpp
        src/AccountManager.cpp
        src/AccountUtils.cpp
        src/AccountParser.cpp)

target_link_libraries(px_accounts_service PRIVATE CapnProto::capnp-rpc yaml-cpp)

target_include_directories(px_accounts_service PRIVATE
        ${CapnProto_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${yaml-cpp_INCLUDE_DIRS})


if (EXISTS "tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()
