#ifndef MODBUSTCPSERVER_P_WIN_H
#define MODBUSTCPSERVER_P_WIN_H

#include "../ModbusTcpServer_p.h"
#include "ModbusTCP_win.h"

namespace Modbus {

namespace ModbusTcpServerPrivateNS {

typedef std::list<ModbusServerPort*> Connections_t;

} // namespace ModbusTcpServerPrivateNS

using namespace ModbusTcpServerPrivateNS;

class ModbusTcpServerPrivateWin : public ModbusTcpServerPrivate
{
public:
    ModbusTcpServerPrivateWin(ModbusInterface *device) :
        ModbusTcpServerPrivate(device)
    {
        WSADATA data;
        WSAStartup(0x202, &data);

        this->socket = new ModbusTcpSocket;
    }

    ~ModbusTcpServerPrivateWin()
    {
        delete this->socket;
        WSACleanup();
    }

public:
    ModbusTcpSocket *socket;
};

inline ModbusTcpServerPrivateWin *d_win(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusTcpServerPrivateWin*>(d_ptr); }

} // namespace Modbus

#endif // MODBUSTCPSERVER_P_WIN_H
