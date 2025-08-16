/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef CLIENT_RUNMESSAGE_H
#define CLIENT_RUNMESSAGE_H

#include <QReadWriteLock>
#include <QObject>

#include <ModbusQt.h>

#include <mbcore.h>

class mbClientRunItem;

class mbClientRunMessage : public QObject
{
    Q_OBJECT
    MB_REF_COUNTING

public:
    mbClientRunMessage(mbClientRunItem *item, uint16_t maxCount, QObject *parent = nullptr);
    mbClientRunMessage(uint8_t unit, uint16_t offset, uint16_t count, uint16_t maxCount, QObject *parent = nullptr);
    mbClientRunMessage(uint16_t offset, uint16_t count, uint16_t maxCount, QObject *parent = nullptr) : mbClientRunMessage(0, offset, count, maxCount, parent){}
    virtual ~mbClientRunMessage();

public:
    virtual uint8_t function() const = 0;
    virtual Modbus::MemoryType memoryType() const = 0;
    inline uint8_t unit() const { return m_unit; }
    inline void setUnit(uint8_t unit) { m_unit = unit; }
    inline uint16_t offset() const { return m_offset; }
    inline uint16_t count() const { return m_count; }
    inline uint16_t writeOffset() const { return m_writeOffset; }
    inline uint16_t writeCount () const { return m_writeCount ; }
    inline uint16_t maxCount() const { return m_count; }
    inline uint32_t period() const { return m_period; }
    inline void *innerBuffer() { return m_buff; }
    inline uint16_t *innerBufferReg() { return reinterpret_cast<uint16_t*>(innerBuffer()); }
    inline int innerBufferSize() const { return MB_MAX_BYTES; }
    inline int innerBufferBitSize() const { return innerBufferSize() * MB_BYTE_SZ_BITES; }
    inline int innerBufferRegSize() const { return innerBufferSize() / MB_REGE_SZ_BYTES; }
    inline Modbus::StatusCode status() const { return m_status; }
    inline mb::Timestamp_t beginTimestamp() const { return m_beginTimestamp; }
    inline mb::Timestamp_t timestamp() const { return m_timestamp; }

public:
    bool addItem(mbClientRunItem *item);
    void setDeleteItems(bool del);

public:
    virtual Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const;
    virtual Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff);
    virtual void prepareToSend();
    virtual void setComplete(Modbus::StatusCode status, mb::Timestamp_t timestamp);
    bool isCompleted() const;
    void clearCompleted();

public:
    QByteArray bytesTx() const;
    void setBytesTx(const QByteArray &data);
    QByteArray bytesRx() const;
    void setBytesRx(const QByteArray &data);

    QByteArray asciiTx() const;
    void setAsciiTx(const QByteArray &data);
    QByteArray asciiRx() const;
    void setAsciiRx(const QByteArray &data);

Q_SIGNALS:
    void signalBytesTx(const QByteArray &bytes);
    void signalBytesRx(const QByteArray &bytes);
    void signalAsciiTx(const QByteArray &bytes);
    void signalAsciiRx(const QByteArray &bytes);
    void completed();

protected:
    void addItemPrivate(mbClientRunItem *item);

protected:
    mutable QReadWriteLock m_lock;

protected:
    typedef QList<mbClientRunItem*> Items_t;
    Items_t m_items;
    bool m_deleteItems;
    bool m_isCompleted;

protected:
    uint8_t m_unit;
    uint16_t m_offset;
    uint16_t m_count;
    uint16_t m_writeOffset;
    uint16_t m_writeCount;
    uint16_t m_andMask;
    uint16_t m_orMask;
    uint32_t m_period;
    uint16_t m_maxCount;
    Modbus::StatusCode m_status;
    mb::Timestamp_t m_beginTimestamp;
    mb::Timestamp_t m_timestamp;
    uint8_t m_buff[MB_MAX_BYTES];

protected:
    QByteArray m_dataTx;
    QByteArray m_dataRx;
};


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- Raw Messages ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageRaw : public mbClientRunMessage
{
public:
    mbClientRunMessageRaw(QObject *parent = nullptr) : mbClientRunMessage(0,0,0, parent) {}

public:
    uint8_t function() const { return 0; }
    Modbus::MemoryType memoryType() const { return Modbus::Memory_Unknown; }
    inline void *inputBuffer() { return innerBuffer(); }
    inline uint16_t inputBaseCount() const { return count(); }
    inline void setInputBaseCount(uint16_t c) { m_count = c; }
    inline uint16_t inputCount() const { return writeCount(); }
    inline void setInputCount(uint16_t c) { m_writeCount = c; }
    inline void *outputBuffer() { return m_outputBuff; }
    inline uint16_t outputCount() const { return m_outputCount; }
    inline void setOutputCount(uint16_t c) { m_outputCount = c; }
    inline uint16_t* outputCountPtr() { return &m_outputCount; }
    inline uint16_t outputMaxCount() const { return MB_MAX_BYTES; }

protected:
    uint8_t m_outputBuff[MB_MAX_BYTES];
    uint16_t m_outputCount;
};


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- Read Messages --------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageRead : public mbClientRunMessage
{
public:
    using mbClientRunMessage::mbClientRunMessage;

public:
    void setComplete(Modbus::StatusCode status, mb::Timestamp_t timestamp) override;
};


