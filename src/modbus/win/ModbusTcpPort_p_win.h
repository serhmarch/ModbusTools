#ifndef MODBUSTCPPORT_P_WIN_H
#define MODBUSTCPPORT_P_WIN_H

#include "../ModbusTcpPort_p.h"

#include "ModbusTCP_win.h"

class ModbusTcpPortPrivateWin : public ModbusTcpPortPrivate
{
public:
    ModbusTcpPortPrivateWin(ModbusTcpSocket *socket, bool blocking) :
        ModbusTcpPortPrivate(blocking)
    {
        WSADATA data;
        WSAStartup(0x202, &data);

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
        WSACleanup();
    }

public:
    inline void freeAddr()
    {
        if (this->addr)
        {
            freeaddrinfo(reinterpret_cast<ADDRINFO*>(this->addr));
            this->addr = nullptr;
        }
    }

public:
    ModbusTcpSocket *socket;
    DWORD timestamp;
    void *addr;
};

inline ModbusTcpPortPrivateWin *d_win(ModbusPortPrivate *d_ptr) { return static_cast<ModbusTcpPortPrivateWin*>(d_ptr); }

#endif // MODBUSTCPPORT_P_WIN_H
