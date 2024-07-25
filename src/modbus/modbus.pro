TEMPLATE = lib

#CONFIG += c++17
CONFIG += qt

DEFINES += MODBUS_EXPORTS

DESTDIR = ../bin

unix:QMAKE_RPATHDIR += .

HEADERS +=                          \
    $$PWD/Modbus_config.h           \
    $$PWD/ModbusPlatform.h          \
    $$PWD/ModbusGlobal.h            \
    $$PWD/Modbus.h                  \
    $$PWD/cModbus.h                 \
    $$PWD/ModbusObject.h            \
    $$PWD/ModbusPort.h              \
    $$PWD/ModbusPort_p.h            \
    $$PWD/ModbusSerialPort.h        \
    $$PWD/ModbusRtuPort.h           \
    $$PWD/ModbusAscPort.h           \
    $$PWD/ModbusTcpPort.h           \
    $$PWD/ModbusClientPort.h        \
    $$PWD/ModbusClient_p.h          \
    $$PWD/ModbusClient.h            \
    $$PWD/ModbusServerPort.h        \
    $$PWD/ModbusServerResource.h    \
    $$PWD/ModbusTcpServer.h         \
    $$PWD/ModbusClientPort_p.h      \
    $$PWD/ModbusObject_p.h          \
    $$PWD/ModbusSerialPort_p.h      \
    $$PWD/ModbusServerPort_p.h      \
    $$PWD/ModbusServerResource_p.h  \
    $$PWD/ModbusTcpPort_p.h         \
    $$PWD/ModbusTcpServer_p.h       \

SOURCES +=                          \
    $$PWD/Modbus.cpp                \
    $$PWD/cModbus.cpp               \
    $$PWD/ModbusObject.cpp          \
    $$PWD/ModbusPort.cpp            \
    $$PWD/ModbusSerialPort.cpp      \
    $$PWD/ModbusRtuPort.cpp         \
    $$PWD/ModbusAscPort.cpp         \
    $$PWD/ModbusTcpPort.cpp         \
    $$PWD/ModbusClientPort.cpp      \
    $$PWD/ModbusClient.cpp          \
    $$PWD/ModbusServerPort.cpp      \
    $$PWD/ModbusServerResource.cpp  \
    $$PWD/ModbusTcpServer.cpp       \

contains(CONFIG, qt) {
#    message("Qt support is enabled.")
QT = core
HEADERS +=                          \
    $$PWD/ModbusQt.h                \

SOURCES +=                          \
    $$PWD/ModbusQt.cpp              \

} else {
#    message("Qt support is NOT enabled.")
}

win32 {

HEADERS +=                              \
    $$PWD/win/Modbus_win.h              \
    $$PWD/win/ModbusTCP_win.h           \
    $$PWD/win/ModbusSerialPort_p_win.h  \
    $$PWD/win/ModbusTcpPort_p_win.h     \
    $$PWD/win/ModbusTcpServer_p_win.h   \

SOURCES +=                              \
    $$PWD/win/Modbus_win.cpp            \
    $$PWD/win/ModbusTcpPort_win.cpp     \
    $$PWD/win/ModbusTcpServer_win.cpp   \
    $$PWD/win/ModbusSerialPort_win.cpp  \

LIBS += -lWs2_32
LIBS += -lWinmm

}

unix {

HEADERS +=                                \
    $$PWD/unix/Modbus_unix.h              \
    $$PWD/unix/ModbusTCP_unix.h           \
    $$PWD/unix/ModbusSerialPort_p_unix.h  \
    $$PWD/unix/ModbusTcpPort_p_unix.h     \
    $$PWD/unix/ModbusTcpServer_p_unix.h   \

SOURCES +=                                \
    $$PWD/unix/Modbus_unix.cpp            \
    $$PWD/unix/ModbusTcpPort_unix.cpp     \
    $$PWD/unix/ModbusTcpServer_unix.cpp   \
    $$PWD/unix/ModbusSerialPort_unix.cpp  \

}

