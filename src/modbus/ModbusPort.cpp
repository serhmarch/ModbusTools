#include "ModbusPort.h"
#include "ModbusPort_p.h"

ModbusPort::ModbusPort(ModbusPortPrivate *d) :
    d_ptr(d)
{
}

ModbusPort::~ModbusPort()
{
    delete d_ptr;
}

void ModbusPort::setNextRequestRepeated(bool /*v*/)
{
}

bool ModbusPort::isChanged() const
{
    return d_ptr->changed;
}

bool ModbusPort::isServerMode() const
{
    return d_ptr->modeServer;
}

void ModbusPort::setServerMode(bool mode)
{
    d_ptr->modeServer = mode;
}

bool ModbusPort::isBlocking() const
{
    return d_ptr->modeSynch;
}

bool ModbusPort::isNonBlocking() const
{
    return !d_ptr->modeSynch;
}

StatusCode ModbusPort::lastErrorStatus() const
{
    return d_ptr->lastErrorStatus();
}

const Modbus::Char *ModbusPort::lastErrorText() const
{
    return d_ptr->lastErrorText();
}

StatusCode ModbusPort::setError(StatusCode status, const Char *text)
{
    return d_ptr->setError(status, String(text));
}
