/*!
 * \file   ModbusAscPort.h
 * \brief  Contains definition of base server side port class.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSSERVERPORT_H
#define MODBUSSERVERPORT_H

#include "ModbusObject.h"

/*! \brief Abstract base class for direct control of `ModbusPort` derived classes (TCP or serial) for server side.

    \details Pointer to `ModbusPort` object must be passed to `ModbusServerPort` derived class constructor.

    Also assumed that `ModbusServerPort` derived classes must accept `ModbusInterface` object in its constructor
    to process every Modbus function request.

 */
class MODBUS_EXPORT ModbusServerPort : public ModbusObject
{
public:
    /// \details Returns pointer to `ModbusInterface` object/device that was previously passed in constructor.
    /// This device must process every input Modbus function request for this server port
    ModbusInterface *device() const;

public: // server port interface
    /// \details Returns type of Modbus protocol.
    virtual Modbus::ProtocolType type() const = 0;

    /// \details Returns `true` if current server port is TCP server, `false` otherwise.
    virtual bool isTcpServer() const;

    /// \details Open inner port/connection to begin working and returns status of the operation. 
    /// User do not need to call this method directly.
    virtual Modbus::StatusCode open() = 0;

    /// \details Closes port/connection and returns status of the operation.
    virtual Modbus::StatusCode close() = 0;

    /// \details Returns `true` if inner port is open, `false` otherwise.
    virtual bool isOpen() const = 0;

    /// \details Main function of the class. Must be called in the cycle. 
    /// Return statuc code is not very useful but can indicate that inner server operations are good, bad or in process.
    virtual Modbus::StatusCode process() = 0;

public:
    /// \details Returns `true` if current port has closed inner state, `false` otherwise.
    bool isStateClosed() const;

public: // SIGNALS
    /// \details Signal occured when inner port was opened. `source` - current port name.
    void signalOpened(const Modbus::Char *source);

    /// \details Signal occured when inner port was closed. `source` - current port name.
    void signalClosed(const Modbus::Char *source);

    /// \details Signal occured when  the original packet 'Tx' from the internal list of callbacks, 
    /// passing them the original array 'buff' and its size 'size'. `source` - current port name.
    void signalTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);

    /// \details Signal occured when  the incoming packet 'Rx' from the internal list of callbacks, 
    /// passing them the input array 'buff' and its size 'size'. `source` - current port name.
    void signalRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);

    /// \details Signal occured when  error is occured with error's `status` and `text`. `source` - current port name.
    void signalError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text);

protected:
    using ModbusObject::ModbusObject;
};

#endif // MODBUSSERVERPORT_H

