TEMPLATE = app

include(../version.pri)

CONFIG += no_keywords

DESTDIR  = ../bin

QT = core gui widgets network serialport xml

unix:QMAKE_RPATHDIR += .

INCLUDEPATH += . .. \
    $$PWD/../modbus \
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
LIBS  += -L../bin -lmodbus

RC_ICONS = gui/icons/server.ico
