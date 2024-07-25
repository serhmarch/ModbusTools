#include "ModbusClient.h"
#include "ModbusClient_p.h"

ModbusClient::ModbusClient(uint8_t unit, ModbusClientPort *port) :
    ModbusObject(new ModbusClientPrivate)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    d->unit            = unit;
    d->port            = port;
    d->lastStatus      = Status_Uncertain;
    d->lastErrorStatus = Status_Uncertain;

}


Modbus::ProtocolType ModbusClient::type() const
{
    return d_ModbusClient(d_ptr)->port->type();
}

uint8_t ModbusClient::unit() const
{
    return d_ModbusClient(d_ptr)->unit;
}

void ModbusClient::setUnit(uint8_t unit)
{
    d_ModbusClient(d_ptr)->unit = unit;
}

bool ModbusClient::isOpen() const
{
    return d_ModbusClient(d_ptr)->port->isOpen();
}

ModbusClientPort *ModbusClient::port() const
{
    return d_ModbusClient(d_ptr)->port;
}

StatusCode ModbusClient::readCoils(uint16_t offset, uint16_t count, void *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readCoils(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::readDiscreteInputs(uint16_t offset, uint16_t count, void *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readDiscreteInputs(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::readHoldingRegisters(uint16_t offset, uint16_t count, uint16_t *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readHoldingRegisters(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::readInputRegisters(uint16_t offset, uint16_t count, uint16_t *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readInputRegisters(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::writeSingleCoil(uint16_t offset, bool value)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->writeSingleCoil(this, d->unit, offset, value);
}

StatusCode ModbusClient::writeSingleRegister(uint16_t offset, uint16_t value)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->writeSingleRegister(this, d->unit, offset, value);
}

StatusCode ModbusClient::readExceptionStatus(uint8_t *value)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readExceptionStatus(this, d->unit, value);
}

StatusCode ModbusClient::writeMultipleCoils(uint16_t offset, uint16_t count, const void *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->writeMultipleCoils(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::writeMultipleRegisters(uint16_t offset, uint16_t count, const uint16_t *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->writeMultipleRegisters(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::readCoilsAsBoolArray(uint16_t offset, uint16_t count, bool *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readCoilsAsBoolArray(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::readDiscreteInputsAsBoolArray(uint16_t offset, uint16_t count, bool *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->readDiscreteInputsAsBoolArray(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::writeMultipleCoilsAsBoolArray(uint16_t offset, uint16_t count, const bool *values)
{
    ModbusClientPrivate *d = d_ModbusClient(d_ptr);
    return d->port->writeMultipleCoilsAsBoolArray(this, d->unit, offset, count, values);
}

StatusCode ModbusClient::lastPortStatus() const
{
    return d_ModbusClient(d_ptr)->port->lastStatus();
}

StatusCode ModbusClient::lastPortErrorStatus() const
{
    return d_ModbusClient(d_ptr)->port->lastErrorStatus();
}

const Char *ModbusClient::lastPortErrorText() const
{
    return d_ModbusClient(d_ptr)->port->lastErrorText();
}
