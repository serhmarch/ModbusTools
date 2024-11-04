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
#include "client_runmessage.h"

#include "client_runitem.h"

mbClientRunMessage::mbClientRunMessage(mbClientRunItem *item, uint16_t maxCount, QObject *parent)
    : QObject{parent}
{
    //m_refCount = 0; // Note: g++ initialize it incorrectly using default constructor somehow (detected for Ubuntu 22.04, 64 bit)
    m_maxCount = maxCount;
    m_offset = item->offset();
    m_count = item->count();
    if (m_count > m_maxCount)
        m_count = m_maxCount;
    m_writeOffset = 0;
    m_writeCount = 0;
    m_period = item->period();
    m_status = Modbus::Status_Uncertain;
    m_beginTimestamp = 0;
    m_timestamp = 0;
    addItemPrivate(item);
    m_deleteItems = false;
    m_isCompleted = false;
    memset(m_buff, 0, sizeof(m_buff));
}

mbClientRunMessage::mbClientRunMessage(uint16_t offset, uint16_t count, uint16_t maxCount, QObject *parent)
    : QObject{parent}
{
    //m_refCount = 0; // Note: g++ initialize it incorrectly using default constructor somehow (detected for Ubuntu 22.04, 64 bit)
    m_maxCount = maxCount;
    m_offset = offset;
    m_count = count;
    if (m_count > m_maxCount)
        m_count = m_maxCount;
    m_writeOffset = 0;
    m_writeCount = 0;
    m_period = 0;
    m_maxCount = maxCount;
    m_status = Modbus::Status_Uncertain;
    m_beginTimestamp = 0;
    m_timestamp = 0;
    m_deleteItems = false;
    m_isCompleted = false;
    memset(m_buff, 0, sizeof(m_buff));
}

mbClientRunMessage::~mbClientRunMessage()
{
    // TODO: make items with ref count
    //       and setMessage(nullptr) foreach in destructor
    if (m_deleteItems)
        qDeleteAll(m_items);
}

uint16_t mbClientRunMessage::offset() const
{
    return m_offset;
}

uint16_t mbClientRunMessage::count() const
{
    return m_count;
}

uint16_t mbClientRunMessage::maxCount() const
{
    return m_maxCount;
}

uint32_t mbClientRunMessage::period() const
{
    return m_period;
}

void *mbClientRunMessage::innerBuffer()
{
    return m_buff;
}

int mbClientRunMessage::innerBufferSize() const
{
    return MB_MAX_BYTES;
}

int mbClientRunMessage::innerBufferBitSize() const
{
    return innerBufferSize() * MB_BYTE_SZ_BITES;
}

int mbClientRunMessage::innerBufferRegSize() const
{
    return innerBufferSize() / MB_REGE_SZ_BYTES;
}

Modbus::StatusCode mbClientRunMessage::status() const
{
    return m_status;
}

mb::Timestamp_t mbClientRunMessage::beginTimestamp() const
{
    return m_beginTimestamp;
}

mb::Timestamp_t mbClientRunMessage::timestamp() const
{
    return m_timestamp;
}

bool mbClientRunMessage::addItem(mbClientRunItem *item)
{
    if (item->period() != period())
    {
        // Unsuccessful (period don't match)
        return false;
    }
    if (item->memoryType() != memoryType())
    {
        // Unsuccessful (memoryType don't match)
        return false;
    }
    uint16_t itemOffset, itemCount, nextItemOffset;
    itemOffset = item->offset();
    itemCount = item->count();
    nextItemOffset = itemOffset + itemCount;
    if ((itemOffset >= m_offset) && (nextItemOffset <= (m_offset+m_maxCount))) // expand message to end
    {
        if (nextItemOffset >= (m_offset+m_count))
            m_count = nextItemOffset - m_offset;
    }
    else if ((itemOffset < m_offset) && ((itemOffset+m_maxCount) >= (m_offset+m_count))) // expand message from begin
    {
        m_count += (m_offset - itemOffset);
        m_offset = itemOffset;
    }
    else
    {
        //Unsuccessful. ModbusItem is out of message data range
        return false;
    }
    addItemPrivate(item);
    return true;
}

void mbClientRunMessage::setDeleteItems(bool del)
{
    m_deleteItems = del;
}

Modbus::StatusCode mbClientRunMessage::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void * /*buff*/) const
{
    return Modbus::Status_Bad;
}

Modbus::StatusCode mbClientRunMessage::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void * /*buff*/)
{
    return Modbus::Status_Bad;
}

void mbClientRunMessage::prepareToSend()
{
    m_beginTimestamp = mb::currentTimestamp();
}

void mbClientRunMessage::setComplete(Modbus::StatusCode status, mb::Timestamp_t timestamp)
{
    m_status = status;
    m_timestamp = timestamp;
    m_isCompleted = true;
    Q_EMIT completed();
}

bool mbClientRunMessage::isCompleted() const
{
    QReadLocker _(&m_lock);
    return m_isCompleted;
}

void mbClientRunMessage::clearCompleted()
{
    QWriteLocker _(&m_lock);
    m_isCompleted = false;
}

QByteArray mbClientRunMessage::bytesTx() const
{
    QReadLocker _(&m_lock);
    return m_dataTx;
}

void mbClientRunMessage::setBytesTx(const QByteArray &data)
{
    QWriteLocker _(&m_lock);
    m_dataTx = data;
    Q_EMIT signalBytesTx(data);
}

QByteArray mbClientRunMessage::bytesRx() const
{
    QReadLocker _(&m_lock);
    return m_dataRx;
}

