/*!
 * \file   ModbusServerResource.h
 * \brief  The header file defines the class that controls specific port
 *
 * \author march
 * \date   May 2024
 */
#ifndef MODBUSSERVERRESOURCE_H
#define MODBUSSERVERRESOURCE_H

#include "ModbusServerPort.h"

class ModbusPort;

/*! \brief Implements direct control for `ModbusPort` derived classes (TCP or serial) for server side.

    \details `ModbusServerResource` derived from `ModbusServerPort` and makes `ModbusPort` object behaves 
    like server port. Pointer to `ModbusPort` object is passed to `ModbusServerResource` constructor.

    Also `ModbusServerResource` have `ModbusInterface` object as second parameter of constructor which
    process every Modbus function request.

 */
class MODBUS_EXPORT ModbusServerResource : public ModbusServerPort
{
public:
    /// \details Constructor of the class.
    /// \param[in]  port    Pointer to the `ModbusPort` which is managed by the current class object.
    /// \param[in]  device  Pointer to the `ModbusInterface` implementation to which all requests for Modbus functions are forwarded.
    ModbusServerResource(ModbusPort *port, ModbusInterface *device);

public:
    /// \details Returns pointer to inner port which was previously passed in constructor.
    ModbusPort *port() const;

public: // server port interface
    /// \details Returns type of Modbus protocol. Same as `port()->type()`.
    Modbus::ProtocolType type() const override;

    Modbus::StatusCode open() override;

    Modbus::StatusCode close() override;

    bool isOpen() const override;

    Modbus::StatusCode process() override;

protected:
    /// \details Process input data `buff` with `size` and returns status of the operation.
    virtual Modbus::StatusCode processInputData(const uint8_t *buff, uint16_t sz);

    /// \details Transfer input request Modbus function to inner device and returns status of the operation.
    virtual Modbus::StatusCode processDevice();


    /// \details Process output data `buff` with `size` and returns status of the operation.
    virtual Modbus::StatusCode processOutputData(uint8_t *buff, uint16_t &sz);

protected:
    using ModbusServerPort::ModbusServerPort;
};

#endif // MODBUSSERVERRESOURCE_H
