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
    explicit mbClientRunMessage(mbClientRunItem *item, uint16_t maxCount, QObject *parent = nullptr);
    explicit mbClientRunMessage(uint16_t offset, uint16_t count, uint16_t maxCount, QObject *parent = nullptr);
    virtual ~mbClientRunMessage();

public:
    virtual uint8_t function() const = 0;
    virtual Modbus::MemoryType memoryType() const = 0;
    uint16_t offset() const;
    uint16_t count() const;
    uint16_t maxCount() const;
    uint32_t period() const;
    void *innerBuffer();
    int innerBufferSize() const;
    int innerBufferBitSize() const;
    int innerBufferRegSize() const;
    Modbus::StatusCode status() const;
    mb::Timestamp_t timestamp()  const;

public:
    bool addItem(mbClientRunItem *item);
    void setDeleteItems(bool del);

public:
    virtual bool getData(uint16_t innerOffset, uint16_t count, void *buff) const;
    virtual bool setData(uint16_t innerOffset, uint16_t count, const void *buff);
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
    uint16_t m_offset;
    uint16_t m_count;
    uint32_t m_period;
    uint16_t m_maxCount;
    Modbus::StatusCode m_status;
    mb::Timestamp_t m_timestamp;
    uint8_t m_buff[MB_MAX_BYTES];

protected:
    QByteArray m_dataTx;
    QByteArray m_dataRx;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- WRITE_SINGLE_COIL ------------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteSingleCoil : public mbClientRunMessageWrite
{
public:
    explicit mbClientRunMessageWriteSingleCoil(mbClientRunItem *item, QObject *parent = nullptr) : mbClientRunMessageWrite(item, 1, parent) {}
    explicit mbClientRunMessageWriteSingleCoil(uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWrite(offset, 1, 1, parent) {}

public:
    uint8_t function() const override { return MBF_WRITE_SINGLE_COIL; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_SINGLE_REGISTER ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageWriteSingleRegister : public mbClientRunMessageWrite
{
public:
    explicit mbClientRunMessageWriteSingleRegister(mbClientRunItem *item, QObject *parent = nullptr) : mbClientRunMessageWrite(item, 1, parent) {}
    explicit mbClientRunMessageWriteSingleRegister(uint16_t offset, QObject *parent = nullptr) : mbClientRunMessageWrite(offset, 1, 1, parent) {}

public:
    uint8_t function() const override { return MBF_WRITE_SINGLE_REGISTER; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_4x; }
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_EXCEPTION_STATUS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

class mbClientRunMessageReadExceptionStatus : public mbClientRunMessageRead
{
public:
    explicit mbClientRunMessageReadExceptionStatus(QObject *parent = nullptr) : mbClientRunMessageRead(0, 0, 0, parent) {}

public:
    uint8_t function() const override { return MBF_READ_EXCEPTION_STATUS; }
    Modbus::MemoryType memoryType() const override { return Modbus::Memory_0x; }
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
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
    bool getData(uint16_t innerOffset, uint16_t count, void *buff) const override;
    bool setData(uint16_t innerOffset, uint16_t count, const void *buff) override;
};

#endif // CLIENT_RUNMESSAGE_H
