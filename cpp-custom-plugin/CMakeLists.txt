cmake_minimum_required(VERSION 3.0)
project(px-accounts-service-plugin-cpp-custom)

set(CMAKE_CXX_STANDARD 14)

include_directories(../src/Plugins)

add_library(${PROJECT_NAME} SHARED
        ../src/Plugins/PluginInterface.h
        PluginInterface.cpp
        cpp-custom-plugin.h
        cpp-custom-plugin.cpp)
IF(UNIX)
    IF(NOT APPLE)
        target_link_libraries(${PROJECT_NAME} PRIVATE uuid)
    ENDIF(NOT APPLE)
ENDIF(UNIX)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
