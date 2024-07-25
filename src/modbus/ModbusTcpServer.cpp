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
#include "ModbusTcpServer.h"
#include "ModbusTcpServer_p.h"

#include "ModbusTcpPort.h"
#include "ModbusServerResource.h"

ModbusTcpServer::Defaults::Defaults() :
    port   (STANDARD_TCP_PORT),
    timeout(3000)
{
}

const ModbusTcpServer::Defaults &ModbusTcpServer::Defaults::instance()
{
    static const Defaults d;
    return d;
}

uint16_t ModbusTcpServer::port() const
{
    return d_ModbusTcpServer(d_ptr)->tcpPort;
}

void ModbusTcpServer::setPort(uint16_t port)
{
    d_ModbusTcpServer(d_ptr)->tcpPort = port;
}

int ModbusTcpServer::timeout() const
{
    return d_ModbusTcpServer(d_ptr)->timeout;
}

void ModbusTcpServer::setTimeout(int timeout)
{
    d_ModbusTcpServer(d_ptr)->timeout = timeout;
}

void ModbusTcpServer::clearConnections()
{
    ModbusTcpServerPrivate *d = d_ModbusTcpServer(d_ptr);
    for (auto& elem : d->connections)
        delete elem;
    d->connections.clear();

}

StatusCode ModbusTcpServer::process()
{
    ModbusTcpServerPrivate *d = d_ModbusTcpServer(d_ptr);
    StatusCode r;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_CLOSED:
            //if (d->cmdClose)
            //    break;
            d->state = STATE_WAIT_FOR_OPEN;
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (d->cmdClose)
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            r = open();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))  // an error occured
            {
                signalError(d->getName(), r, d->lastErrorText());
                d->state = STATE_CLOSED;
                return r;
            }
            d->state = STATE_OPENED;
            signalOpened(d->getName());
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = close();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))  // an error occured
            {
                signalError(d->getName(), r, d->lastErrorText());
            }
            d->state = STATE_CLOSED;
            signalClosed(d->getName());
            clearConnections();
            //setMessage("Finalized");
            break;
        case STATE_OPENED:
            ////setMessage("Initialized. Waiting for connections...");
            d->state = STATE_PROCESS_DEVICE;
            // no need break
        case STATE_PROCESS_DEVICE:
        {
            if (d->cmdClose)
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            // check up new connection
            if (ModbusTcpSocket *s = this->nextPendingConnection())
            {
                ModbusServerPort *c = createTcpPort(s);
                c->connect(&ModbusServerPort::signalTx   , static_cast<ModbusServerPort*>(this), &ModbusTcpServer::signalTx   );
                c->connect(&ModbusServerPort::signalRx   , static_cast<ModbusServerPort*>(this), &ModbusTcpServer::signalRx   );
                c->connect(&ModbusServerPort::signalError, static_cast<ModbusServerPort*>(this), &ModbusTcpServer::signalError);
                d->connections.push_back(c);
                signalNewConnection(c->objectName());
            }
            // process current connections
            for (Connections_t::iterator it = d->connections.begin(); it != d->connections.end(); )
            {
                ModbusServerPort *c = *it;
                c->process();
                if (!c->isOpen())
                {
                    signalCloseConnection(c->objectName());
                    it = d->connections.erase(it);
                    delete c;
                    continue;
                }
                it++;
            }
        }
            break;
        default:
            if (d->cmdClose && isOpen())
                d->state = STATE_WAIT_FOR_CLOSE;
            else if (isOpen())
                d->state = STATE_OPENED;
            else
                d->state = STATE_CLOSED;
            fRepeatAgain = true;
            break;
        }
    }
    while(fRepeatAgain);
    return Status_Processing;
}

ModbusServerPort *ModbusTcpServer::createTcpPort(ModbusTcpSocket *socket)
{
    ModbusTcpPort *tcp = new ModbusTcpPort(socket);
    ModbusServerResource *c = new ModbusServerResource(tcp, device());
    String host, service;
    if (ModbusTcpServerPrivate::getHostService(socket, host, service))
    {
        String name = host + StringLiteral(":") + service;
        c->setObjectName(name.data());
    }
    return c;
}

void ModbusTcpServer::signalNewConnection(const Modbus::Char *source)
{
    this->emitSignal(__func__, &ModbusTcpServer::signalNewConnection, source);
}

void ModbusTcpServer::signalCloseConnection(const Modbus::Char *source)
{
    this->emitSignal(__func__, &ModbusTcpServer::signalCloseConnection, source);
}
