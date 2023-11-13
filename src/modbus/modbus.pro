TEMPLATE = lib

VERSION = "0.1.0.0"

DESTDIR = ../bin

CONFIG += no_keywords
CONFIG += skip_target_version_ext

DEFINES += MODBUS_EXPORTS

QT += serialport network

unix:QMAKE_RPATHDIR += .

HEADERS +=                      \
    $$PWD/Modbus.h              \
    $$PWD/ModbusPort.h          \
    $$PWD/ModbusPortTCP.h       \
    $$PWD/ModbusPortSerial.h    \
    $$PWD/ModbusPortRTU.h       \
    $$PWD/ModbusPortASC.h       \
    $$PWD/ModbusClientPort.h    \
    $$PWD/ModbusClient.h        \
    $$PWD/ModbusServerPort.h    \
    $$PWD/ModbusServerTCP.h     \

SOURCES +=                      \
    $$PWD/Modbus.cpp            \
    $$PWD/ModbusPort.cpp        \
    $$PWD/ModbusPortTCP.cpp     \
    $$PWD/ModbusPortSerial.cpp  \
    $$PWD/ModbusPortRTU.cpp     \
    $$PWD/ModbusPortASC.cpp     \
    $$PWD/ModbusClientPort.cpp  \
    $$PWD/ModbusClient.cpp      \
    $$PWD/ModbusServerPort.cpp  \
    $$PWD/ModbusServerTCP.cpp   \

