TEMPLATE = subdirs

# Specify the location where the help files should be installed
HELPDIR = $$OUT_PWD/help

# List of help files to install
help.files = $$PWD/doc/output/*.qch \
             $$PWD/doc/output/*.qhc

help.path = $$HELPDIR
help.depends = $(first)
INSTALLS += help
#QMAKE_POST_LINK=$(MAKE) install

CONFIG += ordered

SUBDIRS += modbus/src/modbus.pro
SUBDIRS += src/core
SUBDIRS += src/client
SUBDIRS += src/server
