#ifndef MODBUSTCPSERVER_P_UNIX_H
#define MODBUSTCPSERVER_P_UNIX_H

#include "../ModbusTcpServer_p.h"
#include "ModbusTCP_unix.h"

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
        this->socket = new ModbusTcpSocket;
    }

    ~ModbusTcpServerPrivateWin()
    {
        delete this->socket;
    }

public:
    ModbusTcpSocket *socket;
};

inline ModbusTcpServerPrivateWin *d_unix(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusTcpServerPrivateWin*>(d_ptr); }

} // namespace Modbus

#endif // MODBUSTCPSERVER_P_WIN_H
