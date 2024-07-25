#include "ModbusClientPort.h"
#include "ModbusClientPort_p.h"

#include "ModbusPort.h"

ModbusClientPort::ModbusClientPort(ModbusPort *port) :
    ModbusObject(new ModbusClientPortPrivate(port))
{
}

ProtocolType ModbusClientPort::type() const
{
    return d_ModbusClientPort(d_ptr)->port->type();
}

StatusCode ModbusClientPort::close()
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    StatusCode s = d->port->close();
    signalClosed(this->objectName());
    d->currentClient = nullptr;
    d->setPortStatus(s);
    return s;
}

bool ModbusClientPort::isOpen() const
{
    return d_ModbusClientPort(d_ptr)->port->isOpen();
}

uint32_t ModbusClientPort::repeatCount() const
{
    return d_ModbusClientPort(d_ptr)->settings.repeatCount;
}

void ModbusClientPort::setRepeatCount(uint32_t v)
{
    if (v > 0)
        d_ModbusClientPort(d_ptr)->settings.repeatCount = v;
}

Modbus::StatusCode ModbusClientPort::readCoils(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff,  fcBytes;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_DISCRETS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::readCoils(offset=%hu, count=%hu): Requested count of coils is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1];    // Start coil offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0];    // Start coil offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];     // Quantity of coils - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];     // Quantity of coils - LS BYTE
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_READ_COILS,                 // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (!szOutBuff)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcBytes = buff[0];  // count of bytes received
        if (fcBytes != szOutBuff - 1)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        if (fcBytes != ((count + 7) / 8))
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        memcpy(values, &buff[1], fcBytes);
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::readDiscreteInputs(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, fcBytes;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_DISCRETS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::readDiscreteInputs(offset=%hu, count=%hu): Requested count of inputs is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1];    // Start input offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0];    // Start input offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];     // Quantity of inputs - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];     // Quantity of inputs - LS BYTE
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_READ_DISCRETE_INPUTS,       // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (!szOutBuff)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcBytes = buff[0];  // count of bytes received
        if (fcBytes != szOutBuff - 1)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        if (fcBytes != ((count + 7) / 8))
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        memcpy(values, &buff[1], fcBytes);
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}


Modbus::StatusCode ModbusClientPort::readHoldingRegisters(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, fcRegs, fcBytes, i;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_REGISTERS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::readHoldingRegisters(offset=%hu, count=%hu): Requested count of registers is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1]; // Start register offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0]; // Start register offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];  // Quantity of values - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];  // Quantity of values - LS BYTE
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_READ_HOLDING_REGISTERS,     // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (!szOutBuff)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcBytes = buff[0];  // count of bytes received
        if (fcBytes != szOutBuff - 1)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcRegs = fcBytes / sizeof(uint16_t); // count values received
        if (fcRegs != count)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        for (i = 0; i < fcRegs; i++)
            values[i] = (buff[i * 2 + 1] << 8) | buff[i * 2 + 2];
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::readInputRegisters(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, fcRegs, fcBytes, i;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_REGISTERS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::readInputRegisters(offset=%hu, count=%hu): Requested count of registers is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1]; // Start register offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0]; // Start register offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];  // Quantity of values - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];  // Quantity of values - LS BYTE
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_READ_INPUT_REGISTERS,       // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good) // processing
            return r;
        if (!szOutBuff)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcBytes = buff[0];  // count of bytes received
        if (fcBytes != szOutBuff - 1)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcRegs = fcBytes / sizeof(uint16_t); // count values received
        if (fcRegs != count)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        for (i = 0; i < fcRegs; i++)
            values[i] = (buff[i * 2 + 1] << 8) | buff[i * 2 + 2];
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::writeSingleCoil(ModbusObject *client, uint8_t unit, uint16_t offset, bool value)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 4;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, outOffset;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1];   // Coil offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0];   // Coil offset - LS BYTE
        buff[2] = (value ? 0xFF : 0x00);                    // Value - 0xFF if true, 0x00 if false
        buff[3] = 0x00;                                     // Value - must always be NULL
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_WRITE_SINGLE_COIL,          // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (szOutBuff != 4)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));

        outOffset = buff[1] | (buff[0] << 8);
        if (outOffset != offset)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::writeSingleRegister(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t value)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 4;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, outOffset, outValue;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1];    // Register offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0];    // Register offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&value)[1];     // Value - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&value)[0];     // Value - LS BYTE
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_WRITE_SINGLE_REGISTER,      // modbus function number
                          buff,                           // in-out buffer
                          4,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;

        if (szOutBuff != 4)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));

        outOffset = buff[1] | (buff[0] << 8);
        outValue = buff[3] | (buff[2] << 8);
        if (!((outOffset == offset) && (outValue == value)))
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

