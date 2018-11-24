
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
set(INTERFACE_DIR ${PARENT_DIR}/interface)

find_package(yaml-cpp)

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
        ${yaml-cpp_INCLUDE_DIRS})

link_libraries(CapnProto::capnp-rpc yaml-cpp)
set(CMAKE_CXX_FLAGS -pthread)


add_library(${LIB_NAME}
        ${accountSources} ${accountHeaders}
        RPCServer.cpp
        RPCHandler.cpp
        AccountManager.cpp
        AccountUtils.cpp
        AccountParser.cpp)

add_executable(${EXE_NAME} main.cpp)
target_link_libraries(${EXE_NAME} ${LIB_NAME})
