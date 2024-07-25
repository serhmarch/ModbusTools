#include "../ModbusTcpPort.h"

#include "ModbusTcpPort_p_unix.h"

#include <string.h>

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
    return reinterpret_cast<Handle>(d_unix(d_ptr)->socket->socket());
}

Modbus::StatusCode ModbusTcpPort::open()
{
    ModbusTcpPortPrivateWin *d = d_unix(d_ptr);
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
            struct addrinfo hints;
            memset(&hints, 0, sizeof hints);
            hints.ai_family = AF_UNSPEC;
            hints.ai_socktype = SOCK_STREAM;
            hints.ai_protocol = IPPROTO_TCP;

            d->freeAddr();

            struct addrinfo* addr = nullptr;
            int status = getaddrinfo(d->settings.host.data(), nullptr, &hints, &addr);
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
            reinterpret_cast<sockaddr_in*>(d->addr->ai_addr)->sin_port = htons(d->settings.port);
            d->timestamp = timer();
            d->state = STATE_WAIT_FOR_OPEN;
        }
        // no need break
        case STATE_WAIT_FOR_OPEN:
        {
            int r = d->socket->connect(d->addr->ai_addr, static_cast<int>(d->addr->ai_addrlen));
            if ((r == 0) || (errno == EISCONN))
            {
                d->state = STATE_BEGIN;
                return Status_Good;
            }
            else if (isNonBlocking() && (timer() - d->timestamp >= d->settings.timeout))
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setError(Status_BadTcpConnect, StringLiteral("TCP. Error while connecting - timeout"));
            }
            else if (isBlocking())
            {
                d->socket->close();
                d->state = STATE_CLOSED;
                return d->setError(Status_BadTcpConnect, StringLiteral("TCP. Error while connecting. Error code: ") + toModbusString(errno));
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
    ModbusTcpPortPrivateWin *d = d_unix(d_ptr);
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
    ModbusTcpPortPrivateWin *d = d_unix(d_ptr);
    if (d->socket->isInvalid())
        return false;
    int error = 0;
    socklen_t error_len = sizeof(error);
    int r = d->socket->getsockopt(SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&error), &error_len);
    if (r != 0)
        return false;
    return (error == 0);
}

StatusCode ModbusTcpPort::write()
{
    ModbusTcpPortPrivateWin *d = d_unix(d_ptr);
    switch (d->state)
    {
    case STATE_BEGIN:
    case STATE_PREPARE_TO_WRITE:
    case STATE_WAIT_FOR_WRITE:
    case STATE_WAIT_FOR_WRITE_ALL:
    {
        ssize_t c = d->socket->send(reinterpret_cast<char*>(d->buff), d->sz, 0);
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
    ModbusTcpPortPrivateWin *d = d_unix(d_ptr);
    const uint16_t size = MBCLIENTTCP_BUFF_SZ;
    switch (d->state)
    {
    case STATE_BEGIN:
    case STATE_PREPARE_TO_READ:
        d->timestamp = timer();
        d->state = STATE_WAIT_FOR_READ;
        // no need break
    case STATE_WAIT_FOR_READ:
    case STATE_WAIT_FOR_READ_ALL:
    {
        ssize_t c = d->socket->recv(reinterpret_cast<char*>(d->buff), size, 0);
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
        else if (isNonBlocking() && (timer() - d->timestamp >= d->settings.timeout)) // waiting timeout read first byte elapsed
        {
            close();
            return d->setError(Status_BadTcpRead, StringLiteral("TCP. Error while reading - timeout"));
        }
        else
        {
            int e = errno;
            if (e != EWOULDBLOCK)
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
