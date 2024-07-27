TEMPLATE = lib

include(../version.pri)

CONFIG += no_keywords

DESTDIR = ../bin

QT = core gui widgets xml help

DEFINES += MB_EXPORTS

INCLUDEPATH += . \
    ./../modbus/src \
    ./sdk\
    ./core

unix:QMAKE_RPATHDIR += .

include(sdk/sdk.pri)
include(core/core.pri)
include(task/task.pri)
include(plugin/plugin.pri)
include(project/project.pri)
include(gui/gui.pri)
include(runtime/runtime.pri)

HEADERS +=
     
SOURCES +=

LIBS  += -L../modbus/bin -lmodbus
