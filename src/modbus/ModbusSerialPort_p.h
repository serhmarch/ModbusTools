#ifndef MODBUSSERIALPORT_P_H
#define MODBUSSERIALPORT_P_H

#include "ModbusPort_p.h"
#include "ModbusSerialPort.h"

class ModbusSerialPortPrivate : public ModbusPortPrivate
{
public:
    static ModbusSerialPortPrivate *create(bool blocking);

public:
    ModbusSerialPortPrivate(bool blocking) :
        ModbusPortPrivate(blocking)
    {
        const ModbusSerialPort::Defaults &d = ModbusSerialPort::Defaults::instance();

        settings.portName         = d.portName;
        settings.baudRate         = d.baudRate;
        settings.dataBits         = d.dataBits;
        settings.stopBits         = d.stopBits;
        settings.parity           = d.parity  ;
        settings.flowControl      = d.flowControl;
        settings.timeoutFirstByte = d.timeoutFirstByte;
        settings.timeoutInterByte = d.timeoutInterByte;
    }

public:
    struct
    {
        String portName;
        int32_t baudRate;
        int8_t dataBits;
        Parity parity;
        StopBits stopBits;
        FlowControl flowControl;
        uint32_t timeoutFirstByte;
        uint32_t timeoutInterByte;
    } settings;

    uint8_t *buff;
    uint16_t c_buffSz;
    uint16_t sz;

};

inline ModbusSerialPortPrivate *d_ModbusSerialPort(ModbusPortPrivate *d_ptr) { return static_cast<ModbusSerialPortPrivate*>(d_ptr); }

#endif // MODBUSSERIALPORT_P_H
