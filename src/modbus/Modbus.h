/*!
 * \file   Modbus.h
 * \brief  Contains general definitions of the Modbus protocol.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUS_H
#define MODBUS_H

#include <string>
#include <list>

#include "ModbusGlobal.h"

class ModbusPort;
class ModbusClientPort;
class ModbusServerPort;

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- Modbus interface -------------------------------------------
// --------------------------------------------------------------------------------------------------------

/*! \brief Main interface of Modbus communication protocol.

    \details `ModbusInterface` constains list of functions that ModbusLib is supported.
    There are such functions as:
    * 1  (0x01) - `READ_COILS`
    * 2  (0x02) - `READ_DISCRETE_INPUTS`
    * 3  (0x03) - `READ_HOLDING_REGISTERS`
    * 4  (0x04) - `READ_INPUT_REGISTERS`
    * 5  (0x05) - `WRITE_SINGLE_COIL`
    * 6  (0x06) - `WRITE_SINGLE_REGISTER`
    * 7  (0x07) - `READ_EXCEPTION_STATUS`
    * 15 (0x0F) - `WRITE_MULTIPLE_COILS`
    * 16 (0x10) - `WRITE_MULTIPLE_REGISTERS`

    Default implementation of every Modbus function returns `Modbus::Status_BadIllegalFunction`.
 */
class MODBUS_EXPORT ModbusInterface
{
public:
    /// \details Function for read discrete outputs (coils, 0x bits).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of coils (bits).
    /// \param[out] values  Pointer to the output buffer (bit array) where the read values are stored.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values);

    /// \details Function for read digital inputs (1x bits).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of inputs (bits).
    /// \param[out] values  Pointer to the output buffer (bit array) where the read values are stored.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values);

    /// \details Function for read holding (output) 16-bit registers (4x regs).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of registers.
    /// \param[out] values  Pointer to the output buffer (bit array) where the read values are stored.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

    /// \details Function for read input 16-bit registers (3x regs).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of registers.
    /// \param[out] values  Pointer to the output buffer (bit array) where the read values are stored.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

    /// \details Function for write one separate discrete output (0x coil).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[out] value   Boolean value to be set.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value);

    /// \details Function for write one separate 16-bit holding register (4x).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[out] value   16-bit unsigned integer value to be set.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value);

    /// \details Function to read ExceptionStatus.
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[out] status  Pointer to the byte (bit array) where the exception status is stored.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode readExceptionStatus(uint8_t unit, uint8_t *status);

    /// \details Function for write discrete outputs (coils, 0x bits).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of coils.
    /// \param[out] values  Pointer to the input buffer (bit array) which values must be written.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values);

    /// \details Function for write holding (output) 16-bit registers (4x regs).
    /// \param[in]  unit    Address of the remote Modbus device.
    /// \param[in]  offset  Starting offset (0-based).
    /// \param[in]  count   Count of registers.
    /// \param[out] values  Pointer to the input buffer which values must be written.
    /// \return The result `Modbus::StatusCode` of the operation. Default implementation returns `Status_BadIllegalFunction`.
    virtual Modbus::StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values);
};

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- Modbus namespace -------------------------------------------
// --------------------------------------------------------------------------------------------------------                 

/// Main Modbus namespace. Contains classes, functions and constants to work with Modbus-protocol.
namespace Modbus {

/// \brief Modbus::String class for strings
typedef std::string String;

/// \brief Modbus::List template class
template <class T>
using List = std::list<T>;

/// \details Convert interger value to Modbus::String
/// \returns Returns new Modbus::String value
inline String toModbusString(int val) { return std::to_string(val); }

/// \details Make string representation of bytes array and separate bytes by space
MODBUS_EXPORT String bytesToString(const uint8_t* buff, uint32_t count);

/// \details Make string representation of ASCII array and separate bytes by space
MODBUS_EXPORT String asciiToString(const uint8_t* buff, uint32_t count);

/// \details Return list of names of available serial ports
MODBUS_EXPORT List<String> availableSerialPorts();

/// \details Return list of baud rates
MODBUS_EXPORT List<int32_t> availableBaudRate();

/// \details Return list of data bits
MODBUS_EXPORT List<int8_t> availableDataBits();

/// \details Return list of `Parity` values
MODBUS_EXPORT List<Parity> availableParity();

/// \details Return list of `StopBits` values
MODBUS_EXPORT List<StopBits> availableStopBits();

/// \details Return list of `FlowControl` values
MODBUS_EXPORT List<FlowControl> availableFlowControl();

/// \details Function for creation `ModbusPort` with defined parameters:
/// \param[in]  type        Protocol type: TCP, RTU, ASC.
/// \param[in]  settings    For TCP must be pointer: `TcpSettings*`, `SerialSettings*` otherwise.
/// \param[in]  blocking    If true blocking will be set, non blocking otherwise.
MODBUS_EXPORT ModbusPort *createPort(ProtocolType type, const void *settings, bool blocking);

/// \details Function for creation `ModbusClientPort` with defined parameters:
/// \param[in]  type        Protocol type: TCP, RTU, ASC.
/// \param[in]  settings    For TCP must be pointer: `TcpSettings*`, `SerialSettings*` otherwise.
/// \param[in]  blocking    If true blocking will be set, non blocking otherwise.
MODBUS_EXPORT ModbusClientPort *createClientPort(ProtocolType type, const void *settings, bool blocking);

/// \details Function for creation `ModbusServerPort` with defined parameters:
/// \param[in]  device      Pointer to the `ModbusInterface` implementation to which all requests for Modbus functions are forwarded.
/// \param[in]  type        Protocol type: TCP, RTU, ASC.
/// \param[in]  settings    For TCP must be pointer: `TcpSettings*`, `SerialSettings*` otherwise.
/// \param[in]  blocking    If true blocking will be set, non blocking otherwise.
MODBUS_EXPORT ModbusServerPort *createServerPort(ModbusInterface *device, ProtocolType type, const void *settings, bool blocking);

} //namespace Modbus

#endif // MODBUS_H
