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
#include "../ModbusTcpServer.h"

#include "ModbusTcpServer_p_unix.h"

ModbusTcpServer::ModbusTcpServer(ModbusInterface *device) :
    ModbusServerPort(new ModbusTcpServerPrivateWin(device))
{
}

StatusCode ModbusTcpServer::open()
{
    ModbusTcpServerPrivateWin *d = d_unix(d_ptr);
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_CLOSED:
        case STATE_WAIT_FOR_OPEN:
        {
            if (isOpen())
            {
                d->state = STATE_OPENED;
                return Status_Good;
            }

            d->socket->create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (d->socket->isInvalid())
            {
                d->state = STATE_CLOSED;
                return d->setErrorBase(Status_BadTcpCreate, StringLiteral("TCP socket creation error"));
            }

            // Bind the socket
            sockaddr_in serverAddr;
            serverAddr.sin_family = AF_INET;
            serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // Bind to any available interface
            serverAddr.sin_port = htons(d->tcpPort); // Port number

            if (d->socket->bind((sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setErrorBase(Status_BadTcpBind, StringLiteral("Bind error"));
            }

            // Listen on the socket
            if (d->socket->listen(SOMAXCONN) == SOCKET_ERROR)
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setErrorBase(Status_BadTcpListen, StringLiteral("Listen error"));
            }
            d->socket->setBlocking(false);
        }
            return Status_Good;
        default:
            if (!isOpen())
            {
                d->state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            return Status_Good;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode ModbusTcpServer::close()
{
    ModbusTcpServerPrivateWin *d = d_unix(d_ptr);
    if (isOpen())
        d->socket->close();
    d->cmdClose = true;
    for (ModbusServerPort *c : d->connections)
        c->close();
    switch (d->state)
    {
    case STATE_WAIT_FOR_CLOSE:
        for (ModbusServerPort *c : d->connections)
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

bool ModbusTcpServer::isOpen() const
{
    return d_unix(d_ptr)->socket->isValid();
}

ModbusTcpSocket *ModbusTcpServer::nextPendingConnection()
{
    ModbusTcpServerPrivateWin *d = d_unix(d_ptr);
    // Accept the incoming connection
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = d->socket->accept((sockaddr*)&clientAddr, &clientAddrSize);
    if (clientSocket == INVALID_SOCKET)
    {
        if (errno != EWOULDBLOCK)
        {
            d->socket->close();
            d->state = STATE_CLOSED;
        }
        return nullptr;
    }

    ModbusTcpSocket *tcp = new ModbusTcpSocket(clientSocket);
    return tcp;
}

bool ModbusTcpServerPrivate::getHostService(ModbusTcpSocket *socket, String &host, String &service)
{
    sockaddr_in clientAddr;
    socklen_t clientAddrSize = sizeof(clientAddr);
    SOCKET clientSocket = socket->socket();
    // Get the remote host name and port number
    char remoteHost[NI_MAXHOST];
    char remoteService[NI_MAXSERV];
    if (getpeername(clientSocket, (sockaddr*)&clientAddr, &clientAddrSize) == 0)
    {
        if (getnameinfo((sockaddr*)&clientAddr, clientAddrSize, remoteHost, NI_MAXHOST, remoteService, NI_MAXSERV, 0) == 0)
        {
            host = remoteHost;
            service = remoteService;
            return true;
        }
    }
    return false;
}

