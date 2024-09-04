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
    m_offset = item->offset();
    m_count = item->count();
    m_writeOffset = 0;
    m_writeCount = 0;
    m_period = item->period();
    m_maxCount = maxCount;
    m_status = Modbus::Status_Uncertain;
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
    m_offset = offset;
    m_count = count;
    m_writeOffset = 0;
    m_writeCount = 0;
    m_period = 0;
    m_maxCount = maxCount;
    m_status = Modbus::Status_Uncertain;
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

uint16_t mbClientRunMessage::offset() const { return m_offset; }

uint16_t mbClientRunMessage::count() const { return m_count; }

uint16_t mbClientRunMessage::maxCount() const { return m_maxCount; }

uint32_t mbClientRunMessage::period() const { return m_period; }

void *mbClientRunMessage::innerBuffer() { return m_buff; }

int mbClientRunMessage::innerBufferSize() const { return MB_MAX_BYTES; }

int mbClientRunMessage::innerBufferBitSize() const { return innerBufferSize() * MB_BYTE_SZ_BITES; }

int mbClientRunMessage::innerBufferRegSize() const { return innerBufferSize() / MB_REGE_SZ_BYTES; }

Modbus::StatusCode mbClientRunMessage::status() const { return m_status; }

mb::Timestamp_t mbClientRunMessage::timestamp() const { return m_timestamp; }

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

bool mbClientRunMessage::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void * /*buff*/) const
{
    return false;
}

bool mbClientRunMessage::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void * /*buff*/)
{
    return false;
}

void mbClientRunMessage::prepareToSend()
{
    // base implementation does nothing
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
    for (Items_t::ConstIterator it = m_items.cbegin(); it != m_items.cend(); ++it)
    {
        mbClientRunItem *pItem = static_cast<mbClientRunItem*>(*it);
        pItem->writeDataToMessage();
    }
}


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------------- READ_COILS ----------------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageReadCoils::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}

bool mbClientRunMessageReadCoils::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}


// --------------------------------------------------------------------------------------------------------
// -----------------------------------------  READ_DISCRETE_INPUTS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageReadDiscreteInputs::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}

bool mbClientRunMessageReadDiscreteInputs::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------- READ_HOLDING_REGISTERS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageReadHoldingRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}

bool mbClientRunMessageReadHoldingRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_INPUT_REGISTERS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageReadInputRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}

bool mbClientRunMessageReadInputRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- WRITE_SINGLE_COIL ------------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageWriteSingleCoil::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void *buff) const
{
    // ignoring offset and count
    reinterpret_cast<uint8_t*>(buff)[0] = m_buff[0];
    return true;
}

bool mbClientRunMessageWriteSingleCoil::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void *buff)
{
    // ignoring offset and count
    m_buff[0] = reinterpret_cast<const uint8_t*>(buff)[0];
    return true;
}


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_SINGLE_REGISTER ----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageWriteSingleRegister::getData(uint16_t /*innerOffset*/, uint16_t /*count*/, void *buff) const
{
    // ignoring offset and count
    reinterpret_cast<uint16_t*>(buff)[0] = reinterpret_cast<const uint16_t*>(m_buff)[0];
    return true;
}

bool mbClientRunMessageWriteSingleRegister::setData(uint16_t /*innerOffset*/, uint16_t /*count*/, const void *buff)
{
    // ignoring offset and count
    reinterpret_cast<uint16_t*>(m_buff)[0] = reinterpret_cast<const uint16_t*>(buff)[0];
    return true;
}

// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- READ_EXCEPTION_STATUS ----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageReadExceptionStatus::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}

// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- WRITE_MULTIPLE_COILS -----------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageWriteMultipleCoils::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}

bool mbClientRunMessageWriteMultipleCoils::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemBits(innerOffset, count, buff, m_buff, innerBufferBitSize()));
}


// --------------------------------------------------------------------------------------------------------
// --------------------------------------- WRITE_MULTIPLE_REGISTERS ---------------------------------------
// --------------------------------------------------------------------------------------------------------

bool mbClientRunMessageWriteMultipleRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}

bool mbClientRunMessageWriteMultipleRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}


// --------------------------------------------------------------------------------------------------------
// ------------------------------------------ MASK_WRITE_REGISTER -----------------------------------------
// --------------------------------------------------------------------------------------------------------

mbClientRunMessageMaskWriteRegister::mbClientRunMessageMaskWriteRegister(uint16_t offset, QObject *parent) : mbClientRunMessageWrite(offset, 2, 2, parent)
{
}

bool mbClientRunMessageMaskWriteRegister::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}

bool mbClientRunMessageMaskWriteRegister::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
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

bool mbClientRunMessageReadWriteMultipleRegisters::getData(uint16_t innerOffset, uint16_t count, void *buff) const
{
    return Modbus::StatusIsGood(Modbus::readMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}

bool mbClientRunMessageReadWriteMultipleRegisters::setData(uint16_t innerOffset, uint16_t count, const void *buff)
{
    return Modbus::StatusIsGood(Modbus::writeMemRegs(innerOffset, count, buff, m_buff, innerBufferRegSize()));
}