// --------------------------------------------------------------------------------------------------------
// -------------------------------------------- Write Messages --------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWrite : public mbClientRunMessage
{
public:
    using mbClientRunMessage::mbClientRunMessage;

public:
    void prepareToSend() override;
};


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------------- READ_COILS ----------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadCoils : public mbClientRunMessageRead
{
public:
    using mbClientRunMessageRead::mbClientRunMessageRead;

public:
    uint8_t function() const override { return MBF_READ_COILS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// -----------------------------------------  READ_DISCRETE_INPUTS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadDiscreteInputs : public mbClientRunMessageRead
{
public:
    using mbClientRunMessageRead::mbClientRunMessageRead;

public:
    uint8_t function() const override { return MBF_READ_DISCRETE_INPUTS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_1x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------- READ_HOLDING_REGISTERS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadHoldingRegisters : public mbClientRunMessageRead
{
public:
    using mbClientRunMessageRead::mbClientRunMessageRead;

public:
    uint8_t function() const override { return MBF_READ_HOLDING_REGISTERS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_INPUT_REGISTERS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadInputRegisters : public mbClientRunMessageRead
{
public:
    using mbClientRunMessageRead::mbClientRunMessageRead;

public:
    uint8_t function() const override { return MBF_READ_INPUT_REGISTERS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_3x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- WRITE_SINGLE_COIL ------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteSingleCoil : public mbClientRunMessageWrite
{
public:
    explicit mbClientRunMessageWriteSingleCoil(mbClientRunItem *item, QObject *parent = nullptr) : mbClientRunMessageWrite(item, 1, parent) {}
    explicit mbClientRunMessageWriteSingleCoil(uint8_t unit, uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWrite(unit, offset, 1, 1, parent) {}
    explicit mbClientRunMessageWriteSingleCoil(uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWriteSingleCoil(0, offset, parent) {}

public:
    uint8_t function() const override { return MBF_WRITE_SINGLE_COIL; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_SINGLE_REGISTER ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteSingleRegister : public mbClientRunMessageWrite
{
public:
    explicit mbClientRunMessageWriteSingleRegister(mbClientRunItem *item, QObject *parent = nullptr) : mbClientRunMessageWrite(item, 1, parent) {}
    explicit mbClientRunMessageWriteSingleRegister(uint8_t unit, uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWrite(unit, offset, 1, 1, parent) {}
    explicit mbClientRunMessageWriteSingleRegister(uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWriteSingleRegister(0, offset, parent) {}

public:
    uint8_t function() const override { return MBF_WRITE_SINGLE_REGISTER; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_EXCEPTION_STATUS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadExceptionStatus : public mbClientRunMessageRead
{
public:
    explicit mbClientRunMessageReadExceptionStatus(uint8_t unit, QObject *parent = nullptr) : mbClientRunMessageRead(unit, 0, 0, 0, parent) {}
    explicit mbClientRunMessageReadExceptionStatus(QObject *parent = nullptr) : mbClientRunMessageReadExceptionStatus(0, parent) {}

public:
    uint8_t function() const override { return MBF_READ_EXCEPTION_STATUS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
};


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- REPORT SERVER ID -------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReportServerID : public mbClientRunMessageRead
{
public:
    explicit mbClientRunMessageReportServerID(uint8_t unit, QObject *parent = nullptr) : mbClientRunMessageRead(unit, 0, 0, 0, parent) {}
    explicit mbClientRunMessageReportServerID(QObject *parent = nullptr) : mbClientRunMessageReportServerID(0, parent) {}

public:
    uint8_t function() const override { return MBF_REPORT_SERVER_ID; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_Unknown; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    inline void setCount(uint16_t count) { m_count = count; }
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_MULTIPLE_COILS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteMultipleCoils : public mbClientRunMessageWrite
{
public:
    using mbClientRunMessageWrite::mbClientRunMessageWrite;

public:
    uint8_t function() const override { return MBF_WRITE_MULTIPLE_COILS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// --------------------------------------- WRITE_MULTIPLE_REGISTERS ---------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteMultipleRegisters : public mbClientRunMessageWrite
{
public:
    using mbClientRunMessageWrite::mbClientRunMessageWrite;

public:
    uint8_t function() const override { return MBF_WRITE_MULTIPLE_REGISTERS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------ MASK_WRITE_REGISTER -----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageMaskWriteRegister : public mbClientRunMessageWrite
{
public:
    explicit mbClientRunMessageMaskWriteRegister(uint8_t unit, uint16_t offset, QObject *parent = nullptr);
    explicit mbClientRunMessageMaskWriteRegister(uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageMaskWriteRegister(0, offset, parent) {}

public:
    uint8_t function() const override { return MBF_MASK_WRITE_REGISTER; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};

// --------------------------------------------------------------------------------------------------------
// ------------------------------------- READ_WRITE_MULTIPLE_REGISTERS ------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadWriteMultipleRegisters : public mbClientRunMessage
{
public:
    mbClientRunMessageReadWriteMultipleRegisters(uint8_t unit, uint16_t readOffset, uint16_t readCount, uint16_t writeOffset, uint16_t writeCount, QObject *parent = nullptr);
    mbClientRunMessageReadWriteMultipleRegisters(uint16_t readOffset, uint16_t readCount, uint16_t writeOffset, uint16_t writeCount, QObject *parent = nullptr) :
        mbClientRunMessageReadWriteMultipleRegisters(0, readOffset, readCount, writeOffset, writeCount, parent) {}

public:
    uint8_t function() const override { return MBF_READ_WRITE_MULTIPLE_REGISTERS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    Modbus::StatusCode getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    Modbus::StatusCode setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};

#endif // CLIENT_RUNMESSAGE_H
