cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(ModbusTools VERSION 0.5.0 LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Debug")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Debug")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/Release")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}/Release")

if (NOT WIN32)
    #set(CMAKE_SKIP_BUILD_RPATH  FALSE)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH $ORIGIN)
    #set(CMAKE_INSTALL_RPATH "\${ORIGIN}")
endif()

set(MB_QT_ENABLED ON)
set(MB_C_SUPPORT_DISABLE ON)
add_subdirectory(modbus/src)

add_subdirectory(src/core)

add_subdirectory(src/client)

add_subdirectory(src/server)

# Check if Doxygen is available
#find_package(Doxygen)
#
#if (DOXYGEN_FOUND)
#    message("Doxygen found: ${DOXYGEN_EXECUTABLE}")
#    # Add a target to generate Doxygen documentation for the client
#    add_custom_target(DoxygenClient
#        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/DoxyfileClient
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
#        COMMENT "Generating Doxygen documentation for Client"
#        VERBATIM
#    )
#
#    # Add a target to generate Doxygen documentation for the server
#    add_custom_target(DoxygenServer
#        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/DoxyfileServer
#        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc
#        COMMENT "Generating Doxygen documentation for Server"
#        VERBATIM
#    )
#
#    # Find qhelpgenerator executable
#    find_program(QHELPGENERATOR_EXECUTABLE qhelpgenerator)
#
#    if (QHELPGENERATOR_EXECUTABLE)
#        message("qhelpgenerator found: ${QHELPGENERATOR_EXECUTABLE}")
#
#        # Add a target to generate Qt help files for the server
#        add_custom_target(QtHelpServer
#            COMMAND ${QHELPGENERATOR_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/output/ModbusServer.qhcp
#            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/output
#            COMMENT "Generating Qt help files for Server"
#            VERBATIM
#        )
#
#        # Add a target to generate Qt help files for the client
#        add_custom_target(QtHelpClient
#            COMMAND ${QHELPGENERATOR_EXECUTABLE} ${CMAKE_SOURCE_DIR}/doc/output/ModbusClient.qhcp
#            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/output
#            COMMENT "Generating Qt help files for Client"
#            VERBATIM
#        )
#
#        # Make sure QtHelpServer runs after DoxygenServer
#        add_dependencies(QtHelpServer DoxygenServer)
#
#        # Make sure QtHelpClient runs after DoxygenClient
#        add_dependencies(QtHelpClient DoxygenClient)
#    else()
#        message(STATUS "qhelpgenerator not found. Qt help targets will not be generated.")
#    endif()
#else()
#    message(STATUS "Doxygen not found. Documentation targets will not be generated/regenerated.")
#endif()

install(TARGETS ${MB_LIBRARY_NAME}         DESTINATION .)
install(TARGETS ${MBTOOLS_CORE_LIB_NAME}   DESTINATION .)
install(TARGETS ${MBTOOLS_CLIENT_APP_NAME} DESTINATION .)
install(TARGETS ${MBTOOLS_SERVER_APP_NAME} DESTINATION .)

# Install scripts
# Collect Python files using GLOB
file(GLOB MBTOOLS_SCRIPT_FILES "${CMAKE_SOURCE_DIR}/src/server/python/*.py")
# Install Python files
install(FILES ${MBTOOLS_SCRIPT_FILES} DESTINATION "./script/server")

# Install help files
# Collect help files using GLOB
file(GLOB MBTOOLS_HELP_FILES "${CMAKE_SOURCE_DIR}/doc/output/*.qch" "${CMAKE_SOURCE_DIR}/doc/output/*.qhc")
install(FILES ${MBTOOLS_HELP_FILES} DESTINATION "./help")
