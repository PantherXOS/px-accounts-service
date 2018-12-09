cmake_minimum_required(VERSION 3.0)

set(UNIT_TEST_LIST
        test_account_manager.cpp
        test_rpc_server.cpp
        test_plugin_system.cpp)

set(TARGET_NAME tests)

configure_file(px_accounts_service_plugin_test.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

find_package(PythonLibs REQUIRED)

set(CMAKE_CXX_FLAGS -fvisibility=hidden)

add_executable(${TARGET_NAME}
        main.cpp
        ${UNIT_TEST_LIST})

target_link_libraries(${TARGET_NAME}
        PUBLIC ${LIB_NAME})

target_include_directories(${TARGET_NAME}
        PUBLIC ${PYTHON_INCLUDE_DIRS}
        PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/../src/
        PUBLIC ../Catch2/single_include
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

