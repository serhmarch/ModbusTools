#ifndef MODBUSTCPPORT_P_H
#define MODBUSTCPPORT_P_H

#include "ModbusPort_p.h"
#include "ModbusTcpPort.h"

#define MBCLIENTTCP_BUFF_SZ MB_TCP_IO_BUFF_SZ

class ModbusTcpPortPrivate : public ModbusPortPrivate
{
public:
    ModbusTcpPortPrivate(bool blocking) :
        ModbusPortPrivate(blocking)
    {
        const ModbusTcpPort::Defaults &d = ModbusTcpPort::Defaults::instance();

        settings.host    = d.host   ;
        settings.port    = d.port   ;
        settings.timeout = d.timeout;

        autoIncrement = true;
        transaction = 0;
        sz = 0;
    }

public:
    struct
    {
        String   host   ;
        uint16_t port   ;
        uint32_t timeout;
    } settings;

    bool autoIncrement;
    uint16_t transaction;
    uint8_t  buff[MBCLIENTTCP_BUFF_SZ];
    uint16_t sz;

};

inline ModbusTcpPortPrivate *d_ModbusTcpPort(ModbusPortPrivate *d_ptr) { return static_cast<ModbusTcpPortPrivate*>(d_ptr); }

#endif // MODBUSTCPPORT_P_H
