cmake_minimum_required(VERSION 3.0)
project(px-accounts-service-plugin-cpp-test)

set(CMAKE_CXX_STANDARD 14)

include_directories(../src/Plugins)

add_library(${PROJECT_NAME} SHARED
        ../src/plugins/PluginInterface.h
        PluginInterface.cpp
        cpp-test-plugin.h
        cpp-test-plugin.cpp)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)