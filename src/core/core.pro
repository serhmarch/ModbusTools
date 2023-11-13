TEMPLATE = lib

VERSION = "0.1.0.0"

CONFIG += no_keywords
CONFIG += skip_target_version_ext

DESTDIR = ../bin

QT = core gui widgets serialport xml help

DEFINES += MB_EXPORTS

INCLUDEPATH += . \
    ./../Modbus \
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

LIBS  += -L../bin -lmodbus
