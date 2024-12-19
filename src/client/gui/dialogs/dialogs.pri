include(settings/settings.pri)

HEADERS += \
    $$PWD/client_dialogdevice.h         \
    $$PWD/client_dialogport.h           \
    $$PWD/client_dialogs.h              \
    $$PWD/client_dialogdataviewitem.h
    
SOURCES += \
    $$PWD/client_dialogdevice.cpp       \
    $$PWD/client_dialogport.cpp         \
    $$PWD/client_dialogs.cpp            \
    $$PWD/client_dialogdataviewitem.cpp

FORMS += \
    $$PWD/client_dialogdevice.ui        \
    $$PWD/client_dialogport.ui          \
    $$PWD/client_dialogdataviewitem.ui
