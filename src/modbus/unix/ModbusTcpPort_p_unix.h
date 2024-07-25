#ifndef MODBUSTCPPORT_P_UNIX_H
#define MODBUSTCPPORT_P_UNIX_H

#include <netdb.h>

#include "../ModbusTcpPort_p.h"

#include "ModbusTCP_unix.h"

class ModbusTcpPortPrivateWin : public ModbusTcpPortPrivate
{
public:
    ModbusTcpPortPrivateWin(ModbusTcpSocket *socket, bool blocking) :
        ModbusTcpPortPrivate(blocking)
    {
        this->timestamp = 0;
        this->addr = nullptr;

        if (socket)
        {
            socket->setBlocking(isBlocking());
            this->socket = socket;
        }
        else
        {
            this->socket = new ModbusTcpSocket();
        }
    }

    ~ModbusTcpPortPrivateWin()
    {
        this->freeAddr();
    }

public:
    inline void freeAddr()
    {
        if (this->addr)
        {
            freeaddrinfo(addr);
            this->addr = nullptr;
        }
    }

public:
    ModbusTcpSocket *socket;
    Timer timestamp;
    struct addrinfo *addr;
};

inline ModbusTcpPortPrivateWin *d_unix(ModbusPortPrivate *d_ptr) { return static_cast<ModbusTcpPortPrivateWin*>(d_ptr); }

#endif // MODBUSTCPPORT_P_UNIX_H
