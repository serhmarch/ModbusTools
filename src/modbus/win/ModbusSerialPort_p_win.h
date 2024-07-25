#ifndef MODBUSSERIALPORT_P_WIN_H
#define MODBUSSERIALPORT_P_WIN_H

#include <Windows.h>

#include "../ModbusSerialPort_p.h"

class ModbusSerialPortPrivateWin : public ModbusSerialPortPrivate
{
public:
    ModbusSerialPortPrivateWin(bool blocking) :
        ModbusSerialPortPrivate(blocking)
    {
        this->serialPort = INVALID_HANDLE_VALUE;
        this->timestamp = 0;
    }

public:
    inline bool serialPortIsInvalid() const { return serialPort == INVALID_HANDLE_VALUE; }
    inline bool serialPortIsValid() const { return serialPort != INVALID_HANDLE_VALUE; }
    inline bool serialPortIsOpen() const { return serialPortIsValid(); }
    inline void serialPortClose() { CloseHandle(serialPort); }
    inline void timestampRefresh() { timestamp = GetTickCount(); }

public:
    HANDLE serialPort;
    DWORD timestamp;

};

inline ModbusSerialPortPrivateWin *d_win(ModbusPortPrivate *d_ptr) { return static_cast<ModbusSerialPortPrivateWin*>(d_ptr); }

#endif // MODBUSSERIALPORT_P_WIN_H
