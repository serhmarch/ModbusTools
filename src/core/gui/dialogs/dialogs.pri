include(settings/settings.pri)

HEADERS += \
    $$PWD/core_dialogbase.h             \
    $$PWD/core_dialogedit.h \
    $$PWD/core_dialogprojectinfo.h \
    $$PWD/core_dialogsystemsettings.h   \
    $$PWD/core_dialogproject.h          \
    $$PWD/core_dialogport.h             \
    $$PWD/core_dialogdevice.h           \
    $$PWD/core_dialogdataview.h         \
    $$PWD/core_dialogdataviewitem.h     \
    $$PWD/core_dialogvaluelist.h        \
    $$PWD/core_dialogs.h                \

SOURCES += \
    $$PWD/core_dialogbase.cpp           \
    $$PWD/core_dialogprojectinfo.cpp \
    $$PWD/core_dialogsystemsettings.cpp \
    $$PWD/core_dialogproject.cpp        \
    $$PWD/core_dialogport.cpp           \
    $$PWD/core_dialogdevice.cpp         \
    $$PWD/core_dialogdataview.cpp       \
    $$PWD/core_dialogdataviewitem.cpp   \
    $$PWD/core_dialogvaluelist.cpp      \
    $$PWD/core_dialogs.cpp

FORMS += \
    $$PWD/core_dialogprojectinfo.ui \
    $$PWD/core_dialogsystemsettings.ui  \
    $$PWD/core_dialogproject.ui         \
    $$PWD/core_dialogdataview.ui        \
    $$PWD/core_dialogvaluelist.ui
