include(dialogs/dialogs.pri)
include(project/project.pri)
include(dataview/dataview.pri)
include(logview/logview.pri)
include(widgets/widgets.pri)
include(help/help.pri)

HEADERS += \
    $$PWD/core_windowmanager.h \
    $$PWD/core_ui.h
    
SOURCES += \
    $$PWD/core_windowmanager.cpp \
    $$PWD/core_ui.cpp

RESOURCES += \
    $$PWD/core_rsc.qrc
