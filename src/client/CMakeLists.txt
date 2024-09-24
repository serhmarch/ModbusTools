cmake_minimum_required(VERSION 3.5) # 2.2 - case insensitive syntax

project(client VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(MBTOOLS_CLIENT_APP_NAME client)

message("MBTOOLS: Start configure client '${MBTOOLS_CLIENT_APP_NAME}'")

# QT supporting turn ON
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml)

set(HEADERS
    core/client_global.h
    core/client.h      
    ${CMAKE_CURRENT_LIST_DIR}/project/client_builder.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_device.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dom.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_port.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_project.h
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dataview.h
    gui/dialogs/client_dialogdevice.h
    gui/dialogs/client_dialogport.h
    gui/dialogs/client_dialogdataviewitem.h
    gui/dialogs/client_dialogs.h
    gui/dataview/client_dataviewdelegate.h
    gui/dataview/client_dataviewmanager.h
    gui/dataview/client_dataviewmodel.h
    gui/dataview/client_dataviewui.h
    gui/project/client_projectdelegate.h
    gui/project/client_projectmodel.h
    gui/project/client_projectui.h
    gui/sendmessage/client_sendmessageui.h
    gui/scanner/client_dialogscannerrequest.h
    gui/scanner/client_scanner.h
    gui/scanner/client_scannermodel.h
    gui/scanner/client_scannerthread.h
    gui/scanner/client_scannerui.h
    gui/client_windowmanager.h
    gui/client_ui.h
    runtime/client_devicerunnable.h
    runtime/client_portrunnable.h
    runtime/client_rundevice.h
    runtime/client_runitem.h
    runtime/client_runmessage.h
    runtime/client_runthread.h
    runtime/client_runtime.h
)

set(SOURCES
    core/client.cpp      
    ${CMAKE_CURRENT_LIST_DIR}/project/client_builder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_device.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dom.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_port.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_project.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/client_dataview.cpp
    gui/dialogs/client_dialogdevice.cpp
    gui/dialogs/client_dialogport.cpp
    gui/dialogs/client_dialogdataviewitem.cpp
    gui/dialogs/client_dialogs.cpp
    gui/dataview/client_dataviewdelegate.cpp
    gui/dataview/client_dataviewmanager.cpp
    gui/dataview/client_dataviewmodel.cpp
    gui/dataview/client_dataviewui.cpp
    gui/project/client_projectdelegate.cpp
    gui/project/client_projectmodel.cpp
    gui/project/client_projectui.cpp
    gui/sendmessage/client_sendmessageui.cpp
    gui/scanner/client_dialogscannerrequest.cpp
    gui/scanner/client_scanner.cpp
    gui/scanner/client_scannermodel.cpp
    gui/scanner/client_scannerthread.cpp
    gui/scanner/client_scannerui.cpp
    gui/client_windowmanager.cpp
    gui/client_ui.cpp
    runtime/client_devicerunnable.cpp
    runtime/client_portrunnable.cpp
    runtime/client_rundevice.cpp
    runtime/client_runitem.cpp
    runtime/client_runmessage.cpp
    runtime/client_runthread.cpp
    runtime/client_runtime.cpp
    main.cpp
)     

set(RESOURCES 
    gui/client_rsc.qrc              
)     

add_executable(${MBTOOLS_CLIENT_APP_NAME} ${HEADERS} ${SOURCES} ${RESOURCES})

include_directories(.
                    ..
                    ../modbus/src
                    ../core/sdk  
                    ../core/core 
                    ../core      
                    core
)

target_compile_definitions(${MBTOOLS_CLIENT_APP_NAME} PRIVATE QT_NO_KEYWORDS)

set_target_properties(
    ${MBTOOLS_CLIENT_APP_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    WIN32_EXECUTABLE true
)

target_link_libraries(${MBTOOLS_CLIENT_APP_NAME} PRIVATE 
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Xml
                      modbus
                      core
)

