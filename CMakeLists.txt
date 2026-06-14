cmake_minimum_required(VERSION 3.13) # 2.2 - case insensitive syntax
                                     # 3.13 included policy CMP0077

project(ModbusTools VERSION 0.5.1 LANGUAGES CXX)

set(MBTOOLS_ROOT_DIR ${CMAKE_SOURCE_DIR} CACHE INTERNAL "Root directory of the project")

set(MBTOOLS_NAME                "mbtools" CACHE INTERNAL "Name of the application"   )
set(MBTOOLS_VERSION             ${PROJECT_VERSION} CACHE INTERNAL "Version of the application")
set(MBTOOLS_VENDOR              "serhmarch" CACHE INTERNAL "Vendor of the application" )
set(MBTOOLS_CONTACT             "Serhii Marchuk <mrchsrg@gmail.com>" CACHE INTERNAL "Contact of the application")
set(MBTOOLS_DESCRIPTION_SUMMARY "Modbus Client and Server tools" CACHE INTERNAL "Description summary of the application")
set(MBTOOLS_DESCRIPTION         "GUI tools for Modbus TCP/UDP/RTU/ASCII: client and server." CACHE INTERNAL "Description of the application")
set(MBTOOLS_HOMEPAGE_URL        "https://github.com/serhmarch/ModbusTools" CACHE INTERNAL "Homepage URL of the application")
set(MBTOOLS_LICENSE             "LICENSE" CACHE INTERNAL "License of the application")
set(MBTOOLS_README              "README.md" CACHE INTERNAL "README file of the application")

include(cmake/InstallDirs.cmake)

option(MBTOOLS_CLIENT_ENABLED "Enable client application build" ON)
option(MBTOOLS_SERVER_ENABLED "Enable server application build" ON)
option(MBTOOLS_PACKAGE "Enable CPack packaging configuration" OFF)

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
    set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib/mbtools")
endif()

set(MB_QT_ENABLED ON)
set(MB_C_SUPPORT_DISABLE ON)
set(MB_NO_INSTALL ON)
# EXCLUDE_FROM_ALL: to not generate install data
add_subdirectory(modbus/src EXCLUDE_FROM_ALL)

add_subdirectory(src/core)

if (MBTOOLS_CLIENT_ENABLED)
    add_subdirectory(src/client)
endif()

if (MBTOOLS_SERVER_ENABLED)
    add_subdirectory(src/server)
endif()

add_subdirectory("doc")

install(TARGETS ${MB_LIBRARY_NAME}         DESTINATION ${MBTOOLS_INSTALL_LIB_DIR})
install(TARGETS ${MBTOOLS_CORE_LIB_NAME}   DESTINATION ${MBTOOLS_INSTALL_LIB_DIR})

if (MBTOOLS_CLIENT_ENABLED)
    install(TARGETS ${MBTOOLS_CLIENT_APP_NAME} DESTINATION ${MBTOOLS_INSTALL_BIN_DIR})
endif()

if (MBTOOLS_SERVER_ENABLED)
    install(TARGETS ${MBTOOLS_SERVER_APP_NAME} DESTINATION ${MBTOOLS_INSTALL_BIN_DIR})
    # Install scripts
    # Collect Python files using GLOB
    file(GLOB MBTOOLS_SCRIPT_FILES "${CMAKE_SOURCE_DIR}/src/server/python/*.py")
    # Install Python files
    install(FILES ${MBTOOLS_SCRIPT_FILES} DESTINATION ${MBTOOLS_INSTALL_SCRIPT_DIR})
endif()

if (WIN32)
    # Need to install next Qt libs for Windows:
    # - Qt5Core.dll
    # - Qt5Gui.dll
    # - Qt5Widgets.dll
    # - Qt5Sql.dll
    # - Qt5Help.dll
    # - libEGL.dll
    # - libGLESv2.dll
    # Need to install next platform plugins for Windows:
    # - platforms/qwindows.dll
    # - sqldrivers/qsqlite.dll
    # - styles/qwindowsvistastyle.dll
    
    # find Qt libraries and install them to the same directory as the application executable
    find_package(Qt5 COMPONENTS Core Gui Widgets Sql Help REQUIRED)

    # Get the Qt core library location to determine the Qt installation directory
    get_target_property(_QtCore_LIBRARY Qt5::Core LOCATION)
    get_filename_component(_QtCore_LIB_DIR "${_QtCore_LIBRARY}" DIRECTORY)

    set(MBTOOLS_QT_LIBS
        Qt5::Core
        Qt5::Gui
        Qt5::Widgets
        Qt5::Sql
        Qt5::Help
    )
    foreach(_lib IN LISTS MBTOOLS_QT_LIBS)
        get_target_property(_lib_file ${_lib} LOCATION)
        list(APPEND QT_DLLS ${_lib_file})
    endforeach()
    # Add libEGL.dll and libGLESv2.dll which are located in the same directory as the Qt DLLs
    list(APPEND QT_DLLS
        "${_QtCore_LIB_DIR}/libEGL.dll"
        "${_QtCore_LIB_DIR}/libGLESv2.dll"
    )

    message(STATUS "MBTOOLS: Qt libraries to be installed: ${QT_DLLS}")
    # Install Qt DLLs alongside the executable
    install(FILES ${QT_DLLS} DESTINATION ${MBTOOLS_INSTALL_BIN_DIR})

    # Determine the Qt plugins directory
    get_target_property(_Qt_PLUGINS_DIR Qt5::Core QT_PLUGINS_DIR)
    if(NOT _Qt_PLUGINS_DIR)
        # Fallback: assume plugins are in ../plugins relative to the QtCore DLL directory
        get_filename_component(_Qt_BASE_DIR "${_QtCore_LIB_DIR}" DIRECTORY)
        set(_Qt_PLUGINS_DIR "${_Qt_BASE_DIR}/plugins")
    endif()

    # Install required platform plugins
    install(FILES "${_Qt_PLUGINS_DIR}/platforms/qwindows.dll"        DESTINATION ${MBTOOLS_INSTALL_BIN_DIR}/platforms)
    install(FILES "${_Qt_PLUGINS_DIR}/sqldrivers/qsqlite.dll"        DESTINATION ${MBTOOLS_INSTALL_BIN_DIR}/sqldrivers)
    install(FILES "${_Qt_PLUGINS_DIR}/styles/qwindowsvistastyle.dll" DESTINATION ${MBTOOLS_INSTALL_BIN_DIR}/styles)
endif()

# Install help files
# Collect help files using GLOB
file(GLOB MBTOOLS_HELP_FILES "${CMAKE_BINARY_DIR}/doc/output/*.qch" "${CMAKE_BINARY_DIR}/doc/output/*.qhc")
install(FILES ${MBTOOLS_HELP_FILES} DESTINATION ${MBTOOLS_INSTALL_HELP_DIR})

message(STATUS "MBTOOLS: Packaging: ${MBTOOLS_PACKAGE}")
if (MBTOOLS_PACKAGE)
    add_subdirectory(package)
endif()
