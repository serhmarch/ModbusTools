/*!
 * \file   ModbusClient.h
 * \brief  Header file of Modbus client.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSCLIENT_H
#define MODBUSCLIENT_H

#include "ModbusObject.h"

class ModbusClientPort;

/*! \brief The `ModbusClient` class implements the interface of the client part of the Modbus protocol.

    \details `ModbusClient` contains a list of Modbus functions that are implemented by the Modbus client program.
    It implements functions for reading and writing different types of Modbus memory that are defined by the specification.
    The operations that return `Modbus::StatusCode` are asynchronous, that is, if the operation is not completed, it returns the intermediate status `Modbus::Status_Processing`,
    and then it must be called until it is successfully completed or returns an error status.

 */

class MODBUS_EXPORT ModbusClient : public ModbusObject
{
public:
    /// \details Class constructor.
    /// \param[in] unit The address of the remote Modbus device to which this client is bound.
    /// \param[in] port A pointer to the port object to which this client object belongs.
    ModbusClient(uint8_t unit, ModbusClientPort *port);

public:
    /// \details Returns the type of the Modbus protocol.
    Modbus::ProtocolType type() const;

    /// \details Returns the address of the remote Modbus device to which this client is bound.
    uint8_t unit() const;

    /// \details Sets the address of the remote Modbus device to which this client is bound.
    void setUnit(uint8_t unit);

    /// \details Returns `true` if communication with the remote device is established, `false` otherwise.
    bool isOpen() const;

    /// \details Returns a pointer to the port object to which this client object belongs.
    ModbusClientPort *port() const;

public:
    /// \details Same as `ModbusInterface::readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values)`, but the address of the remote Modbus device is missing. It is preset in the constructor.
    Modbus::StatusCode readCoils(uint16_t offset, uint16_t count, void *values);

    /// \details Same as `ModbusInterface::readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values)`, but the address of the remote Modbus device is missing. It is preset in the constructor.
    Modbus::StatusCode readDiscreteInputs(uint16_t offset, uint16_t count, void *values);

    /// \details Same as `ModbusInterface::readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode readHoldingRegisters(uint16_t offset, uint16_t count, uint16_t *values);

    /// \details Same as `ModbusInterface::readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode readInputRegisters(uint16_t offset, uint16_t count, uint16_t *values);

    /// \details Same as `ModbusInterface::writeSingleCoil(uint8_t unit, uint16_t offset, bool value)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode writeSingleCoil(uint16_t offset, bool value);

    /// \details Same as `ModbusInterface::writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode writeSingleRegister(uint16_t offset, uint16_t value);

    /// \details Same as `ModbusInterface::readExceptionStatus(uint8_t unit, uint8_t *status)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode readExceptionStatus(uint8_t *value);

    /// \details Same as `ModbusInterface::writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values)`, but the address of the remote Modbus device is missing. It is preset in the constructor.
    Modbus::StatusCode writeMultipleCoils(uint16_t offset, uint16_t count, const void *values);

    /// \details Same as `ModbusInterface::writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode writeMultipleRegisters(uint16_t offset, uint16_t count, const uint16_t *values);

    /// \details Same as `ModbusClientPort::readCoilsAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, bool *values)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode readCoilsAsBoolArray(uint16_t offset, uint16_t count, bool *values);

    /// \details Same as `ModbusClientPort::readDiscreteInputsAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, bool *values)`, but the address of the remote Modbus device is missing. It is pre-set in the constructor.
    Modbus::StatusCode readDiscreteInputsAsBoolArray(uint16_t offset, uint16_t count, bool *values);

    /// \details Same as `ModbusClientPort::writeMultipleCoilsAsBoolArray(uint8_t unit, uint16_t offset, uint16_t count, const bool *values)`, but the address of the remote Modbus device is missing. It is preset in the constructor.
    Modbus::StatusCode writeMultipleCoilsAsBoolArray(uint16_t offset, uint16_t count, const bool *values);

public:
    /// \details Returns the status of the last operation performed.
    Modbus::StatusCode lastPortStatus() const;

    /// \details Returns the status of the last error of the performed operation.
    Modbus::StatusCode lastPortErrorStatus() const;

    /// \details Returns text repr of the last error of the performed operation.
    const Modbus::Char *lastPortErrorText() const;

protected:
    /// \cond
    using ModbusObject::ModbusObject;
    /// \endcond
};

#endif // MODBUSCLIENT_H
