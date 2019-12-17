cmake_minimum_required(VERSION 3.0)
project(px-accounts-service)

set(CMAKE_CXX_STANDARD 14)

set(LIB_NAME _${PROJECT_NAME})
set(EXE_NAME ${PROJECT_NAME})

include_directories(src)

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(cpp-test-plugin)
add_subdirectory(cpp-custom-plugin)

add_dependencies(tests
        px-accounts-service-plugin-cpp-test
        px-accounts-service-plugin-cpp-custom)

file(COPY providers DESTINATION ${CMAKE_BINARY_DIR}/src/)
file(COPY providers DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file(COPY python-test-plugin/px_accounts_service_plugin_protected_params.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file(COPY python-test-plugin/px_accounts_service_plugin_protected_params.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file(COPY python-test-plugin/px_accounts_service_plugin_python_test.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file(COPY python-test-plugin/px_accounts_service_plugin_python_test.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file(COPY python-test-plugin/px_accounts_service_plugin_test_public_service.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file(COPY python-test-plugin/px_accounts_service_plugin_test_public_service.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)
file(COPY python-test-plugin/px_accounts_service_plugin_python_json.py DESTINATION ${CMAKE_BINARY_DIR}/src/)
file(COPY python-test-plugin/px_accounts_service_plugin_python_json.py DESTINATION ${CMAKE_BINARY_DIR}/tests/)

install(DIRECTORY providers DESTINATION etc/px/accounts/)

find_package(Doxygen)
if (DOXYGEN_FOUND)
    # set input and output files
    set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile)
    set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

    # request to configure the file
    configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
    message("Doxygen build started")

    # note the option ALL which allows to build the docs together with the application
    add_custom_target(doc_doxygen ALL
            COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen need to be installed to generate the doxygen documentation")
endif (DOXYGEN_FOUND)