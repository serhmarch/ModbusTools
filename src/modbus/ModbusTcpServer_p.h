#ifndef MODBUSTCPSERVER_P_H
#define MODBUSTCPSERVER_P_H

#include <list>

#include "ModbusTcpServer.h"
#include "ModbusServerPort_p.h"

namespace ModbusTcpServerPrivateNS {

typedef std::list<ModbusServerPort*> Connections_t;

} // namespace ModbusTcpServerPrivateNS

using namespace ModbusTcpServerPrivateNS;

class ModbusTcpServerPrivate : public ModbusServerPortPrivate
{
public:
    static bool getHostService(ModbusTcpSocket *socket, String &host, String &service);

public:
    ModbusTcpServerPrivate(ModbusInterface *device) :
        ModbusServerPortPrivate(device)
    {
        const ModbusTcpServer::Defaults &d = ModbusTcpServer::Defaults::instance();

        this->tcpPort = d.port;
        this->timeout = d.timeout;
    }

public:
    uint16_t tcpPort;
    uint32_t timeout;
    Connections_t connections;
};

inline ModbusTcpServerPrivate *d_ModbusTcpServer(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusTcpServerPrivate*>(d_ptr); }

#endif // MODBUSTCPSERVER_P_H
