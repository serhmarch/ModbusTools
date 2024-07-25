#include "ModbusTcpPort.h"
#include "ModbusTcpPort_p.h"

ModbusTcpPort::Defaults::Defaults() :
    host   (StringLiteral("localhost")),
    port   (STANDARD_TCP_PORT),
    timeout(3000)
{
}

const ModbusTcpPort::Defaults &ModbusTcpPort::Defaults::instance()
{
    static const Defaults d;
    return d;
}

const Char *ModbusTcpPort::host() const
{
    return d_ModbusTcpPort(d_ptr)->settings.host.data();
}

void ModbusTcpPort::setHost(const Char *host)
{
    ModbusTcpPortPrivate *d = d_ModbusTcpPort(d_ptr);
    if (d->settings.host != host)
    {
        d->settings.host = host;
        d->setChanged(true);
    }
}

uint16_t ModbusTcpPort::port() const
{
    return d_ModbusTcpPort(d_ptr)->settings.port;
}

void ModbusTcpPort::setPort(uint16_t port)
{
    ModbusTcpPortPrivate *d = d_ModbusTcpPort(d_ptr);
    if (d->settings.port != port)
    {
        d->settings.port = port;
        d->setChanged(true);
    }
}

uint32_t ModbusTcpPort::timeout() const
{
    return d_ModbusTcpPort(d_ptr)->settings.timeout;
}

void ModbusTcpPort::setTimeout(uint32_t timeout)
{
    ModbusTcpPortPrivate *d = d_ModbusTcpPort(d_ptr);
    if (d->settings.timeout != timeout)
    {
        d->settings.timeout = timeout;
        d->setChanged(true);
    }
}

void ModbusTcpPort::setNextRequestRepeated(bool v)
{
    d_ModbusTcpPort(d_ptr)->autoIncrement = !v;
}

bool ModbusTcpPort::autoIncrement() const
{
    return d_ModbusTcpPort(d_ptr)->autoIncrement;
}

const uint8_t *ModbusTcpPort::readBufferData() const
{
    return d_ModbusTcpPort(d_ptr)->buff;
}

uint16_t ModbusTcpPort::readBufferSize() const
{
    return d_ModbusTcpPort(d_ptr)->sz;
}

const uint8_t *ModbusTcpPort::writeBufferData() const
{
    return d_ModbusTcpPort(d_ptr)->buff;
}

uint16_t ModbusTcpPort::writeBufferSize() const
{
    return d_ModbusTcpPort(d_ptr)->sz;
}

StatusCode ModbusTcpPort::writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff)
{
    ModbusTcpPortPrivate *d = d_ModbusTcpPort(d_ptr);
    if (!d->modeServer)
    {
        d->transaction += d->autoIncrement;
        d->autoIncrement = true;
    } // if (!d->modeServer)
    // 8 = 6(TCP prefix size in bytes) + 2(unit and function bytes)
    if (szInBuff > MBCLIENTTCP_BUFF_SZ - 8)
        return d->setError(Status_BadWriteBufferOverflow, StringLiteral("TCP. Write-buffer overflow"));
    // standart TCP message prefix
    d->buff[0] = static_cast<uint8_t>(d->transaction >> 8);  // transaction id
    d->buff[1] = static_cast<uint8_t>(d->transaction);       // transaction id
    d->buff[2] = 0;
    d->buff[3] = 0;
    uint16_t cBytes = szInBuff + 2; // quantity of next bytes
    d->buff[4] = reinterpret_cast<uint8_t*>(&cBytes)[1]; // quantity of next bytes (MSB)
    d->buff[5] = reinterpret_cast<uint8_t*>(&cBytes)[0]; // quantity of next bytes (LSB)
    // unit, function, data
    d->buff[6] = unit;
    d->buff[7] = func;
    memcpy(&d->buff[8], buff, szInBuff);
    d->sz = szInBuff + 8;
    return Status_Good;
}

StatusCode ModbusTcpPort::readBuffer(uint8_t &unit, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff)
{
    ModbusTcpPortPrivate *d = d_ModbusTcpPort(d_ptr);
    if (d->sz < 8)
        return d->setError(Status_BadNotCorrectResponse, StringLiteral("TCP. Not correct response. Responsed data length to small"));

    uint16_t transaction = d->buff[1] | (d->buff[0] << 8);

    if (!((d->buff[2] == 0) && (d->buff[3] == 0)))
        return d->setError(Status_BadNotCorrectResponse, StringLiteral("TCP. Not correct read-buffer's TCP-prefix"));

    uint16_t cBytes = d->buff[5] | (d->buff[4] << 8);
    if (cBytes != (d->sz-6))
        return d->setError(Status_BadNotCorrectResponse, StringLiteral("TCP. Not correct read-buffer's TCP-prefix. Size defined in TCP-prefix is not equal to actual response-size"));
    
    if (d->modeServer)
    {
        d->transaction = transaction;
    }
    else
    {
        if (d->transaction != transaction)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("TCP. Not correct response. Requested transaction id is not equal to responded"));
    }
    unit = d->buff[6];
    func = d->buff[7];

    d->sz = d->sz - 8;
    if (d->sz > maxSzBuff)
        d->sz = maxSzBuff;
    memcpy(buff, &d->buff[8], d->sz);
    *szOutBuff = d->sz;
    return Status_Good;
}
