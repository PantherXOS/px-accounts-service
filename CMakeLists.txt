cmake_minimum_required(VERSION 3.0)
project(px-accounts-service)

set(CMAKE_CXX_STANDARD 14)

set(LIB_NAME _${PROJECT_NAME})
set(EXE_NAME ${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(tests)

file (COPY providers DESTINATION ${CMAKE_BINARY_DIR}/src/)
file (COPY providers DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file (COPY px_accounts_service_plugin_test.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file (COPY px_accounts_service_plugin_test.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)