TEMPLATE = subdirs

# Specify the location where the help files should be installed
HELPDIR = $$OUT_PWD/help

# Install client and server HTML documentation (contents of the doxygen
# html output directories, flattened into help/mbclient and help/mbserver)
help_client.files = $$PWD/doc/output/client/html/*
help_client.path = $$HELPDIR/mbclient
help_client.depends = $(first)

help_server.files = $$PWD/doc/output/server/html/*
help_server.path = $$HELPDIR/mbserver
help_server.depends = $(first)

INSTALLS += help_client help_server
#QMAKE_POST_LINK=$(MAKE) install

CONFIG += ordered

SUBDIRS += modbus/src/modbus.pro
SUBDIRS += src/core
SUBDIRS += src/client
SUBDIRS += src/server
