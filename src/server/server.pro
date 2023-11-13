TEMPLATE = app

VERSION = "0.1.0.0"

CONFIG += no_keywords
CONFIG += skip_target_version_ext

DESTDIR  = ../bin

QT    += gui widgets network serialport xml

unix:QMAKE_RPATHDIR += .

INCLUDEPATH += . .. \
    $$PWD/../Modbus \
    $$PWD/../core/sdk \
    $$PWD/../core/core \
    $$PWD/../core \
    $$PWD/core

include(core/core.pri)
include(project/project.pri)
include(gui/gui.pri)
include(runtime/runtime.pri)

HEADERS +=

SOURCES += \
    main.cpp

LIBS  += -L../bin -lcore
LIBS  += -L../bin -lModbus

RC_ICONS = gui/icons/server.ico
