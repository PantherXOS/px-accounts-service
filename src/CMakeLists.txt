cmake_minimum_required(VERSION 3.0)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(INTERFACE_DIR ${PARENT_DIR}/interface)

find_package(yaml-cpp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)
find_package(nng REQUIRED)


find_package(CapnProto CONFIG REQUIRED)
set(CAPNPC_SRC_PREFIX "${INTERFACE_DIR}" CACHE STRING "" FORCE)
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/interface" CACHE STRING "" FORCE)
message(${CAPNPC_OUTPUT_DIR})
file(MAKE_DIRECTORY "${CAPNPC_OUTPUT_DIR}")
capnp_generate_cpp(rpcSources rpcHeaders
        ${INTERFACE_DIR}/Account.capnp
        ${INTERFACE_DIR}/AccountReader.capnp
        ${INTERFACE_DIR}/AccountWriter.capnp
        ${INTERFACE_DIR}/event.capnp
        ${INTERFACE_DIR}/Secret.capnp
        )

include_directories(${CapnProto_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${yaml-cpp_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS})

link_libraries(CapnProto::capnp-rpc yaml-cpp dl uuid)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_FLAGS -fvisibility=hidden)


add_library(${LIB_NAME}
        ${rpcSources} ${rpcHeaders}
        RPCServer.cpp
        RPCClient.cpp
        RPCHandler.cpp
        Accounts/AccountManager.cpp
        Accounts/AccountUtils.cpp
        Accounts/AccountParser.cpp
        Accounts/AccountService.cpp
        Accounts/AccountObject.cpp
        Accounts/AccountDefinitions.cpp
        Plugins/DLLoader.h
        Plugins/PluginContainerBase.cpp
        Plugins/PluginContainerPython.cpp
        Plugins/PluginContainerCpp.cpp
        Plugins/PluginManager.cpp
        Utils/Logger.cpp
        ProviderHandler.cpp
        Secret/SecretManager.cpp
        EventManager.cpp)
target_link_libraries(${LIB_NAME} PRIVATE pybind11::embed
        PRIVATE nng::nng gcov)
install(TARGETS ${LIB_NAME} DESTINATION lib)

add_executable(${EXE_NAME} main.cpp)
target_include_directories(${PROJECT_NAME} PUBLIC ../vendor/)
target_link_libraries(${EXE_NAME} ${LIB_NAME})
install(TARGETS ${EXE_NAME} DESTINATION bin)
