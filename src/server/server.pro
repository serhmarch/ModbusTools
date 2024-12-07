TEMPLATE = app

include(../version.pri)

CONFIG += no_keywords

DESTDIR  = ../bin

QT = core gui widgets xml

unix:QMAKE_RPATHDIR += .

INCLUDEPATH += . ..     \
    $$PWD/../modbus/src \
    $$PWD/../core/sdk   \
    $$PWD/../core/core  \
    $$PWD/../core       \
    $$PWD/core

include(core/core.pri)
include(project/project.pri)
include(gui/gui.pri)
include(runtime/runtime.pri)

HEADERS +=

SOURCES += \
    main.cpp

RESOURCES += \
    $$PWD/resource/server_resource.qrc

LIBS  += -L../bin -lcore
LIBS  += -L../modbus/bin -lmodbus

RC_ICONS = gui/icons/server.ico
