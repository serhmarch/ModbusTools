include(dialogs/dialogs.pri)
include(project/project.pri)
include(dataview/dataview.pri)
include(sendmessage/sendmessage.pri)
include(scanner/scanner.pri)

HEADERS += \
    $$PWD/client_windowmanager.h    \
    $$PWD/client_ui.h               \

SOURCES += \
    $$PWD/client_windowmanager.cpp  \
    $$PWD/client_ui.cpp             \

FORMS += \
    $$PWD/client_ui.ui              \

RESOURCES += \
    $$PWD/client_rsc.qrc