StatusCode ModbusClientPort::readExceptionStatus(ModbusObject *client, uint8_t unit, uint8_t *value)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 1;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff;

    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_READ_EXCEPTION_STATUS,      // modbus function number
                          buff,                           // in-out buffer
                          0,                              // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;

        if (szOutBuff != 1)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        *value = buff[0];
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::writeMultipleCoils(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, const void *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const int szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, outOffset, fcCoils, fcBytes;


    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_DISCRETS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::writeMultipleCoils(offset=%hu, count=%hu): Requested count of coils is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1]; // Start coil offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0]; // Start coil offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];  // Quantity of coils - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];  // Quantity of coils - LS BYTE
        fcBytes = (count + 7) / 8;
        buff[4] = static_cast<uint8_t>(fcBytes);      // Quantity of next bytes
        memcpy(&buff[5], values, fcBytes);
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_WRITE_MULTIPLE_COILS,       // modbus function number
                          buff,                           // in-out buffer
                          5 + buff[4],                    // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (szOutBuff != 4)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        outOffset = (buff[0] << 8) | buff[1];
        if (outOffset != offset)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcCoils = (buff[2] << 8) | buff[3];
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::writeMultipleRegisters(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    const uint16_t szBuff = 300;

    uint8_t buff[szBuff];
    Modbus::StatusCode r;
    uint16_t szOutBuff, i, outOffset, fcRegs;


    ModbusClientPort::RequestStatus status = this->getRequestStatus(client);
    switch (status)
    {
    case ModbusClientPort::Enable:
        if (count > MB_MAX_REGISTERS)
        {
            const size_t len = 200;
            Char buff[len];
            snprintf(buff, len, StringLiteral("ModbusClientPort::writeMultipleRegisters(offset=%hu, count=%hu): Requested count of registers is too large"), offset, count);
            d->lastErrorText = buff;
            this->cancelRequest(client);
            return d->setError(Status_BadNotCorrectRequest, StringLiteral("Not correct request"));
        }
        buff[0] = reinterpret_cast<uint8_t*>(&offset)[1];   // start register offset - MS BYTE
        buff[1] = reinterpret_cast<uint8_t*>(&offset)[0];   // start register offset - LS BYTE
        buff[2] = reinterpret_cast<uint8_t*>(&count)[1];    // quantity of registers - MS BYTE
        buff[3] = reinterpret_cast<uint8_t*>(&count)[0];    // quantity of registers - LS BYTE
        buff[4] = static_cast<uint8_t>(count * 2);          // quantity of next bytes

        for (i = 0; i < count; i++)
        {
            buff[5 + i * 2] = reinterpret_cast<const uint8_t*>(&values[i])[1];
            buff[6 + i * 2] = reinterpret_cast<const uint8_t*>(&values[i])[0];
        }
        // no need break
    case ModbusClientPort::Process:
        r = this->request(unit,             // unit ID
                          MBF_WRITE_MULTIPLE_REGISTERS,   // modbus function number
                          buff,                           // in-out buffer
                          5 + buff[4],                    // count of input data bytes
                          szBuff,                         // maximum size of buffer
                          &szOutBuff);                    // count of output data bytes
        if (r != Status_Good)
            return r;
        if (szOutBuff != 4)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        outOffset = (buff[0] << 8) | buff[1];
        if (outOffset != offset)
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response"));
        fcRegs = (buff[2] << 8) | buff[3];
        return d->setGoodStatus();
    default:
        return Status_Processing;
    }
}

