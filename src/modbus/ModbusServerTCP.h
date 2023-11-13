/*
    Modbus

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef MODBUSSERVERTCP_H
#define MODBUSSERVERTCP_H

#include "ModbusServerPort.h"

class QTcpServer;
class QTcpSocket;

namespace Modbus {

class MODBUS_EXPORT ServerTCP : public ServerPort
{
public:
    struct MODBUS_EXPORT Strings
    {
        const QString port;
        const QString timeout;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults
    {
        const quint16 port;
        const int timeout;

        Defaults();
        static const Defaults &instance();
    };

public:
    ServerTCP(QTcpServer* server, Interface *device, QObject *parent = nullptr);
    ServerTCP(Interface *device, QObject *parent = nullptr);

public:
    Modbus::Type type() const override{ return Modbus::TCP; }
    StatusCode open() override;
    StatusCode close() override;
    bool isOpen() const override;
    Settings settings() override;
    bool setSettings(const Settings &settings) override;
    StatusCode process() override;
    
public:
    virtual ServerPort *createPortTCP(QTcpSocket *socket);
    
public:
    inline QTcpServer* server() const { return m_server; }
    inline quint16 port() const { return m_tcpPort; }
    inline void setPort(quint16 port) { m_tcpPort = port; }
    inline int timeout() const { return m_timeout; }
    inline void setTimeout(int timeout) { m_timeout = timeout; }

private:
    typedef QList<ServerPort*> Connections_t;

    QTcpServer* m_server;
    quint16 m_tcpPort;
    int m_timeout;
    Connections_t m_connections;
    qint64 m_timestamp;
};

} // namespace Modbus

#endif // MODBUSSERVERTCP_H
