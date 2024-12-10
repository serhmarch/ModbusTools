include(dialogs/dialogs.pri)
include(project/project.pri)
include(device/device.pri)
include(actions/actions.pri)
include(dataview/dataview.pri)
include(script/script.pri)

HEADERS += \
    $$PWD/server_windowmanager.h    \
    $$PWD/server_ui.h               \
    
SOURCES += \
    $$PWD/server_windowmanager.cpp  \
    $$PWD/server_ui.cpp             \

FORMS += \
    $$PWD/server_ui.ui

RESOURCES += \
    $$PWD/server_rsc.qrc