Modbus::StatusCode ModbusClientPort::readCoilsAsBoolArray(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, bool *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    Modbus::StatusCode r = readCoils(client, unit, offset, count, d->buff);
    if (r != Status_Good)
        return r;
    for (int i = 0; i < count; ++i)
        values[i] = (d->buff[i / 8] & static_cast<uint8_t>(1 << (i % 8))) != 0;
    return Status_Good;
}

Modbus::StatusCode ModbusClientPort::readDiscreteInputsAsBoolArray(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, bool *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    Modbus::StatusCode r = readDiscreteInputs(client, unit, offset, count, d->buff);
    if (r != Status_Good)
        return r;
    for (int i = 0; i < count; ++i)
        values[i] = (d->buff[i / 8] & static_cast<uint8_t>(1 << (i % 8))) != 0;
    return Status_Good;
}

Modbus::StatusCode ModbusClientPort::writeMultipleCoilsAsBoolArray(ModbusObject *client, uint8_t unit, uint16_t offset, uint16_t count, const bool *values)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);

    if (this->currentClient() == nullptr)
    {
        for (int i = 0; i < count; i++)
        {
            if (!(i & 7))
                d->buff[i / 8] = 0;
            if (values[i] != 0)
                d->buff[i / 8] |= (1 << (i % 8));
        }
        return writeMultipleCoils(client, unit, offset, count, d->buff);
    }
    else if (this->currentClient() == client)
        return writeMultipleCoils(client, unit, offset, count, d->buff);
    return Status_Processing;
}

StatusCode ModbusClientPort::readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    return readCoils(this, unit, offset, count, values);
}

StatusCode ModbusClientPort::readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values)
{
    return readDiscreteInputs(this, unit, offset, count, values);
}

StatusCode ModbusClientPort::readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    return readHoldingRegisters(this, unit, offset, count, values);
}

StatusCode ModbusClientPort::readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values)
{
    return readInputRegisters(this, unit, offset, count, values);
}

StatusCode ModbusClientPort::writeSingleCoil(uint8_t unit, uint16_t offset, bool value)
{
    return writeSingleCoil(this, unit, offset, value);
}

StatusCode ModbusClientPort::writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value)
{
    return writeSingleRegister(this, unit, offset, value);
}

StatusCode ModbusClientPort::readExceptionStatus(uint8_t unit, uint8_t *value)
{
    return readExceptionStatus(this, unit, value);
}

StatusCode ModbusClientPort::writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values)
{
    return writeMultipleCoils(this, unit, offset, count, values);
}

StatusCode ModbusClientPort::writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values)
{
    return writeMultipleRegisters(this, unit, offset, count, values);
}

ModbusPort *ModbusClientPort::port() const
{
    return d_ModbusClientPort(d_ptr)->port;
}

StatusCode ModbusClientPort::lastStatus() const
{
    return d_ModbusClientPort(d_ptr)->lastStatus;
}

Modbus::StatusCode ModbusClientPort::lastErrorStatus() const
{
    return d_ModbusClientPort(d_ptr)->port->lastErrorStatus();
}

const Char *ModbusClientPort::lastErrorText() const
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    if (d->isLastPortError)
        return d->port->lastErrorText();
    else
        return d->lastErrorText.data();
}

const ModbusObject *ModbusClientPort::currentClient() const
{
    return d_ModbusClientPort(d_ptr)->currentClient;
}

ModbusClientPort::RequestStatus ModbusClientPort::getRequestStatus(ModbusObject *client)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    if (d->currentClient)
    {
        if (d->currentClient == client)
            return Process;
        return Disable;
    }
    else
    {
        d->currentClient = client;
        return Enable;
    }
}

void ModbusClientPort::cancelRequest(ModbusObject *client)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    if (d->currentClient == client)
        d->currentClient = nullptr;
}

void ModbusClientPort::signalOpened(const Modbus::Char *source)
{
    emitSignal(__func__, &ModbusClientPort::signalOpened, source);
}

void ModbusClientPort::signalClosed(const Modbus::Char *source)
{
    emitSignal(__func__, &ModbusClientPort::signalClosed, source);
}

void ModbusClientPort::signalTx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    emitSignal(__func__, &ModbusClientPort::signalTx, source, buff, size);
}

void ModbusClientPort::signalRx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    emitSignal(__func__, &ModbusClientPort::signalRx, source, buff, size);
}

