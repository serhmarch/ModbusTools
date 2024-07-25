#include "cModbus.h"

#include "Modbus.h"
#include "ModbusPort.h"
#include "ModbusClientPort.h"
#include "ModbusClient.h"
#include "ModbusServerPort.h"
#include "ModbusTcpServer.h"

class cModbusInterfaceImpl : public ModbusInterface
{
public:
    cModbusInterfaceImpl(cModbusDevice            device                , 
                         pfReadCoils              readCoils             ,
                         pfReadDiscreteInputs     readDiscreteInputs    ,
                         pfReadHoldingRegisters   readHoldingRegisters  ,
                         pfReadInputRegisters     readInputRegisters    ,
                         pfWriteSingleCoil        writeSingleCoil       ,
                         pfWriteSingleRegister    writeSingleRegister   ,
                         pfReadExceptionStatus    readExceptionStatus   ,
                         pfWriteMultipleCoils     writeMultipleCoils    ,
                         pfWriteMultipleRegisters writeMultipleRegisters) :
        m_device                (device                ),
        m_readCoils             (readCoils             ),
        m_readDiscreteInputs    (readDiscreteInputs    ),
        m_readHoldingRegisters  (readHoldingRegisters  ),
        m_readInputRegisters    (readInputRegisters    ),
        m_writeSingleCoil       (writeSingleCoil       ),
        m_writeSingleRegister   (writeSingleRegister   ),
        m_readExceptionStatus   (readExceptionStatus   ),
        m_writeMultipleCoils    (writeMultipleCoils    ),
        m_writeMultipleRegisters(writeMultipleRegisters)
    {
    }

