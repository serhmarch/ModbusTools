TEMPLATE = subdirs

# Specify the location where the help files should be installed
HELPDIR = $$OUT_PWD/bin/help

# List of help files to install
help.files = $$PWD/../help/client/*.qch \
             $$PWD/../help/client/*.qhc \
             $$PWD/../help/server/*.qch \
             $$PWD/../help/server/*.qhc

help.path = $$HELPDIR
help.depends = $(first)
INSTALLS += help
#QMAKE_POST_LINK=$(MAKE) install

CONFIG += ordered

SUBDIRS += modbus/src/modbus.pro
SUBDIRS += src/core
SUBDIRS += src/client
SUBDIRS += src/server
