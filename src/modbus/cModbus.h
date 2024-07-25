/*!
 * \file   cModbus.h
 * \brief  Contains library interface for C language
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef CMODBUS_H
#define CMODBUS_H

#include <stdbool.h>
#include "ModbusGlobal.h"

#ifdef __cplusplus
using namespace Modbus;
extern "C" {
#endif

#ifdef __cplusplus
class ModbusPort      ;
class ModbusClientPort;
class ModbusClient    ;
class ModbusServerPort;
class ModbusInterface ;

#else
typedef struct ModbusPort       ModbusPort      ;
typedef struct ModbusClientPort ModbusClientPort;
typedef struct ModbusClient     ModbusClient    ;
typedef struct ModbusServerPort ModbusServerPort;
typedef struct ModbusInterface  ModbusInterface ;
#endif


/// \brief Handle (pointer) of `ModbusPort` for C interface
typedef ModbusPort* cModbusPort;

/// \brief Handle (pointer) of `ModbusClientPort` for C interface
typedef ModbusClientPort* cModbusClientPort;

/// \brief Handle (pointer) of `ModbusClient` for C interface
typedef ModbusClient* cModbusClient;

/// \brief Handle (pointer) of `ModbusServerPort` for C interface
typedef ModbusServerPort* cModbusServerPort;

/// \brief Handle (pointer) of `ModbusInterface` for C interface
typedef ModbusInterface* cModbusInterface;

/// \brief Handle (pointer) of `ModbusDevice` for C interface
typedef void* cModbusDevice;

/// \details Pointer to C function for read coils (0x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::readCoils`
typedef StatusCode (*pfReadCoils)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, void *values);

/// \details Pointer to C function for read discrete inputs (1x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::readDiscreteInputs`
typedef StatusCode (*pfReadDiscreteInputs)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, void *values);

/// \details Pointer to C function for read holding registers (4x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::readHoldingRegisters`
typedef StatusCode (*pfReadHoldingRegisters)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Pointer to C function for read input registers (3x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::readInputRegisters`
typedef StatusCode (*pfReadInputRegisters)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Pointer to C function for write single coil (0x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::writeSingleCoil`
typedef StatusCode (*pfWriteSingleCoil)(cModbusDevice dev, uint8_t unit, uint16_t offset, bool value);

/// \details Pointer to C function for write single register (4x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::writeSingleRegister`
typedef StatusCode (*pfWriteSingleRegister)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t value);

/// \details Pointer to C function for read exception status bits. `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::readExceptionStatus`
typedef StatusCode (*pfReadExceptionStatus)(cModbusDevice dev, uint8_t unit, uint8_t *status);

/// \details Pointer to C function for write coils (0x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::writeMultipleCoils`
typedef StatusCode (*pfWriteMultipleCoils)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, const void *values);

/// \details Pointer to C function for write registers (4x). `dev` - pointer to any struct that can hold memory data. \sa `ModbusInterface::writeMultipleRegisters`
typedef StatusCode (*pfWriteMultipleRegisters)(cModbusDevice dev, uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusClientPort::signalOpened` and `ModbusServerPort::signalOpened`
typedef void (*pfSlotOpened)(const Char *source);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusClientPort::signalClosed` and `ModbusServerPort::signalClosed`
typedef void (*pfSlotClosed)(const Char *source);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusClientPort::signalTx` and `ModbusServerPort::signalTx`
typedef void (*pfSlotTx)(const Char *source, const uint8_t* buff, uint16_t size);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusClientPort::signalRx` and `ModbusServerPort::signalRx`
typedef void (*pfSlotRx)(const Char *source, const uint8_t* buff, uint16_t size);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusClientPort::signalError` and `ModbusServerPort::signalError`
typedef void (*pfSlotError)(const Char *source, StatusCode status, const Char *text);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusTcpServer::signalNewConnection`
typedef void (*pfSlotNewConnection)(const Char *source);

/// \details Pointer to C callback function. `dev` - pointer to any struct that can hold memory data. \sa `ModbusTcpServer::signalCloseConnection`
typedef void (*pfSlotCloseConnection)(const Char *source);

/// \details Function create `ModbusInterface` object and returns pointer to it for server.
/// `dev` - pointer to any struct that can hold memory data.
/// readCoils,
/// readDiscreteInputs,
/// readHoldingRegisters,
/// readInputRegisters,
/// writeSingleCoil,
/// writeSingleRegister,
/// readExceptionStatus,
/// writeMultipleCoils - pointers to corresponding Modbus functions to process data.
/// Any pointer can have `NULL` value. In this case server will return `Status_BadIllegalFunction`.
MODBUS_EXPORT cModbusInterface cCreateModbusDevice(cModbusDevice            device                , 
                                                   pfReadCoils              readCoils             ,
                                                   pfReadDiscreteInputs     readDiscreteInputs    ,
                                                   pfReadHoldingRegisters   readHoldingRegisters  ,
                                                   pfReadInputRegisters     readInputRegisters    ,
                                                   pfWriteSingleCoil        writeSingleCoil       ,
                                                   pfWriteSingleRegister    writeSingleRegister   ,
                                                   pfReadExceptionStatus    readExceptionStatus   ,
                                                   pfWriteMultipleCoils     writeMultipleCoils    ,
                                                   pfWriteMultipleRegisters writeMultipleRegisters);


/// \details Deletes previously created `ModbusInterface` object represented by `dev` handle
MODBUS_EXPORT void cDeleteModbusDevice(cModbusInterface dev);

// --------------------------------------------------------------------------------------------------------
// ---------------------------------------------- ModbusPort ----------------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \details Creates `ModbusPort` object and returns handle to it. \sa `Modbus::createPort`
MODBUS_EXPORT cModbusPort cPortCreate(ProtocolType type, const void *settings, bool blocking);

/// \details Deletes previously created `ModbusPort` object represented by `port` handle
MODBUS_EXPORT void cPortDelete(cModbusPort port);


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- ModbusClientPort -------------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \details Creates `ModbusClientPort` object and returns handle to it. \sa `Modbus::createClientPort`
MODBUS_EXPORT cModbusClientPort cCpoCreate(ProtocolType type, const void *settings, bool blocking);

/// \details Creates `ModbusClientPort` object and returns handle to it.
MODBUS_EXPORT cModbusClientPort cCpoCreateForPort(cModbusPort port);

/// \details Deletes previously created `ModbusClientPort` object represented by `port` handle
MODBUS_EXPORT void cCpoDelete(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::objectName`
MODBUS_EXPORT const Char *cCpoGetObjectName(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::setObjectName`
MODBUS_EXPORT void cCpoSetObjectName(cModbusClientPort clientPort, const Char *name);

/// \details Wrapper for `ModbusClientPort::type`
MODBUS_EXPORT ProtocolType cCpoGetType(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::isOpen`
MODBUS_EXPORT bool cCpoIsOpen(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::close`
MODBUS_EXPORT bool cCpoClose(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::repeatCount`
MODBUS_EXPORT uint32_t cCpoGetRepeatCount(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::repeatCount`
MODBUS_EXPORT void cCpoSetRepeatCount(cModbusClientPort clientPort, uint32_t count);

/// \details Wrapper for `ModbusClientPort::readCoils`
MODBUS_EXPORT StatusCode cCpoReadCoils(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, void *values);

/// \details Wrapper for `ModbusClientPort::readDiscreteInputs`
MODBUS_EXPORT StatusCode cCpoReadDiscreteInputs(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, void *values);

/// \details Wrapper for `ModbusClientPort::readHoldingRegisters`
MODBUS_EXPORT StatusCode cCpoReadHoldingRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Wrapper for `ModbusClientPort::readInputRegisters`
MODBUS_EXPORT StatusCode cCpoReadInputRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Wrapper for `ModbusClientPort::writeSingleCoil`
MODBUS_EXPORT StatusCode cCpoWriteSingleCoil(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, bool value);

/// \details Wrapper for `ModbusClientPort::writeSingleRegister`
MODBUS_EXPORT StatusCode cCpoWriteSingleRegister(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t value);

/// \details Wrapper for `ModbusClientPort::readExceptionStatus`
MODBUS_EXPORT StatusCode cCpoReadExceptionStatus(cModbusClientPort clientPort, uint8_t unit, uint8_t *value);

/// \details Wrapper for `ModbusClientPort::writeMultipleCoils`
MODBUS_EXPORT StatusCode cCpoWriteMultipleCoils(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const void *values);

/// \details Wrapper for `ModbusClientPort::writeMultipleRegisters`
MODBUS_EXPORT StatusCode cCpoWriteMultipleRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values);

/// \details Wrapper for `ModbusClientPort::readCoilsAsBoolArray`
MODBUS_EXPORT StatusCode cCpoReadCoilsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, bool *values);

/// \details Wrapper for `ModbusClientPort::readDiscreteInputsAsBoolArray`
MODBUS_EXPORT StatusCode cCpoReadDiscreteInputsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, bool *values);

/// \details Wrapper for `ModbusClientPort::writeMultipleCoilsAsBoolArray`
MODBUS_EXPORT StatusCode cCpoWriteMultipleCoilsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const bool *values);

/// \details Wrapper for `ModbusClientPort::getLastStatus`
MODBUS_EXPORT StatusCode cCpoGetLastStatus(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::getLastErrorStatus`
MODBUS_EXPORT StatusCode cCpoGetLastErrorStatus(cModbusClientPort clientPort);

/// \details Wrapper for `ModbusClientPort::getLastErrorText`
MODBUS_EXPORT const Char *cCpoGetLastErrorText(cModbusClientPort clientPort);

/// \details Connects `funcPtr`-function to `ModbusClientPort::signalOpened` signal
MODBUS_EXPORT void cCpoConnectOpened(cModbusClientPort clientPort, pfSlotOpened funcPtr);

/// \details Connects `funcPtr`-function to `ModbusClientPort::signalClosed` signal
MODBUS_EXPORT void cCpoConnectClosed(cModbusClientPort clientPort, pfSlotClosed funcPtr);

/// \details Connects `funcPtr`-function to `ModbusClientPort::signalTx` signal
MODBUS_EXPORT void cCpoConnectTx(cModbusClientPort clientPort, pfSlotTx funcPtr);

/// \details Connects `funcPtr`-function to `ModbusClientPort::signalRx` signal
MODBUS_EXPORT void cCpoConnectRx(cModbusClientPort clientPort, pfSlotRx funcPtr);

/// \details Connects `funcPtr`-function to `ModbusClientPort::signalError` signal
MODBUS_EXPORT void cCpoConnectError(cModbusClientPort clientPort, pfSlotError funcPtr);

/// \details Disconnects `funcPtr`-function from `ModbusClientPort`
MODBUS_EXPORT void cCpoDisconnectFunc(cModbusClientPort clientPort, void *funcPtr);


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- ModbusClient ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \details Creates `ModbusClient` object and returns handle to it. \sa `Modbus::createClient`
MODBUS_EXPORT cModbusClient cCliCreate(uint8_t unit, ProtocolType type, const void *settings, bool blocking);

/// \details Creates `ModbusClient` object with `unit` for port `clientPort` and returns handle to it.
MODBUS_EXPORT cModbusClient cCliCreateForClientPort(uint8_t unit, cModbusClientPort clientPort);

/// \details Deletes previously created `ModbusClient` object represented by `client` handle
MODBUS_EXPORT void cCliDelete(cModbusClient client);

/// \details Wrapper for `ModbusClient::objectName`
MODBUS_EXPORT const Char *cCliGetObjectName(cModbusClient client);

/// \details Wrapper for `ModbusClient::setObjectName`
MODBUS_EXPORT void cCliSetObjectName(cModbusClient client, const Char *name);

/// \details Wrapper for `ModbusClient::type`
MODBUS_EXPORT ProtocolType cCliGetType(cModbusClient client);

/// \details Wrapper for `ModbusClient::unit`
MODBUS_EXPORT uint8_t cCliGetUnit(cModbusClient client);

/// \details Wrapper for `ModbusClient::setUnit`
MODBUS_EXPORT void cCliSetUnit(cModbusClient client, uint8_t unit);

/// \details Wrapper for `ModbusClient::isOpen`
MODBUS_EXPORT bool cCliIsOpen(cModbusClient client);

/// \details Wrapper for `ModbusClient::port`
MODBUS_EXPORT cModbusClientPort cCliGetPort(cModbusClient client);

/// \details Wrapper for `ModbusClient::readCoils`
MODBUS_EXPORT StatusCode cReadCoils(cModbusClient client, uint16_t offset, uint16_t count, void *values);

/// \details Wrapper for `ModbusClient::readDiscreteInputs`
MODBUS_EXPORT StatusCode cReadDiscreteInputs(cModbusClient client, uint16_t offset, uint16_t count, void *values);

/// \details Wrapper for `ModbusClient::readHoldingRegisters`
MODBUS_EXPORT StatusCode cReadHoldingRegisters(cModbusClient client, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Wrapper for `ModbusClient::readInputRegisters`
MODBUS_EXPORT StatusCode cReadInputRegisters(cModbusClient client, uint16_t offset, uint16_t count, uint16_t *values);

/// \details Wrapper for `ModbusClient::writeSingleCoil`
MODBUS_EXPORT StatusCode cWriteSingleCoil(cModbusClient client, uint16_t offset, bool value);

/// \details Wrapper for `ModbusClient::writeSingleRegister`
MODBUS_EXPORT StatusCode cWriteSingleRegister(cModbusClient client, uint16_t offset, uint16_t value);

/// \details Wrapper for `ModbusClient::readExceptionStatus`
MODBUS_EXPORT StatusCode cReadExceptionStatus(cModbusClient client, uint8_t *value);

/// \details Wrapper for `ModbusClient::writeMultipleCoils`
MODBUS_EXPORT StatusCode cWriteMultipleCoils(cModbusClient client, uint16_t offset, uint16_t count, const void *values);

/// \details Wrapper for `ModbusClient::writeMultipleRegisters`
MODBUS_EXPORT StatusCode cWriteMultipleRegisters(cModbusClient client, uint16_t offset, uint16_t count, const uint16_t *values);

/// \details Wrapper for `ModbusClient::readCoilsAsBoolArray`
MODBUS_EXPORT StatusCode cReadCoilsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, bool *values);

/// \details Wrapper for `ModbusClient::readDiscreteInputsAsBoolArray`
MODBUS_EXPORT StatusCode cReadDiscreteInputsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, bool *values);

/// \details Wrapper for `ModbusClient::lastPortStatus`
MODBUS_EXPORT StatusCode cWriteMultipleCoilsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, const bool *values);

/// \details Wrapper for `ModbusClient::lastPortStatus`
MODBUS_EXPORT StatusCode cCliGetLastPortStatus(cModbusClient client);

/// \details Wrapper for `ModbusClient::lastPortErrorStatus`
MODBUS_EXPORT StatusCode cCliGetLastPortErrorStatus(cModbusClient client);

/// \details Wrapper for `ModbusClient::lastPortErrorText`
MODBUS_EXPORT const Char *cCliGetLastPortErrorText(cModbusClient client);


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- ModbusServerPort -------------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \details Creates `ModbusServerPort` object and returns handle to it. \sa `Modbus::createServerPort`
MODBUS_EXPORT cModbusServerPort cSpoCreate(cModbusInterface device, ProtocolType type, const void *settings, bool blocking);

/// \details Deletes previously created `ModbusServerPort` object represented by `serverPort` handle
MODBUS_EXPORT void cSpoDelete(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::objectName`
MODBUS_EXPORT const Char *cSpoGetObjectName(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::setObjectName`
MODBUS_EXPORT void cSpoSetObjectName(cModbusServerPort serverPort, const Char *name);

/// \details Wrapper for `ModbusServerPort::type`
MODBUS_EXPORT ProtocolType cSpoGetType(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::isTcpServer`
MODBUS_EXPORT bool cSpoIsTcpServer(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::device`
MODBUS_EXPORT cModbusInterface cSpoGetDevice(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::isOpen`
MODBUS_EXPORT bool cSpoIsOpen(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::open`
MODBUS_EXPORT StatusCode cSpoOpen(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::close`
MODBUS_EXPORT StatusCode cSpoClose(cModbusServerPort serverPort);

/// \details Wrapper for `ModbusServerPort::process`
MODBUS_EXPORT StatusCode cSpoProcess(cModbusServerPort serverPort);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalOpened` signal
MODBUS_EXPORT void cSpoConnectOpened(cModbusServerPort serverPort, pfSlotOpened funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalClosed` signal
MODBUS_EXPORT void cSpoConnectClosed(cModbusServerPort serverPort, pfSlotClosed funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalTx` signal
MODBUS_EXPORT void cSpoConnectTx(cModbusServerPort serverPort, pfSlotTx funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalRx` signal
MODBUS_EXPORT void cSpoConnectRx(cModbusServerPort serverPort, pfSlotRx funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalError` signal
MODBUS_EXPORT void cSpoConnectError(cModbusServerPort serverPort, pfSlotError funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalNewConnection` signal
MODBUS_EXPORT void cSpoConnectNewConnection(cModbusServerPort serverPort, pfSlotNewConnection funcPtr);

/// \details Connects `funcPtr`-function to `ModbusServerPort::signalCloseConnection` signal
MODBUS_EXPORT void cSpoConnectCloseConnection(cModbusServerPort serverPort, pfSlotCloseConnection funcPtr);

/// \details Disconnects `funcPtr`-function from `ModbusServerPort`
MODBUS_EXPORT void cSpoDisconnectFunc(cModbusServerPort serverPort, void *funcPtr);


#ifdef __cplusplus
} // extern "C"
#endif

#endif // CMODBUS_H