    virtual ~cModbusInterfaceImpl()
    {
    }

public:
    StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values)
    {
        if (m_readCoils)
            return m_readCoils(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

    StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values)
    {
        if (m_readDiscreteInputs)
            return m_readDiscreteInputs(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

    StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
    {
        if (m_readHoldingRegisters)
            return m_readHoldingRegisters(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

    StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
    {
        if (m_readInputRegisters)
            return m_readInputRegisters(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

    StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value)
    {
        if (m_writeSingleCoil)
            return m_writeSingleCoil(m_device, unit, offset, value);
        return Status_BadIllegalFunction;
    }

    StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value)
    {
        if (m_writeSingleRegister)
            return m_writeSingleRegister(m_device, unit, offset, value);
        return Status_BadIllegalFunction;
    }

    StatusCode readExceptionStatus(uint8_t unit, uint8_t *status)
    {
        if (m_readExceptionStatus)
            return m_readExceptionStatus(m_device, unit, status);
        return Status_BadIllegalFunction;
    }

    StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values)
    {
        if (m_writeMultipleCoils)
            return m_writeMultipleCoils(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

    StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
    {
        if (m_writeMultipleRegisters)
            return m_writeMultipleRegisters(m_device, unit, offset, count, values);
        return Status_BadIllegalFunction;
    }

private:
    cModbusDevice            m_device                ;
    pfReadCoils              m_readCoils             ;
    pfReadDiscreteInputs     m_readDiscreteInputs    ;
    pfReadHoldingRegisters   m_readHoldingRegisters  ;
    pfReadInputRegisters     m_readInputRegisters    ;
    pfWriteSingleCoil        m_writeSingleCoil       ;
    pfWriteSingleRegister    m_writeSingleRegister   ;
    pfReadExceptionStatus    m_readExceptionStatus   ;
    pfWriteMultipleCoils     m_writeMultipleCoils    ;
    pfWriteMultipleRegisters m_writeMultipleRegisters;
};

cModbusInterface cCreateModbusDevice(cModbusDevice device, pfReadCoils readCoils, pfReadDiscreteInputs readDiscreteInputs, pfReadHoldingRegisters readHoldingRegisters, pfReadInputRegisters readInputRegisters, pfWriteSingleCoil writeSingleCoil, pfWriteSingleRegister writeSingleRegister, pfReadExceptionStatus readExceptionStatus, pfWriteMultipleCoils writeMultipleCoils, pfWriteMultipleRegisters writeMultipleRegisters)
{
    return new cModbusInterfaceImpl(device                ,
                                    readCoils             ,
                                    readDiscreteInputs    ,
                                    readHoldingRegisters  ,
                                    readInputRegisters    ,
                                    writeSingleCoil       ,
                                    writeSingleRegister   ,
                                    readExceptionStatus   ,
                                    writeMultipleCoils    ,
                                    writeMultipleRegisters);
}

void cDeleteModbusDevice(cModbusInterface dev)
{
    delete static_cast<cModbusInterfaceImpl*>(dev);
}

// --------------------------------------------------------------------------------------------------------
// ---------------------------------------------- ModbusPort ----------------------------------------------
// --------------------------------------------------------------------------------------------------------

cModbusPort cPortCreate(ProtocolType type, const void *settings, bool blocking)
{
    return createPort(type, settings, blocking);
}

void cPortDelete(cModbusPort port)
{
    delete port;
}

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- ModbusClientPort -------------------------------------------
// --------------------------------------------------------------------------------------------------------

cModbusClientPort cCpoCreate(ProtocolType type, const void *settings, bool blocking)
{
    return createClientPort(type, settings, blocking);
}

cModbusClientPort cCpoCreateForPort(cModbusPort port)
{
    return new ModbusClientPort(port);
}

void cCpoDelete(cModbusClientPort clientPort)
{
    delete clientPort;
}

const Char *cCpoGetObjectName(cModbusClientPort clientPort)
{
    return clientPort->objectName();
}

void cCpoSetObjectName(cModbusClientPort clientPort, const Char *name)
{
    clientPort->setObjectName(name);
}

ProtocolType cCpoGetType(cModbusClientPort clientPort)
{
    return clientPort->type();
}

bool cCpoIsOpen(cModbusClientPort clientPort)
{
    return clientPort->isOpen();
}

bool cCpoClose(cModbusClientPort clientPort)
{
    return clientPort->close();
}

uint32_t cCpoGetRepeatCount(cModbusClientPort clientPort)
{
    return clientPort->close();
}

void cCpoSetRepeatCount(cModbusClientPort clientPort, uint32_t count)
{
    clientPort->setRepeatCount(count);
}

StatusCode cCpoReadCoils(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    return clientPort->readCoils(unit, offset, count, values);
}

StatusCode cCpoReadDiscreteInputs(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    return clientPort->readDiscreteInputs(unit, offset, count, values);
}

StatusCode cCpoReadHoldingRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    return clientPort->readHoldingRegisters(unit, offset, count, values);
}

StatusCode cCpoReadInputRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    return clientPort->readInputRegisters(unit, offset, count, values);
}

StatusCode cCpoWriteSingleCoil(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, bool value)
{
    return clientPort->writeSingleCoil(unit, offset, value);
}

StatusCode cCpoWriteSingleRegister(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t value)
{
    return clientPort->writeSingleRegister(unit, offset, value);
}

StatusCode cCpoReadExceptionStatus(cModbusClientPort clientPort, uint8_t unit, uint8_t *value)
{
    return clientPort->readExceptionStatus(unit, value);
}

StatusCode cCpoWriteMultipleCoils(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const void *values)
{
    return clientPort->writeMultipleCoils(unit, offset, count, values);
}

StatusCode cCpoWriteMultipleRegisters(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
{
    return clientPort->writeMultipleRegisters(unit, offset, count, values);
}

StatusCode cCpoReadCoilsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, bool *values)
{
    return clientPort->readCoilsAsBoolArray(unit, offset, count, values);
}

StatusCode cCpoReadDiscreteInputsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, bool *values)
{
    return clientPort->readDiscreteInputsAsBoolArray(unit, offset, count, values);
}

StatusCode cCpoWriteMultipleCoilsAsBoolArray(cModbusClientPort clientPort, uint8_t unit, uint16_t offset, uint16_t count, const bool *values)
{
    return clientPort->writeMultipleCoilsAsBoolArray(unit, offset, count, values);
}

StatusCode cCpoGetLastStatus(cModbusClientPort clientPort)
{
    return clientPort->lastStatus();
}

StatusCode cCpoGetLastErrorStatus(cModbusClientPort clientPort)
{
    return clientPort->lastErrorStatus();
}

const Char *cCpoGetLastErrorText(cModbusClientPort clientPort)
{
    return clientPort->lastErrorText();
}

void cCpoConnectOpened(cModbusClientPort clientPort, pfSlotOpened funcPtr)
{
    clientPort->connect(&ModbusClientPort::signalOpened, funcPtr);
}

void cCpoConnectClosed(cModbusClientPort clientPort, pfSlotClosed funcPtr)
{
    clientPort->connect(&ModbusClientPort::signalClosed, funcPtr);
}

void cCpoConnectTx(cModbusClientPort clientPort, pfSlotTx funcPtr)
{
    clientPort->connect(&ModbusClientPort::signalTx, funcPtr);
}

void cCpoConnectRx(cModbusClientPort clientPort, pfSlotRx funcPtr)
{
    clientPort->connect(&ModbusClientPort::signalRx, funcPtr);
}

void cCpoConnectError(cModbusClientPort clientPort, pfSlotError funcPtr)
{
    clientPort->connect(&ModbusClientPort::signalError, funcPtr);
}

void cCpoDisconnectFunc(cModbusClientPort clientPort, void *funcPtr)
{
    clientPort->disconnectFunc(funcPtr);
}


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- ModbusClient ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

cModbusClient cCliCreate(uint8_t unit, ProtocolType type, const void *settings, bool blocking)
{
    ModbusClientPort *clientPort = createClientPort(type, settings, blocking);
    ModbusClient *client = new ModbusClient(unit, clientPort);
    return client;
}

cModbusClient cCliCreateForClientPort(uint8_t unit, cModbusClientPort clientPort)
{
    ModbusClient *client = new ModbusClient(unit, clientPort);
    return client;
}

void cCliDelete(cModbusClient client)
{
    delete client;
}

const Char *cCliGetObjectName(cModbusClient client)
{
    return client->objectName();
}

void cCliSetObjectName(cModbusClient client, const Char *name)
{
    client->setObjectName(name);
}

ProtocolType cCliGetType(cModbusClient client)
{
    return client->type();
}

uint8_t cCliGetUnit(cModbusClient client)
{
    return client->unit();
}

void cCliSetUnit(cModbusClient client, uint8_t unit)
{
    client->setUnit(unit);
}

bool cCliIsOpen(cModbusClient client)
{
    return client->isOpen();
}

cModbusClientPort cCliGetPort(cModbusClient client)
{
    return client->port();
}

StatusCode cReadCoils(cModbusClient client, uint16_t offset, uint16_t count, void *values)
{
    return client->readCoils(offset, count, values);
}

StatusCode cReadDiscreteInputs(cModbusClient client, uint16_t offset, uint16_t count, void *values)
{
    return client->readDiscreteInputs(offset, count, values);
}

StatusCode cReadHoldingRegisters(cModbusClient client, uint16_t offset, uint16_t count, uint16_t *values)
{
    return client->readHoldingRegisters(offset, count, values);
}

StatusCode cReadInputRegisters(cModbusClient client, uint16_t offset, uint16_t count, uint16_t *values)
{
    return client->readInputRegisters(offset, count, values);
}

StatusCode cWriteSingleCoil(cModbusClient client, uint16_t offset, bool value)
{
    return client->writeSingleCoil(offset, value);
}

StatusCode cWriteSingleRegister(cModbusClient client, uint16_t offset, uint16_t value)
{
    return client->writeSingleRegister(offset, value);
}

StatusCode cReadExceptionStatus(cModbusClient client, uint8_t *value)
{
    return client->readExceptionStatus(value);
}

StatusCode cWriteMultipleCoils(cModbusClient client, uint16_t offset, uint16_t count, const void *values)
{
    return client->writeMultipleCoils(offset, count, values);
}

StatusCode cWriteMultipleRegisters(cModbusClient client, uint16_t offset, uint16_t count, const uint16_t *values)
{
    return client->writeMultipleRegisters(offset, count, values);
}

StatusCode cReadCoilsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, bool *values)
{
    return client->readCoilsAsBoolArray(offset, count, values);
}

StatusCode cReadDiscreteInputsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, bool *values)
{
    return client->readDiscreteInputsAsBoolArray(offset, count, values);
}

