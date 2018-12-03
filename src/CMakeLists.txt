cmake_minimum_required(VERSION 3.0)

get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(INTERFACE_DIR ${PARENT_DIR}/interface)

find_package(yaml-cpp REQUIRED)
find_package(PythonLibs REQUIRED)
find_package(pybind11 REQUIRED)


find_package(CapnProto CONFIG REQUIRED)
set(CAPNPC_SRC_PREFIX "${INTERFACE_DIR}" CACHE STRING "" FORCE)
set(CAPNPC_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/interface" CACHE STRING "" FORCE)
message(${CAPNPC_OUTPUT_DIR})
file(MAKE_DIRECTORY "${CAPNPC_OUTPUT_DIR}")
capnp_generate_cpp(accountSources accountHeaders
                ${INTERFACE_DIR}/Account.capnp
                ${INTERFACE_DIR}/AccountReader.capnp
                ${INTERFACE_DIR}/AccountWriter.capnp)

include_directories(${CapnProto_INCLUDE_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${yaml-cpp_INCLUDE_DIRS}
        ${PYTHON_INCLUDE_DIRS})

link_libraries(CapnProto::capnp-rpc yaml-cpp)
set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_FLAGS -fvisibility=hidden)


add_library(${LIB_NAME}
        ${accountSources} ${accountHeaders}
        RPCServer.cpp
        RPCHandler.cpp
        AccountManager.cpp
        AccountUtils.cpp
        AccountParser.cpp
        PluginContainer.cpp
        PluginManager.cpp)
target_link_libraries(${LIB_NAME} PRIVATE pybind11::embed)
install(TARGETS ${LIB_NAME} DESTINATION lib)
configure_file(testmodule.py ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)

add_executable(${EXE_NAME} main.cpp)
target_link_libraries(${EXE_NAME} ${LIB_NAME})
install(TARGETS ${EXE_NAME} DESTINATION bin)
