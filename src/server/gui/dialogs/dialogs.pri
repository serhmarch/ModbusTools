include(settings/settings.pri)

HEADERS += \
    $$PWD/server_dialogdevice.h \
    $$PWD/server_dialogport.h \
    $$PWD/server_dialogs.h \
    $$PWD/server_dialogdataviewitem.h \
    $$PWD/server_dialogsimaction.h \
    $$PWD/server_dialogscriptmodule.h
    #$$PWD/server_dialogtask.h \

SOURCES += \
    $$PWD/server_dialogdevice.cpp \
    $$PWD/server_dialogport.cpp \
    $$PWD/server_dialogs.cpp \
    $$PWD/server_dialogdataviewitem.cpp \
    $$PWD/server_dialogsimaction.cpp \
    $$PWD/server_dialogscriptmodule.cpp
    #$$PWD/server_dialogtask.cpp \

FORMS += \
    $$PWD/server_dialogdevice.ui \
    $$PWD/server_dialogport.ui \
    $$PWD/server_dialogdataviewitem.ui \
    $$PWD/server_dialogsimaction.ui \
    $$PWD/server_dialogscriptmodule.ui
    #$$PWD/server_dialogtask.ui \
