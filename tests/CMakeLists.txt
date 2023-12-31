cmake_minimum_required(VERSION 3.0)

set(UNIT_TEST_LIST
        test_common.cpp
        test_account_manager.cpp
        test_account_parser.cpp
        test_rpc_server.cpp
        test_plugin_system.cpp
        test_providers.cpp
        test_event_system.cpp)

set(TARGET_NAME tests)

find_package(PythonLibs REQUIRED)
find_package(Catch2 REQUIRED)

set(CMAKE_CXX_FLAGS -fvisibility=hidden)

add_executable(${TARGET_NAME}
        main.cpp
        SecretSimulator.cpp
        simulators/EventSimulator.cpp
        ${UNIT_TEST_LIST})
find_package(CapnProto CONFIG REQUIRED)
target_link_libraries(${TARGET_NAME}
        PUBLIC ${LIB_NAME}
        PRIVATE CapnProto::capnp-rpc)

target_include_directories(${TARGET_NAME}
        PUBLIC ${PYTHON_INCLUDE_DIRS}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../src/
        PUBLIC ../src)

add_test(
        NAME ${TARGET_NAME}
        COMMAND ${TARGET_NAME} -o report.xml -r junit)


add_executable(plugin_finder
        plugin_finder.cpp)
target_link_libraries(plugin_finder
        PUBLIC ${LIB_NAME})
target_include_directories(plugin_finder
        PUBLIC ${PYTHON_INCLUDE_DIRS}
        PUBLIC ../src/)


add_executable(secret_tester secret_tester.cpp)
target_link_libraries(secret_tester PUBLIC ${LIB_NAME})
target_include_directories(secret_tester
        PUBLIC ../src/
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../src/)