void mbClientRunMessage::setBytesRx(const QByteArray &data)
{
    QWriteLocker _(&m_lock);
    m_dataRx = data;
    Q_EMIT signalBytesRx(data);
}

QByteArray mbClientRunMessage::asciiTx() const
{
    QReadLocker _(&m_lock);
    return m_dataTx;
}

void mbClientRunMessage::setAsciiTx(const QByteArray &data)
{
    QWriteLocker _(&m_lock);
    m_dataTx = data;
    Q_EMIT signalAsciiTx(data);
}

QByteArray mbClientRunMessage::asciiRx() const
{
    QReadLocker _(&m_lock);
    return m_dataRx;
}

void mbClientRunMessage::setAsciiRx(const QByteArray &data)
{
    QWriteLocker _(&m_lock);
    m_dataRx = data;
    Q_EMIT signalAsciiRx(data);
}

void mbClientRunMessage::addItemPrivate(mbClientRunItem *item)
{
    m_items.append(item);
    item->setMessage(this);
}


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- Read Message ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

void mbClientRunMessageRead::setComplete(Modbus::StatusCode status, mb::Timestamp_t timestamp)
{
    m_status = status;
    m_timestamp = timestamp;
    for (Items_t::ConstIterator it = m_items.cbegin(); it != m_items.cend(); ++it)
    {
        mbClientRunItem *pItem = static_cast<mbClientRunItem*>(*it);
        pItem->readDataFromMessage();
    }
    m_isCompleted = true;
    Q_EMIT completed();
}


// --------------------------------------------------------------------------------------------------------
// --------------------------------------------- Write Message --------------------------------------------
// --------------------------------------------------------------------------------------------------------

void mbClientRunMessageWrite::prepareToSend()
{
    mbClientRunMessage::prepareToSend();
    for (Items_t::ConstIterator it = m_items.cbegin(); it != m_items.cend(); ++it)
    {
        mbClientRunItem *pItem = static_cast<mbClientRunItem*>(*it);
        pItem->writeDataToMessage();
    }
}


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------------- READ_COILS ----------------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageReadCoils::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}

Modbus::StatusCode mbClientRunMessageReadCoils::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// -----------------------------------------  READ_DISCRETE_INPUTS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageReadDiscreteInputs::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}

Modbus::StatusCode mbClientRunMessageReadDiscreteInputs::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------- READ_HOLDING_REGISTERS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageReadHoldingRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}

Modbus::StatusCode mbClientRunMessageReadHoldingRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_INPUT_REGISTERS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageReadInputRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}

Modbus::StatusCode mbClientRunMessageReadInputRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- WRITE_SINGLE_COIL ------------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageWriteSingleCoil::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void *buff) const
{
    // ignoring offset and count
    reinterpret_cast<uint8_t*>(buff)[0] = m_buff[0];
    return Modbus::Status_Good;
}

Modbus::StatusCode mbClientRunMessageWriteSingleCoil::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void *buff)
{
    // ignoring offset and count
    m_buff[0] = reinterpret_cast<const uint8_t*>(buff)[0];
    return Modbus::Status_Good;
}


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_SINGLE_REGISTER ----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageWriteSingleRegister::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void *buff) const
{
    // ignoring offset and count
    reinterpret_cast<uint16_t*>(buff)[0] = reinterpret_cast<const uint16_t*>(m_buff)[0];
    return Modbus::Status_Good;
}

Modbus::StatusCode mbClientRunMessageWriteSingleRegister::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void *buff)
{
    // ignoring offset and count
    reinterpret_cast<uint16_t*>(m_buff)[0] = reinterpret_cast<const uint16_t*>(buff)[0];
    return Modbus::Status_Good;
}

// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_EXCEPTION_STATUS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageReadExceptionStatus::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize());
}

// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_MULTIPLE_COILS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageWriteMultipleCoils::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}

Modbus::StatusCode mbClientRunMessageWriteMultipleCoils::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// --------------------------------------- WRITE_MULTIPLE_REGISTERS ---------------------------------------
// --------------------------------------------------------------------------------------------------------

Modbus::StatusCode mbClientRunMessageWriteMultipleRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}

Modbus::StatusCode mbClientRunMessageWriteMultipleRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------ MASK_WRITE_REGISTER -----------------------------------------
// --------------------------------------------------------------------------------------------------------

mbClientRunMessageMaskWriteRegister::mbClientRunMessageMaskWriteRegister(uint16_t offset, QObject *parent) : mbClientRunMessageWrite(offset, 2, 2, parent)
{
}

Modbus::StatusCode mbClientRunMessageMaskWriteRegister::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}

Modbus::StatusCode mbClientRunMessageMaskWriteRegister::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------- READ_WRITE_MULTIPLE_REGISTERS ------------------------------------
// --------------------------------------------------------------------------------------------------------

mbClientRunMessageReadWriteMultipleRegisters::mbClientRunMessageReadWriteMultipleRegisters(uint16_t readOffset,
                                                                                           uint16_t readCount,
                                                                                           uint16_t writeOffset,
                                                                                           uint16_t writeCount,
                                                                                           QObject *parent) :
    mbClientRunMessage(readOffset, readCount, MB_MAX_REGISTERS, parent)
{
    m_writeOffset = writeOffset;
    m_writeCount = writeCount;
}

Modbus::StatusCode mbClientRunMessageReadWriteMultipleRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    uint32_t c;
    return Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}

Modbus::StatusCode mbClientRunMessageReadWriteMultipleRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    uint32_t c;
    return Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize(), &c);
}
