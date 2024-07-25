#ifndef MODBUSSERIALPORT_P_UNIX_H
#define MODBUSSERIALPORT_P_UNIX_H

#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#include "Modbus_unix.h"
#include "../ModbusSerialPort_p.h"


class ModbusSerialPortPrivateUnix : public ModbusSerialPortPrivate
{
public:
    ModbusSerialPortPrivateUnix(bool blocking) :
        ModbusSerialPortPrivate(blocking)
    {
        this->serialPort = -1;
        this->timestamp = 0;
    }

public:
    inline bool serialPortIsInvalid() const { return serialPort == -1; }
    inline bool serialPortIsValid() const { return serialPort != -1; }
    inline bool serialPortIsOpen() const { return serialPortIsValid(); }
    inline void serialPortClose() { close(serialPort); }
    inline void timestampRefresh() { timestamp = timer(); }

public:
    int serialPort;
    Timer timestamp;

};

inline ModbusSerialPortPrivateUnix *d_unix(ModbusPortPrivate *d_ptr) { return static_cast<ModbusSerialPortPrivateUnix*>(d_ptr); }

#endif // MODBUSSERIALPORT_P_UNIX_H
