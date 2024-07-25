#include "../ModbusTcpPort.h"

#include "ModbusTcpPort_p_win.h"

ModbusTcpPort::ModbusTcpPort(ModbusTcpSocket *socket, bool blocking) :
    ModbusPort(new ModbusTcpPortPrivateWin(socket, blocking))
{
}

ModbusTcpPort::ModbusTcpPort(bool blocking) :
    ModbusTcpPort(nullptr, blocking)
{
}

Handle ModbusTcpPort::handle() const
{
    return reinterpret_cast<Handle>(d_win(d_ptr)->socket->socket());
}

Modbus::StatusCode ModbusTcpPort::open()
{
    ModbusTcpPortPrivateWin *d = d_win(d_ptr);
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_BEGIN:
        case STATE_CLOSED:
        {
            d->clearChanged();
            if (isOpen())
            {
                d->state = STATE_OPENED;
                return Status_Good;
            }
            ADDRINFO hints;
            ZeroMemory(&hints, sizeof(hints));
            hints.ai_family = AF_INET;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            d->freeAddr();

            ADDRINFO* addr = nullptr;
            DWORD status = getaddrinfo(d->settings.host.data(), NULL, &hints, &addr);
            if (status != 0)
                return d->setError(Status_BadTcpCreate, StringLiteral("TCP. Error while getting address info"));
            d->addr = addr;
            d->socket->create(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            if (d->socket->isInvalid())
            {
                d->freeAddr();
                return d->setError(Status_BadTcpCreate, StringLiteral("TCP. Error while creating socket"));
            }
            d->socket->setBlocking(isBlocking());
            if (isBlocking())
                d->socket->setTimeout(d->settings.timeout);
            reinterpret_cast<sockaddr_in*>(reinterpret_cast<ADDRINFO*>(d->addr)->ai_addr)->sin_port = htons(d->settings.port);
            d->timestamp = GetTickCount();
            d->state = STATE_WAIT_FOR_OPEN;
        }
        // no need break
        case STATE_WAIT_FOR_OPEN:
        {
            int r = d->socket->connect(reinterpret_cast<ADDRINFO*>(d->addr)->ai_addr, static_cast<int>(reinterpret_cast<ADDRINFO*>(d->addr)->ai_addrlen));
            if ((r == 0) || (WSAGetLastError() == WSAEISCONN))
            {
                d->state = STATE_BEGIN;
                return Status_Good;
            }
            else if (isNonBlocking() && (GetTickCount() - d->timestamp >= d->settings.timeout))
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setError(Status_BadTcpConnect, StringLiteral("TCP. Error while connecting - timeout"));
            }
            else if (isBlocking())
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setError(Status_BadTcpConnect, StringLiteral("TCP. Error while connecting. Error code: ") + toModbusString(WSAGetLastError()));
            }
        }
            break;
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

StatusCode ModbusTcpPort::close()
{
    ModbusTcpPortPrivateWin *d = d_win(d_ptr);
    if (!d->socket->isInvalid())
    {
        d->socket->shutdown();
        d->socket->close();
    }
    d->state = STATE_CLOSED;
    return Status_Good;
}

bool ModbusTcpPort::isOpen() const
{
    ModbusTcpPortPrivateWin *d = d_win(d_ptr);
    if (d->socket->isInvalid())
        return false;
    int error = 0;
    int error_len = sizeof(error);
    int r = d->socket->getsockopt(SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &error_len);
    if (r != 0)
        return false;
    return (error == 0);
}

StatusCode ModbusTcpPort::write()
{
    ModbusTcpPortPrivateWin *d = d_win(d_ptr);
    switch (d->state)
    {
    case STATE_BEGIN:
    case STATE_PREPARE_TO_WRITE:
    case STATE_WAIT_FOR_WRITE:
    case STATE_WAIT_FOR_WRITE_ALL:
    {
        int c = d->socket->send(reinterpret_cast<char*>(d->buff), d->sz, 0);
        if (c > 0)
        {
            d->state = STATE_BEGIN;
            return Status_Good;
        }
        else
        {
            close();
            return d->setError(Status_BadTcpWrite, StringLiteral("TCP. Error while writing"));
        }
    }
        break;
    default:
        break;
    }
    return Status_Processing;
}

StatusCode ModbusTcpPort::read()
{
    ModbusTcpPortPrivateWin *d = d_win(d_ptr);
    const uint16_t size = MBCLIENTTCP_BUFF_SZ;
    switch (d->state)
    {
    case STATE_BEGIN:
    case STATE_PREPARE_TO_READ:
        d->timestamp = GetTickCount();
        d->state = STATE_WAIT_FOR_READ;
        // no need break
    case STATE_WAIT_FOR_READ:
    case STATE_WAIT_FOR_READ_ALL:
    {
        int c = d->socket->recv(reinterpret_cast<char*>(d->buff), size, 0);
        if (c > 0)
        {
            d->sz = static_cast<uint16_t>(c);
            d->state = STATE_BEGIN;
            return Status_Good;
        }
        else if (c == 0)
        {
            close();
            return Status_Uncertain;
            //return d->setError(Status_BadTcpRead, StringLiteral("TCP. Error while reading - remote connection closed"));
        }
        else if (isNonBlocking() && (GetTickCount() - d->timestamp >= d->settings.timeout)) // waiting timeout read first byte elapsed
        {
            close();
            return d->setError(Status_BadTcpRead, StringLiteral("TCP. Error while reading - timeout"));
        }
        else
        {
            int e = WSAGetLastError();
            if (e != WSAEWOULDBLOCK)
            {
                close();
                return d->setError(Status_BadTcpRead, StringLiteral("TCP. Error while reading. Error code: ") + toModbusString(e));
            }
        }
    }
        break;
    default:
        break;
    }
    return Status_Processing;
}
