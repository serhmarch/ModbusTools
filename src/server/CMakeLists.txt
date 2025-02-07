cmake_minimum_required(VERSION 3.5) # 2.2 - case insensitive syntax

project(server VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(MBTOOLS_SERVER_APP_NAME server)

message("MBTOOLS: Start configure server '${MBTOOLS_SERVER_APP_NAME}'")

# QT supporting turn ON
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml)

set(HEADERS
    core/server_global.h
    core/server.h      
    ${CMAKE_CURRENT_LIST_DIR}/project/server_action.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_builder.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_device.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_deviceref.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_dom.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_port.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_project.h
    ${CMAKE_CURRENT_LIST_DIR}/project/server_dataview.h
    gui/dialogs/server_dialogaction.h
    gui/dialogs/server_dialogdevice.h
    gui/dialogs/server_dialogport.h
    gui/dialogs/server_dialogs.h
    gui/dialogs/server_dialogdataviewitem.h
    gui/dataview/server_dataviewdelegate.h
    gui/dataview/server_dataviewmanager.h
    gui/dataview/server_dataviewmodel.h
    gui/dataview/server_dataviewui.h
    gui/project/server_projectdelegate.h
    gui/project/server_projectmodel.h
    gui/project/server_projectui.h
    gui/device/server_devicemanager.h
    gui/device/server_deviceui.h
    gui/device/server_deviceuidelegate.h
    gui/device/server_deviceuimodel.h
    gui/actions/server_actionsdelegate.h
    gui/actions/server_actionsmodel.h
    gui/actions/server_actionsui.h
    gui/server_windowmanager.h
    gui/server_ui.h
    runtime/server_portrunnable.h
    runtime/server_runaction.h
    runtime/server_runactiontask.h
    runtime/server_rundevice.h
    runtime/server_runthread.h
    runtime/server_runtime.h
)

set(SOURCES
    core/server.cpp      
    ${CMAKE_CURRENT_LIST_DIR}/project/server_action.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_builder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_device.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_deviceref.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_dom.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_port.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_project.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/server_dataview.cpp
    gui/dialogs/server_dialogaction.cpp
    gui/dialogs/server_dialogdevice.cpp
    gui/dialogs/server_dialogport.cpp
    gui/dialogs/server_dialogs.cpp
    gui/dialogs/server_dialogdataviewitem.cpp
    gui/dataview/server_dataviewdelegate.cpp
    gui/dataview/server_dataviewmanager.cpp
    gui/dataview/server_dataviewmodel.cpp
    gui/dataview/server_dataviewui.cpp
    gui/project/server_projectdelegate.cpp
    gui/project/server_projectmodel.cpp
    gui/project/server_projectui.cpp
    gui/device/server_devicemanager.cpp
    gui/device/server_deviceui.cpp
    gui/device/server_deviceuidelegate.cpp
    gui/device/server_deviceuimodel.cpp
    gui/actions/server_actionsdelegate.cpp
    gui/actions/server_actionsmodel.cpp
    gui/actions/server_actionsui.cpp
    gui/server_windowmanager.cpp
    gui/server_ui.cpp
    runtime/server_portrunnable.cpp
    runtime/server_runaction.cpp
    runtime/server_runactiontask.cpp
    runtime/server_rundevice.cpp
    runtime/server_runthread.cpp
    runtime/server_runtime.cpp
    main.cpp
)     

set(RESOURCES 
    gui/server_rsc.qrc              
)     

if (WIN32)
    set(MBTOOLS_WIN_RESOURCE_FILE win_resource.rc)
    message("${PROJECT_NAME} resource file for Windows: '${MBTOOLS_WIN_RESOURCE_FILE}'")

    set(MBTOOLS_WIN_ICON "IDI_ICON1\tICON\tDISCARDABLE\t\"gui\\\\icons\\\\${PROJECT_NAME}.ico\"")
    set(MBTOOLS_WIN_FILE "${PROJECT_NAME}")
    set(MBTOOLS_WIN_OUTFILE "${MBTOOLS_WIN_FILE}.exe")
    configure_file(${CMAKE_CURRENT_LIST_DIR}/../${MBTOOLS_WIN_RESOURCE_FILE}.in 
                ${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_WIN_RESOURCE_FILE})

    set(RESOURCES ${RESOURCES}
        ${CMAKE_CURRENT_LIST_DIR}/${MBTOOLS_WIN_RESOURCE_FILE}           
    )     
endif()

add_executable(${MBTOOLS_SERVER_APP_NAME} ${HEADERS} ${SOURCES} ${RESOURCES})

include_directories(.
                    ..
                    ../../modbus/src
                    ../core/sdk  
                    ../core/core 
                    ../core      
                    core
)

target_compile_definitions(${MBTOOLS_SERVER_APP_NAME} PRIVATE QT_NO_KEYWORDS)

set_target_properties(
    ${MBTOOLS_SERVER_APP_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    WIN32_EXECUTABLE true
)

target_link_libraries(${MBTOOLS_SERVER_APP_NAME} PRIVATE 
                      Qt${QT_VERSION_MAJOR}::Core
                      Qt${QT_VERSION_MAJOR}::Gui
                      Qt${QT_VERSION_MAJOR}::Widgets
                      Qt${QT_VERSION_MAJOR}::Xml
                      modbus
                      core
)

