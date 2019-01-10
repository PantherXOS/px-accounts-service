cmake_minimum_required(VERSION 3.0)
project(px-accounts-service)

set(CMAKE_CXX_STANDARD 14)

set(LIB_NAME _${PROJECT_NAME})
set(EXE_NAME ${PROJECT_NAME})

add_subdirectory(src)
add_subdirectory(tests)
