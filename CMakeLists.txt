cmake_minimum_required(VERSION 3.0)
project(px-accounts-service)

set(CMAKE_CXX_STANDARD 14)

set(LIB_NAME _${PROJECT_NAME})
set(EXE_NAME ${PROJECT_NAME})

OPTION(LOCAL "Build on Local system" OFF)
IF (LOCAL)
    ADD_DEFINITIONS(-DPROVIDER_STORE_PATH="./providers/")
ENDIF(LOCAL)


add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(cpp-test-plugin)

file (COPY providers DESTINATION ${CMAKE_BINARY_DIR}/src/)
file (COPY providers DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file (COPY python-test-plugin/px_accounts_service_plugin_python_test.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file (COPY python-test-plugin/px_accounts_service_plugin_python_test.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)

install(DIRECTORY providers DESTINATION etc/accounts/)
