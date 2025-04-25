include(dialogs/dialogs.pri)
include(project/project.pri)
include(device/device.pri)
include(simactions/simactions.pri)
include(scriptmodules//scriptmodules.pri)
include(dataview/dataview.pri)
include(script/script.pri)

HEADERS += \
    $$PWD/server_outputview.h \
    $$PWD/server_windowmanager.h    \
    $$PWD/server_ui.h               \
    
SOURCES += \
    $$PWD/server_outputview.cpp \
    $$PWD/server_windowmanager.cpp  \
    $$PWD/server_ui.cpp             \

FORMS += \
    $$PWD/server_ui.ui

RESOURCES += \
    $$PWD/server_rsc.qrc