void ModbusClientPort::signalError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text)
{
    emitSignal(__func__, &ModbusClientPort::signalError, source, status, text);
}

StatusCode ModbusClientPort::request(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff, uint16_t maxSzBuff, uint16_t *szOutBuff)
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    if (!d->isWriteBufferBlocked())
    {
        d->unit = unit;
        d->func = func;
        d->port->writeBuffer(unit, func, buff, szInBuff);
        d->blockWriteBuffer();
    }
    StatusCode r = process();
    if (r == Status_Processing)
        return r;
    if (StatusIsBad(r))
    {
        d->repeats++;
        if (d->repeats < d->settings.repeatCount)
        {
            d->port->setNextRequestRepeated(true);
            return Status_Processing;
        }
    }
    d->freeWriteBuffer();
    d->repeats = 0;
    d->currentClient = nullptr;
    if (StatusIsBad(r))
        return r;
    r = d->port->readBuffer(unit, func, buff, maxSzBuff, szOutBuff);

    if (unit != d->unit)
        return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response. Requested unit (unit) is not equal to responsed"));

    if ((func & MBF_EXCEPTION) == MBF_EXCEPTION)
    {
        if (*szOutBuff > 0)
        {
            StatusCode r = static_cast<StatusCode>(d->buff[0]); // Returned modbus exception
            return d->setError(static_cast<StatusCode>(Status_Bad | r), String(StringLiteral("Returned Modbus-exception with code "))+toModbusString(static_cast<int>(r)));
        }
        else
            return d->setError(Status_BadNotCorrectResponse, StringLiteral("Exception status missed"));
    }

    if (func != d->func)
        return d->setError(Status_BadNotCorrectResponse, StringLiteral("Not correct response. Requested function is not equal to responsed"));
    return d->setPortStatus(r);
}

StatusCode ModbusClientPort::process()
{
    ModbusClientPortPrivate *d = d_ModbusClientPort(d_ptr);
    StatusCode r;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (d->state)
        {
        case STATE_BEGIN:
        case STATE_CLOSED:
        case STATE_WAIT_FOR_OPEN:
            r = d->port->open();
            if (StatusIsProcessing(r))
                return r;
            d->setPortStatus(r);
            if (StatusIsBad(r)) // an error occured
            {
                signalError(d->getName(), r, d->port->lastErrorText());
                return r;
            }
            d->state = STATE_OPENED;
            signalOpened(this->objectName());
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = close();
            if (StatusIsProcessing(r))
                return r;
            if (StatusIsBad(r))
            {
                signalError(d->getName(), r, d->port->lastErrorText());
                return r;
            }
            d->state = STATE_CLOSED;
            //fRepeatAgain = true;
            break;
        case STATE_BEGIN_WRITE:
            if (!d->port->isOpen())
            {
                d->state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            d->state = STATE_OPENED;
            // no need break
        case STATE_OPENED:
            // send data to server
            if (d->port->isChanged())
            {
                d->state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            d->state = STATE_WRITE;
            // no need break
        case STATE_WRITE:
            r = d->port->write();
            if (StatusIsProcessing(r))
                return r;
            d->setPortStatus(r);
            if (StatusIsBad(r)) // an error occured
            {
                signalError(d->getName(), r, d->port->lastErrorText());
                d->state = STATE_BEGIN_WRITE;
                return r;
            }
            else
                signalTx(d->getName(), d->port->writeBufferData(), d->port->writeBufferSize());
            d->state = STATE_BEGIN_READ;
            // no need break
        case STATE_BEGIN_READ:
        case STATE_READ:
            r = d->port->read();
            if (StatusIsProcessing(r))
                return r;
            d->setPortStatus(r);
            if (StatusIsBad(r))
                signalError(d->getName(), r, d->port->lastErrorText());
            else
            {
                if (!d->port->isOpen())
                {
                    d->state = STATE_CLOSED;
                    return Status_Uncertain;
                }
                signalRx(d->getName(), d->port->readBufferData(), d->port->readBufferSize());
            }
            d->state = STATE_BEGIN_WRITE;
            return r;
        default:
            if (d->port->isOpen())
                d->state = STATE_OPENED;
            else
                d->state = STATE_CLOSED;
            fRepeatAgain = true;
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

