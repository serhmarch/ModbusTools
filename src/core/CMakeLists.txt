cmake_minimum_required(VERSION 3.5) # 2.2 - case insensitive syntax

project(core VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(MBTOOLS_CORE_LIB_NAME core)

message("MBTOOLS: Start configure library '${MBTOOLS_CORE_LIB_NAME}'")

configure_file(${CMAKE_CURRENT_LIST_DIR}/mbcore_config.h.in ${CMAKE_CURRENT_LIST_DIR}/sdk/mbcore_config.h)

# QT supporting turn ON
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Help)
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Gui Widgets Xml Help)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets Xml Help)

set(HEADERS         
    sdk/mbcore_config.h
    sdk/mbcore.h
    sdk/mbcore_base.h
    sdk/mbcore_sharedpointer.h
    sdk/mbcore_task.h
    sdk/mbcore_taskfactory.h
    core/core.h
    core/core_global.h
    task/core_taskfactoryinfo.h
    plugin/core_pluginmanager.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_project.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_port.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_device.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_dataview.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_taskinfo.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_dom.h
    ${CMAKE_CURRENT_LIST_DIR}/project/core_builder.h
    gui/dialogs/core_dialogbase.h
    gui/dialogs/core_dialogsystemsettings.h
    gui/dialogs/core_dialogsettings.h
    gui/dialogs/core_dialogproject.h
    gui/dialogs/core_dialogport.h
    gui/dialogs/core_dialogdevice.h
    gui/dialogs/core_dialogdataview.h
    gui/dialogs/core_dialogdataviewitem.h
    gui/dialogs/core_dialogvaluelist.h
    gui/dialogs/core_dialogs.h
    gui/project/core_projectdelegate.h
    gui/project/core_projectmodel.h
    gui/project/core_projectui.h
    gui/dataview/core_dataviewdelegate.h
    gui/dataview/core_dataviewmanager.h
    gui/dataview/core_dataviewmodel.h
    gui/dataview/core_dataviewui.h
    gui/help/core_helpbrowser.h
    gui/help/core_helpui.h
    gui/core_logview.h
    gui/core_windowmanager.h
    gui/core_ui.h
    runtime/core_runtaskthread.h
    runtime/core_runtime.h
)

set(SOURCES               
    sdk/mbcore.cpp
    sdk/mbcore_base.cpp
    core/core.cpp
    core/core_global.cpp
    task/core_taskfactoryinfo.cpp
    plugin/core_pluginmanager.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/core_project.cpp 
    ${CMAKE_CURRENT_LIST_DIR}/project/core_port.cpp    
    ${CMAKE_CURRENT_LIST_DIR}/project/core_device.cpp  
    ${CMAKE_CURRENT_LIST_DIR}/project/core_dataview.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/core_taskinfo.cpp
    ${CMAKE_CURRENT_LIST_DIR}/project/core_dom.cpp     
    ${CMAKE_CURRENT_LIST_DIR}/project/core_builder.cpp 
    gui/dialogs/core_dialogbase.cpp          
    gui/dialogs/core_dialogsystemsettings.cpp
    gui/dialogs/core_dialogproject.cpp       
    gui/dialogs/core_dialogport.cpp          
    gui/dialogs/core_dialogdevice.cpp        
    gui/dialogs/core_dialogdataview.cpp      
    gui/dialogs/core_dialogdataviewitem.cpp  
    gui/dialogs/core_dialogvaluelist.cpp
    gui/dialogs/core_dialogs.cpp             
    gui/project/core_projectdelegate.cpp
    gui/project/core_projectmodel.cpp
    gui/project/core_projectui.cpp
    gui/dataview/core_dataviewdelegate.cpp
    gui/dataview/core_dataviewmanager.cpp
    gui/dataview/core_dataviewmodel.cpp
    gui/dataview/core_dataviewui.cpp
    gui/help/core_helpbrowser.cpp
    gui/help/core_helpui.cpp
    gui/core_logview.cpp
    gui/core_windowmanager.cpp
    gui/core_ui.cpp
    runtime/core_runtaskthread.cpp
    runtime/core_runtime.cpp
)     

set(RESOURCES 
    gui/core_rsc.qrc              
    )     

include_directories(.
                    ./../modbus/src
                    ./sdk
                    ./core
)

add_library(${MBTOOLS_CORE_LIB_NAME} SHARED ${HEADERS} ${SOURCES} ${RESOURCES})
target_compile_definitions(${MBTOOLS_CORE_LIB_NAME} PRIVATE QT_NO_KEYWORDS MB_EXPORTS)

set_target_properties(
    ${MBTOOLS_CORE_LIB_NAME}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_link_libraries(${MBTOOLS_CORE_LIB_NAME} PRIVATE 
                        Qt${QT_VERSION_MAJOR}::Core
                        Qt${QT_VERSION_MAJOR}::Gui
                        Qt${QT_VERSION_MAJOR}::Widgets
                        Qt${QT_VERSION_MAJOR}::Xml
                        Qt${QT_VERSION_MAJOR}::Help
                        modbus
)

