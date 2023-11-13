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
#include "ModbusServerTCP.h"

#include <QTcpServer>
#include <QTcpSocket>

#include <QDateTime>
#include <QDebug>
#include "ModbusPortTCP.h"

namespace Modbus {

ServerTCP::Strings::Strings() :
    port(QStringLiteral("port")),
    timeout(QStringLiteral("timeout"))
{
}

const ServerTCP::Strings &ServerTCP::Strings::instance()
{
    static const Strings s;
    return s;
}

ServerTCP::Defaults::Defaults() :
    port(static_cast<quint16>(Modbus::STANDARD_TCP_PORT)),
    timeout(3000)
{
}

const ServerTCP::Defaults &ServerTCP::Defaults::instance()
{
    static const Defaults d;
    return d;
}

ServerTCP::ServerTCP(QTcpServer *server, Interface *device, QObject *parent) :
    ServerPort(nullptr, device, parent),
    m_server(server)
{
    Defaults d = Defaults::instance();

    m_tcpPort = d.port;
    m_timeout = d.timeout;
}

ServerTCP::ServerTCP(Interface *device, QObject *parent) :
    ServerTCP(new QTcpServer, device, parent)
{
    m_server->setParent(this);
}

StatusCode ServerTCP::open()
{
    bool fRepeatAgain;
    //QAbstractSocket::SocketState s;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_CLOSED:
            if (isOpen())
            {
                m_state = STATE_OPENED;
                return Status_Good;
            }
            m_server->listen(QHostAddress::Any, port());
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_OPEN;
            //fRepeatAgain = true;
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (m_server->isListening())
            {
                setMessage(QString("Start to listen successfully"));
                return Status_Good;
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp >= m_timeout)
            {
                m_server->close();
                m_state = STATE_CLOSED;
                return setError(Status_BadTcpConnect, QString("TCP-server error while try to listen - %1")
                                                       .arg(m_server->errorString()));
            }
            break;
        default:
            if (!isOpen())
            {
                m_state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            return Status_Good;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode ServerTCP::close()
{
    if (m_server->isListening())
        m_server->close();
    m_cmdClose = true;
    Q_FOREACH (ServerPort *c, m_connections)
        c->close();
    switch (m_state)
    {
    case STATE_WAIT_FOR_CLOSE:
        Q_FOREACH (ServerPort *c, m_connections)
        {
            c->process();
            if (!c->isStateClosed())
                return Status_Processing;
        }
        break;
    default:
        return Status_Processing;
    }
    return Status_Good;
}

bool ServerTCP::isOpen() const
{
    return m_server->isListening();
}

Settings ServerTCP::settings()
{
    Settings params;
    Strings s = Strings::instance();
    params.insert(s.port, port());
    params.insert(s.timeout, timeout());
    return params;
}

bool ServerTCP::setSettings(const Settings &settings)
{
    const Strings &s = Strings::instance();

    Settings::const_iterator it;
    Settings::const_iterator end = settings.end();

    it = settings.find(s.port);
    if (it != end)
    {
        QVariant v = it.value();
        setPort(static_cast<uint16_t>(v.toUInt()));
    }

    it = settings.find(s.timeout);
    if (it != end)
    {
        QVariant v = it.value();
        setTimeout(v.toUInt());
    }

    return true;
}

StatusCode ServerTCP::process()
{
    StatusCode r;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_CLOSED:
            //if (m_cmdClose)
            //    break;
            //m_state = STATE_WAIT_FOR_OPEN;
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (m_cmdClose)
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = open();
            if (r) // if not OK it's mean that an error occured or in process
                return r;
            m_state = STATE_OPENED;
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = close();
            if (r) // if not OK it's mean that an error occured or in process
                return r;
            m_state = STATE_CLOSED;
            qDeleteAll(m_connections);
            m_connections.clear();
            setMessage("Finalized");
            break;
        case STATE_OPENED:
            setMessage("Initialized. Waiting for connections...");
            m_state = STATE_PROCESS_DEVICE;
            // no need break
        case STATE_PROCESS_DEVICE:
        {
            if (m_cmdClose)
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            // check up new connection
            if (QTcpSocket* s = m_server->nextPendingConnection())
            {
                ServerPort *c = createPortTCP(s);
                m_connections.append(c);
                connect(c, &ServerPort::signalTx     , this, &ServerPort::signalTx     );
                connect(c, &ServerPort::signalRx     , this, &ServerPort::signalRx     );
                connect(c, &ServerPort::signalError  , this, &ServerPort::signalError  );
                connect(c, &ServerPort::signalMessage, this, &ServerPort::signalMessage);
                setMessage(QString("New connection from '%1'").arg(c->name()));
            }
            // process current connections
            for (Connections_t::iterator it = m_connections.begin(); it != m_connections.end(); )
            {
                ServerPort *c = *it;
                c->process();
                if (!c->isOpen())
                {
                    setMessage(QString("Close connection from '%1'").arg(c->name()));
                    it = m_connections.erase(it);
                    delete c;
                    continue;
                }
                it++;
            }
        }
            break;
        default:
            if (m_cmdClose && isOpen())
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
            }
            else if (isOpen())
            {
                m_state = STATE_OPENED;
                fRepeatAgain = true;
            }
            else
                m_state = STATE_CLOSED;
            break;
        }
    }
    while(fRepeatAgain);
    return Status_Processing;
}

ServerPort *ServerTCP::createPortTCP(QTcpSocket *socket)
{
    PortTCP *tcp = new PortTCP(socket);
    ServerPort *port = new ServerPort(tcp, device());
    port->setName(socket->localAddress().toString());
    return port;
}

} // namespace Modbus