StatusCode cWriteMultipleCoilsAsBoolArray(cModbusClient client, uint16_t offset, uint16_t count, const bool *values)
{
    return client->writeMultipleCoilsAsBoolArray(offset, count, values);
}

StatusCode cCliGetLastPortStatus(cModbusClient client)
{
    return client->lastPortStatus();
}

StatusCode cCliGetLastPortErrorStatus(cModbusClient client)
{
    return client->lastPortErrorStatus();
}

const Char *cCliGetLastPortErrorText(cModbusClient client)
{
    return client->lastPortErrorText();
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- ModbusServerPort -------------------------------------------
// --------------------------------------------------------------------------------------------------------

cModbusServerPort cSpoCreate(cModbusInterface device, ProtocolType type, const void *settings, bool blocking)
{
    return createServerPort(device, type, settings, blocking);
}

void cSpoDelete(cModbusServerPort serverPort)
{
    delete serverPort;
}

const Char *cSpoGetObjectName(cModbusServerPort serverPort)
{
    return serverPort->objectName();
}

void cSpoSetObjectName(cModbusServerPort serverPort, const Char *name)
{
    serverPort->setObjectName(name);
}

ProtocolType cSpoGetType(cModbusServerPort serverPort)
{
    return serverPort->type();
}

bool cSpoIsTcpServer(cModbusServerPort serverPort)
{
    return serverPort->isTcpServer();
}

cModbusInterface cSpoGetDevice(cModbusServerPort serverPort)
{
    return serverPort->device();
}

bool cSpoIsOpen(cModbusServerPort serverPort)
{
    return serverPort->isOpen();
}

StatusCode cSpoOpen(cModbusServerPort serverPort)
{
    return serverPort->open();
}

StatusCode cSpoClose(cModbusServerPort serverPort)
{
    return serverPort->close();
}

StatusCode cSpoProcess(cModbusServerPort serverPort)
{
    return serverPort->process();
}

void cSpoConnectOpened(cModbusServerPort serverPort, pfSlotOpened funcPtr)
{
    serverPort->connect(&ModbusServerPort::signalOpened, funcPtr);
}

void cSpoConnectClosed(cModbusServerPort serverPort, pfSlotClosed funcPtr)
{
    serverPort->connect(&ModbusServerPort::signalClosed, funcPtr);
}

void cSpoConnectTx(cModbusServerPort serverPort, pfSlotTx funcPtr)
{
    serverPort->connect(&ModbusServerPort::signalTx, funcPtr);
}

void cSpoConnectRx(cModbusServerPort serverPort, pfSlotRx funcPtr)
{
    serverPort->connect(&ModbusServerPort::signalRx, funcPtr);
}

void cSpoConnectError(cModbusServerPort serverPort, pfSlotError funcPtr)
{
    serverPort->connect(&ModbusServerPort::signalError, funcPtr);
}

void cSpoConnectNewConnection(cModbusServerPort serverPort, pfSlotNewConnection funcPtr)
{
    if (serverPort->isTcpServer())
        static_cast<ModbusTcpServer*>(serverPort)->connect(&ModbusTcpServer::signalNewConnection, funcPtr);
}

void cSpoConnectCloseConnection(cModbusServerPort serverPort, pfSlotCloseConnection funcPtr)
{
    if (serverPort->isTcpServer())
        static_cast<ModbusTcpServer*>(serverPort)->connect(&ModbusTcpServer::signalCloseConnection, funcPtr);
}

void cSpoDisconnectFunc(cModbusServerPort serverPort, void *funcPtr)
{
    serverPort->disconnectFunc(funcPtr);
}
