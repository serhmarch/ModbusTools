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
#include "server_device.h"

/* TODO:
 -  check unit numbers of device
    when add it to project in setProject-method or
    when setUnits-method with project incharge
*/

#include <QSet>

mbServerDevice::Strings::Strings() :
    count0x                  (QStringLiteral("count0x")),
    count1x                  (QStringLiteral("count1x")),
    count3x                  (QStringLiteral("count3x")),
    count4x                  (QStringLiteral("count4x")),
    isSaveData               (QStringLiteral("isSaveData")),
    isReadOnly               (QStringLiteral("isReadOnly")),
    exceptionStatusAddress   (QStringLiteral("exceptionStatusAddress")),
    delay                    (QStringLiteral("delay"))
{
}

const mbServerDevice::Strings &mbServerDevice::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDevice::Defaults::Defaults() :
    maxCount0x(MB_MEMORY_MAX_COUNT),
    maxCount1x(MB_MEMORY_MAX_COUNT),
    maxCount3x(MB_MEMORY_MAX_COUNT),
    maxCount4x(MB_MEMORY_MAX_COUNT),
    count0x(65535),
    count1x(65535),
    count3x(65535),
    count4x(65535),
    isSaveData(true),
    isReadOnly(false),
    exceptionStatusAddress(1),
    delay(0)
{
}

const mbServerDevice::Defaults &mbServerDevice::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbServerDevice::MemoryBlock::MemoryBlock()
{
    m_sizeBits = 0;
    m_changeCounter = 0;
}

mbServerDevice::MemoryBlock::~MemoryBlock()
{
    m_data.detach();
}

void mbServerDevice::MemoryBlock::shmCreate(const QString name, int size)
{
    m_data.setKey(name);
    m_data.create(size);
    m_data.attach();
}

void mbServerDevice::MemoryBlock::resize(int bytes)
{
    //Locker _(&m_data);
    //m_data.resize(bytes);
    //memset(m_data.data(), 0, m_tmpDataSize);
    m_tmpDataSize = bytes < MB_MEMORY_SIZEOF ? bytes : MB_MEMORY_SIZEOF;
    m_sizeBits = m_tmpDataSize * MB_BYTE_SZ_BITES;
}

void mbServerDevice::MemoryBlock::resizeBits(int bits)
{
    //Locker _(&m_data);
    //m_data.resize((bits+7)/8);
    //memset(m_data.data(), 0, m_tmpDataSize);
    const int maxbits = MB_MEMORY_SIZEOF * MB_BYTE_SZ_BITES;
    if (bits > maxbits)
        bits = maxbits;
    m_sizeBits = bits;
    m_tmpDataSize = (bits+7)/8;
}

void mbServerDevice::MemoryBlock::zerroAll()
{
    Locker _(&m_data);
    m_changeCounter++;
    memset(m_data.data(), 0, m_tmpDataSize);
}

Modbus::StatusCode mbServerDevice::MemoryBlock::read(uint offset, uint count, void *buff, uint *fact) const
{
    Locker _(&m_data);
    uint c;
    if (offset >= static_cast<uint>(static_cast<uint>(m_tmpDataSize)))
        return Modbus::Status_BadIllegalDataAddress;

    if ((offset+count) > static_cast<uint>(m_tmpDataSize))
        c = static_cast<uint>(static_cast<uint>(m_tmpDataSize)) - offset;
    else
        c = count;
    memcpy(buff, reinterpret_cast<uint8_t*>(m_data.data())+offset, c);
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::write(uint offset, uint count, const void *buff, uint *fact)
{
    Locker _(&m_data);
    uint c;
    if (offset >= static_cast<uint>(m_tmpDataSize))
        return Modbus::Status_BadIllegalDataAddress;

    if ((offset+count) > static_cast<uint>(m_tmpDataSize))
        c = static_cast<uint>(m_tmpDataSize) - offset;
    else
        c = count;
    if (c == 0)
        return Modbus::Status_BadIllegalDataAddress;
    memcpy(reinterpret_cast<uint8_t*>(m_data.data())+offset, buff, c);
    m_changeCounter++;
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::readBits(uint bitOffset, uint bitCount, void *buff, uint *fact) const
{
    Locker _(&m_data);
    uint c;
    if (bitOffset >= m_sizeBits)
        return Modbus::Status_BadIllegalDataAddress;

    if ((bitOffset+bitCount) > m_sizeBits)
        c = m_sizeBits - bitOffset;
    else
        c = bitCount;

    uint byteOffset = bitOffset/MB_BYTE_SZ_BITES;
    uint bytes = c/MB_BYTE_SZ_BITES;
    uint shift = bitOffset%MB_BYTE_SZ_BITES;
    const quint8 *mem = reinterpret_cast<const quint8*>(m_data.data());
    if (shift)
    {
        for (uint i = 0; i < bytes; i++)
        {
            quint16 v = *(reinterpret_cast<const quint16*>(&mem[byteOffset+i])) >> shift; // no need to check (i+1) < bytes because if (shift > 0) then target bits are located in both nearest bytes (i) and (i+1)
            reinterpret_cast<quint8*>(buff)[i] = static_cast<quint8>(v);
        }
        if (quint16 resid = c%MB_BYTE_SZ_BITES)
        {
            qint8 mask = static_cast<qint8>(0x80);
            mask = ~(mask>>(7-resid));
            if ((shift+resid) > MB_BYTE_SZ_BITES)
            {
                quint16 v = ((*reinterpret_cast<const quint16*>(&mem[byteOffset+bytes])) >> shift) & mask;
                reinterpret_cast<quint8*>(buff)[bytes] = static_cast<quint8>(v);
            }
            else
                reinterpret_cast<quint8*>(buff)[bytes] = (mem[byteOffset+bytes]>>shift) & mask;
        }
    }
    else
    {
        memcpy(buff, &mem[byteOffset], static_cast<size_t>(bytes));
        if (quint16 resid = c%MB_BYTE_SZ_BITES)
        {
            qint8 mask = static_cast<qint8>(0x80);
            mask = ~(mask>>(7-resid));
            reinterpret_cast<quint8*>(buff)[bytes] = mem[byteOffset+bytes] & mask;
        }
    }
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::writeBits(uint bitOffset, uint bitCount, const void *buff, uint *fact)
{
    Locker _(&m_data);
    uint c;
    if (bitOffset >= m_sizeBits)
        return Modbus::Status_BadIllegalDataAddress;

    if ((bitOffset+bitCount) > m_sizeBits)
        c = m_sizeBits - bitOffset;
    else
        c = bitCount;
    if (c == 0)
        return Modbus::Status_BadIllegalDataAddress;
    uint byteOffset = bitOffset/MB_BYTE_SZ_BITES;
    uint bytes = c/MB_BYTE_SZ_BITES;
    uint shift = bitOffset%MB_BYTE_SZ_BITES;
    quint8 *mem = reinterpret_cast<quint8*>(m_data.data());
    if (shift)
    {
        for (uint i = 0; i < bytes; i++)
        {
            quint16 mask = static_cast<quint16>(0x00FF) << shift;
            quint16 v = static_cast<quint16>(reinterpret_cast<const quint8*>(buff)[i]) << shift; // no need to check (i+1) < bytes because if (shift > 0) then target bits are located in both nearest bytes (i) and (i+1)
            *reinterpret_cast<quint16*>(&mem[byteOffset+i]) &= ~mask; // zero undermask buff
            *reinterpret_cast<quint16*>(&mem[byteOffset+i]) |= v; // set bit values
        }
        if (quint16 resid = c%MB_BYTE_SZ_BITES)
        {
            if ((shift+resid) > MB_BYTE_SZ_BITES)
            {
                qint16 m = static_cast<qint16>(0x8000); // using signed mask for right shift filled by '1'-bit
                m = m>>(resid-1);
                quint16 mask = *reinterpret_cast<quint16*>(&m);
                mask = mask >> (MB_REGE_SZ_BITES-resid-shift);
                quint16 v = (static_cast<quint16>(reinterpret_cast<const quint8*>(buff)[bytes]) << shift) & mask;
                *reinterpret_cast<quint16*>(&mem[byteOffset+bytes]) &= ~mask; // zero undermask buff
                *reinterpret_cast<quint16*>(&mem[byteOffset+bytes]) |= v;
            }
            else
            {
                qint8 m = static_cast<qint8>(0x80); // using signed mask for right shift filled by '1'-bit
                m = m>>(resid-1);
                quint8 mask = *reinterpret_cast<quint8*>(&m);
                mask = mask >> (MB_BYTE_SZ_BITES-resid-shift);
                quint8 v = (reinterpret_cast<const quint8*>(buff)[bytes] << shift) & mask;
                mem[byteOffset+bytes] &= ~mask; // zero undermask buff
                mem[byteOffset+bytes] |= v;
            }
        }
    }
    else
    {
        memcpy(&mem[byteOffset], buff, static_cast<size_t>(bytes));
        if (quint16 resid = c%MB_BYTE_SZ_BITES)
        {
            qint8 mask = static_cast<qint8>(0x80);
            mask = mask>>(7-resid);
            mem[byteOffset+bytes] &= mask;
            mask = ~mask;
            mem[byteOffset+bytes] |= (reinterpret_cast<const quint8*>(buff)[bytes] & mask);
        }
    }
    m_changeCounter++;
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::readBools(uint bitOffset, uint bitCount, bool *values, uint *fact) const
{
    Locker _(&m_data);
    uint c;
    if (bitOffset >= m_sizeBits)
        return Modbus::Status_BadIllegalDataAddress;

    if ((bitOffset+bitCount) > m_sizeBits)
        c = m_sizeBits - bitOffset;
    else
        c = bitCount;
    uint byte = bitOffset / MB_BYTE_SZ_BITES;
    uint bit  = bitOffset % MB_BYTE_SZ_BITES;
    const quint8 *mem = reinterpret_cast<const quint8*>(m_data.data());
    for (uint by = byte, i = 0; i < c; by++)
    {
        for (uint bi = bit; bi < MB_BYTE_SZ_BITES && i < c; bi++, i++)
            values[i] = (mem[by] & (1<<bi)) != 0;
        bit = 0;
    }
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::writeBools(uint bitOffset, uint bitCount, const bool *values, uint *fact)
{
    Locker _(&m_data);
    uint c;
    if (bitOffset >= m_sizeBits)
        return Modbus::Status_BadIllegalDataAddress;

    if ((bitOffset+bitCount) > m_sizeBits)
        c = m_sizeBits - bitOffset;
    else
        c = bitCount;
    uint byte = bitOffset / MB_BYTE_SZ_BITES;
    uint bit  = bitOffset % MB_BYTE_SZ_BITES;
    quint8 *mem = reinterpret_cast<quint8*>(m_data.data());
    for (uint by = byte, i = 0; i < c; by++)
    {
        for (uint bi = bit; bi < MB_BYTE_SZ_BITES && i < c; bi++, i++)
        {
            if (values[i])
                mem[by] |= (1<<bi);
            else
                mem[by] &= ~(1<<bi);
        }
        bit = 0;
    }
    m_changeCounter++;
    if (fact)
        *fact = c;
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::readRegs(uint regOffset, uint regCount, quint16 *buff, uint *fact) const
{
    uint offset = regOffset * MB_REGE_SZ_BYTES;
    uint count = regCount * MB_REGE_SZ_BYTES;
    Modbus::StatusCode r = read(offset, count, buff, fact);
    if (Modbus::StatusIsGood(r))
    {
        if (fact)
            *fact /= MB_REGE_SZ_BYTES;
    }
    return r;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::writeRegs(uint regOffset, uint regCount, const quint16 *buff, uint *fact)
{
    uint offset = regOffset * MB_REGE_SZ_BYTES;
    uint count = regCount * MB_REGE_SZ_BYTES;
    Modbus::StatusCode r = write(offset, count, buff, fact);
    if (Modbus::StatusIsGood(r))
    {
        if (fact)
            *fact /= MB_REGE_SZ_BYTES;
    }
    return r;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::readFrameBools(uint bitOffset, int columns, QByteArray &values, uint maxColumns) const
{
    // TODO: make it single operation without 'readBools' call
    bool *v = reinterpret_cast<bool*>(values.data());
    int c = values.count();
    int offset = bitOffset;
    int i = 0;
    // read memory frame line by line
    while (c > 0)
    {
        if (Modbus::StatusCode r = readBools(static_cast<quint16>(offset), columns % (c+1), v+i))
            return r;
        c -= columns;
        offset += maxColumns;
        i += columns;
    }
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::writeFrameBools(uint bitOffset, int columns, const QByteArray &values, int maxColumns)
{
    // TODO: make it single operation without 'writeBools' call
    const bool *v = reinterpret_cast<const bool*>(values.constData());
    int c = values.count();
    int offset = bitOffset;
    int i = 0;
    // read memory frame line by line
    while (c > 0)
    {
        if (Modbus::StatusCode r = writeBools(static_cast<quint16>(offset), columns % (c+1), v+i))
            return r;
        c -= columns;
        offset += maxColumns;
        i += columns;
    }
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::readFrameRegs(uint regOffset, int columns, QByteArray &values, int maxColumns) const
{
    // TODO: make it single operation without 'readRegs' call
    quint16 *v = reinterpret_cast<quint16*>(values.data());
    int c = values.count()/static_cast<int>(sizeof(quint16));
    int offset = regOffset;
    int i = 0;
    // read memory frame line by line
    while (c > 0)
    {
        if (Modbus::StatusCode r = readRegs(static_cast<quint16>(offset), columns % (c+1), v+i))
            return r;
        c -= columns;
        offset += maxColumns;
        i += columns;
    }
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::MemoryBlock::writeFrameRegs(uint regOffset, int columns, const QByteArray &values, int maxColumns)
{
    // TODO: make it single operation without 'writeRegs' call
    const quint16 *v = reinterpret_cast<const quint16*>(values.constData());
    int c = values.count()/static_cast<int>(sizeof(quint16));
    int offset = regOffset;
    int i = 0;
    // read memory frame line by line
    while (c > 0)
    {
        if (Modbus::StatusCode r = writeRegs(offset, columns % (c+1), v+i))
            return r;
        c -= columns;
        offset += maxColumns;
        i += columns;
    }
    return Modbus::Status_Good;
}

mbServerDevice::mbServerDevice(QObject * /*parent*/)
{
    const QString prefix = QString("ModbusTools.Server.PORT1.PLC1.");
    const QString sMem0x = prefix+QStringLiteral("mem0x");
    const QString sMem1x = prefix+QStringLiteral("mem1x");
    const QString sMem3x = prefix+QStringLiteral("mem3x");
    const QString sMem4x = prefix+QStringLiteral("mem4x");
    m_mem_0x.shmCreate(sMem0x, MB_MEMORY_MAX_COUNT/8);
    m_mem_1x.shmCreate(sMem1x, MB_MEMORY_MAX_COUNT/8);
    m_mem_3x.shmCreate(sMem3x, MB_MEMORY_MAX_COUNT*2);
    m_mem_4x.shmCreate(sMem4x, MB_MEMORY_MAX_COUNT*2);

    Defaults d = Defaults::instance();
    m_project = nullptr;
    setName(d.name);

    this->realloc_0x(d.count0x);
    this->realloc_1x(d.count1x);
    this->realloc_3x(d.count3x);
    this->realloc_4x(d.count4x);
    setExceptionStatusAddressInt(d.exceptionStatusAddress);
    setReadOnly(d.isReadOnly);
    m_settings.isSaveData = d.isSaveData;
    m_settings.delay = d.delay;

}

quint8 mbServerDevice::exceptionStatus() const
{
    switch (m_settings.exceptionStatusAddress.type)
    {
    case Modbus::Memory_0x: return uint8_0x(m_settings.exceptionStatusAddress.offset);
    case Modbus::Memory_1x: return uint8_1x(m_settings.exceptionStatusAddress.offset);
    case Modbus::Memory_3x: return uint8_3x(m_settings.exceptionStatusAddress.offset*MB_REGE_SZ_BYTES);
    case Modbus::Memory_4x: return uint8_4x(m_settings.exceptionStatusAddress.offset*MB_REGE_SZ_BYTES);
    }
    return 0;
}

Modbus::Settings mbServerDevice::settings() const
{
    Strings s = Strings();

    Modbus::Settings r = mbCoreDevice::settings();

    r.insert(s.count0x                  , count_0x                  ());
    r.insert(s.count1x                  , count_1x                  ());
    r.insert(s.count3x                  , count_3x                  ());
    r.insert(s.count4x                  , count_4x                  ());
    r.insert(s.isSaveData               , isSaveData                ());
    r.insert(s.isReadOnly               , isReadOnly                ());
    r.insert(s.exceptionStatusAddress   , exceptionStatusAddressInt ());
    r.insert(s.delay                    , delay                     ());

    return r;
}

bool mbServerDevice::setSettings(const Modbus::Settings &settings)
{
    QWriteLocker _(&m_lock);

    Strings s = Strings();

    Modbus::Settings::const_iterator it;
    Modbus::Settings::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.count0x);
    if (it != end)
    {
        QVariant var = it.value();
        int v = static_cast<int>(var.toInt(&ok));
        if (ok)
            realloc_0x(v);
    }

    it = settings.find(s.count1x);
    if (it != end)
    {
        QVariant var = it.value();
        int v = static_cast<int>(var.toInt(&ok));
        if (ok)
            realloc_1x(v);
    }

    it = settings.find(s.count3x);
    if (it != end)
    {
        QVariant var = it.value();
        int v = static_cast<int>(var.toInt(&ok));
        if (ok)
            realloc_3x(v);
    }

    it = settings.find(s.count4x);
    if (it != end)
    {
        QVariant var = it.value();
        int v = static_cast<int>(var.toInt(&ok));
        if (ok)
            realloc_4x(v);
    }

    it = settings.find(s.isSaveData);
    if (it != end)
    {
        QVariant var = it.value();
        setSaveData(var.toBool());
    }

    it = settings.find(s.isReadOnly);
    if (it != end)
    {
        QVariant var = it.value();
        setReadOnly(var.toBool());
    }

    it = settings.find(s.exceptionStatusAddress);
    if (it != end)
    {
        QVariant var = it.value();
        int v = var.toInt(&ok);
        if (ok)
            setExceptionStatusAddress(v);
    }

    it = settings.find(s.delay);
    if (it != end)
    {
        QVariant var = it.value();
        uint v = var.toUInt(&ok);
        if (ok)
            setDelay(v);
    }

    mbCoreDevice::setSettings(settings);
    return true;
}

QByteArray mbServerDevice::readData(const mb::Address &address, quint16 count)
{
    QByteArray v;
    switch (address.type)
    {
    case Modbus::Memory_0x:
    {
        v.resize((count+7)/8);
        read_0x(address.offset, count, v.data());
    }
        break;
    case Modbus::Memory_1x:
    {
        v.resize((count+7)/8);
        read_1x(address.offset, count, v.data());
    }
        break;
    case Modbus::Memory_3x:
    {
        v.resize(count*2);
        read_3x(address.offset, count, reinterpret_cast<quint16*>(v.data()));
    }
        break;
    case Modbus::Memory_4x:
    {
        v.resize(count*2);
        read_4x(address.offset, count, reinterpret_cast<quint16*>(v.data()));
    }
        break;
    default:
        break;
    }
    return v;
}

void mbServerDevice::writeData(const mb::Address &address, quint16 count, const QByteArray &data)
{
    switch (address.type)
    {
    case Modbus::Memory_0x:
        write_0x(address.offset, count, data.constData());
        break;
    case Modbus::Memory_1x:
        write_1x(address.offset, count, data.constData());
        break;
    case Modbus::Memory_3x:
        write_3x(address.offset, count, reinterpret_cast<const quint16*>(data.constData()));
        break;
    case Modbus::Memory_4x:
        write_4x(address.offset, count, reinterpret_cast<const quint16*>(data.constData()));
        break;
    default:
        break;
    }
}

Modbus::StatusCode mbServerDevice::readCoils(uint16_t offset, uint16_t count, void *values)
{
    QReadLocker _(&m_lock);
    if (count > maxReadCoils())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_0x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->read_0x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::readDiscreteInputs(uint16_t offset, uint16_t count, void *values)
{
    QReadLocker _(&m_lock);
    if (count > maxReadDiscreteInputs())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_1x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->read_1x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::readHoldingRegisters(uint16_t offset, uint16_t count, uint16_t *values)
{
    QReadLocker _(&m_lock);
    if (count > maxReadHoldingRegisters())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->read_4x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::readInputRegisters(uint16_t offset, uint16_t count, uint16_t *values)
{
    QReadLocker _(&m_lock);
    if (count > maxReadInputRegisters())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_3x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->read_3x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::writeSingleCoil(uint16_t offset, bool value)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (offset >= this->count_0x())
        return Modbus::Status_BadIllegalDataAddress;
    this->setBool_0x(offset, value);
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::writeSingleRegister(uint16_t offset, uint16_t value)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (offset >= this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    this->setUInt16_4x(offset, value);
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::readExceptionStatus(uint8_t *status)
{
    QReadLocker _(&m_lock);
    *status = this->exceptionStatus();
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::writeMultipleCoils(uint16_t offset, uint16_t count, const void *values)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (count > maxWriteMultipleCoils())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_0x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->write_0x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::writeMultipleRegisters(uint16_t offset, uint16_t count, const uint16_t *values)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (count > maxWriteMultipleRegisters())
        return Modbus::Status_BadIllegalDataAddress;
    if ((offset+count) > this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    return this->write_4x(offset, count, values);
}

Modbus::StatusCode mbServerDevice::maskWriteRegister(uint16_t offset, uint16_t andMask, uint16_t orMask)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (offset > this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    uint16_t c = this->uint16_4x(offset);
    uint16_t r = (c & andMask) | (orMask & ~andMask);
    this->setUInt16_4x(offset, r);
    return Modbus::Status_Good;
}

Modbus::StatusCode mbServerDevice::readWriteMultipleRegisters(uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues)
{
    QWriteLocker _(&m_lock);
    if (isReadOnly())
        return Modbus::Status_BadIllegalFunction;
    if (writeCount > maxWriteMultipleRegisters())
        return Modbus::Status_BadIllegalDataAddress;
    if ((writeOffset+writeCount) > this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    if (readCount > maxWriteMultipleRegisters())
        return Modbus::Status_BadIllegalDataAddress;
    if ((readOffset+readCount) > this->count_4x())
        return Modbus::Status_BadIllegalDataAddress;
    Modbus::StatusCode s = this->write_4x(writeOffset, writeCount, writeValues);
    if (!Modbus::StatusIsGood(s))
        return s;
    return this->read_4x(readOffset, readCount, readValues);
}

void mbServerDevice::realloc_0x(int count)
{
    if (count_0x() != count)
    {
        m_mem_0x.resizeBits(count);
        Q_EMIT count_0x_changed(count);
    }
}

void mbServerDevice::realloc_1x(int count)
{
    if (count_1x() != count)
    {
        m_mem_1x.resizeBits(count);
        Q_EMIT count_1x_changed(count);
    }
}

void mbServerDevice::realloc_3x(int count)
{
    if (count_3x() != count)
    {
        m_mem_3x.resizeRegs(count);
        Q_EMIT count_3x_changed(count);
    }
}

void mbServerDevice::realloc_4x(int count)
{
    if (count_4x() != count)
    {
        m_mem_4x.resizeRegs(count);
        Q_EMIT count_4x_changed(count);
    }
}

QVariant mbServerDevice::value(mb::Address address, mb::DataType dataType)
{
    switch (dataType)
    {
    case mb::Bit:
        switch (address.type)
        {
        case Modbus::Memory_0x: return bool_0x(address.offset);
        case Modbus::Memory_1x: return bool_1x(address.offset);
        case Modbus::Memory_3x: return bool_3x(address.offset*MB_REGE_SZ_BITES);
        case Modbus::Memory_4x: return bool_4x(address.offset*MB_REGE_SZ_BITES);
        }
        break;
    case mb::Int8:
        switch (address.type)
        {
        case Modbus::Memory_0x: return int8_0x(address.offset);
        case Modbus::Memory_1x: return int8_1x(address.offset);
        case Modbus::Memory_3x: return int8_3x(address.offset*MB_REGE_SZ_BYTES);
        case Modbus::Memory_4x: return int8_4x(address.offset*MB_REGE_SZ_BYTES);
        }
        break;
    case mb::UInt8:
        switch (address.type)
        {
        case Modbus::Memory_0x: return uint8_0x(address.offset);
        case Modbus::Memory_1x: return uint8_1x(address.offset);
        case Modbus::Memory_3x: return uint8_3x(address.offset*MB_REGE_SZ_BYTES);
        case Modbus::Memory_4x: return uint8_4x(address.offset*MB_REGE_SZ_BYTES);
        }
        break;
    case mb::Int16:
        switch (address.type)
        {
        case Modbus::Memory_0x: return int16_0x(address.offset);
        case Modbus::Memory_1x: return int16_1x(address.offset);
        case Modbus::Memory_3x: return int16_3x(address.offset);
        case Modbus::Memory_4x: return int16_4x(address.offset);
        }
        break;
    case mb::UInt16:
        switch (address.type)
        {
        case Modbus::Memory_0x: return uint16_0x(address.offset);
        case Modbus::Memory_1x: return uint16_1x(address.offset);
        case Modbus::Memory_3x: return uint16_3x(address.offset);
        case Modbus::Memory_4x: return uint16_4x(address.offset);
        }
        break;
    case mb::Int32:
        switch (address.type)
        {
        case Modbus::Memory_0x: return int32_0x(address.offset);
        case Modbus::Memory_1x: return int32_1x(address.offset);
        case Modbus::Memory_3x: return int32_3x(address.offset);
        case Modbus::Memory_4x: return int32_4x(address.offset);
        }
        break;
    case mb::UInt32:
        switch (address.type)
        {
        case Modbus::Memory_0x: return uint32_0x(address.offset);
        case Modbus::Memory_1x: return uint32_1x(address.offset);
        case Modbus::Memory_3x: return uint32_3x(address.offset);
        case Modbus::Memory_4x: return uint32_4x(address.offset);
        }
        break;
    case mb::Int64:
        switch (address.type)
        {
        case Modbus::Memory_0x: return int64_0x(address.offset);
        case Modbus::Memory_1x: return int64_1x(address.offset);
        case Modbus::Memory_3x: return int64_3x(address.offset);
        case Modbus::Memory_4x: return int64_4x(address.offset);
        }
        break;
    case mb::UInt64:
        switch (address.type)
        {
        case Modbus::Memory_0x: return uint64_0x(address.offset);
        case Modbus::Memory_1x: return uint64_1x(address.offset);
        case Modbus::Memory_3x: return uint64_3x(address.offset);
        case Modbus::Memory_4x: return uint64_4x(address.offset);
        }
        break;
    case mb::Float32:
        switch (address.type)
        {
        case Modbus::Memory_0x: return float_0x(address.offset);
        case Modbus::Memory_1x: return float_1x(address.offset);
        case Modbus::Memory_3x: return float_3x(address.offset);
        case Modbus::Memory_4x: return float_4x(address.offset);
        }
        break;
    case mb::Double64:
        switch (address.type)
        {
        case Modbus::Memory_0x: return double_0x(address.offset);
        case Modbus::Memory_1x: return double_1x(address.offset);
        case Modbus::Memory_3x: return double_3x(address.offset);
        case Modbus::Memory_4x: return double_4x(address.offset);
        }
        break;
    }
    return QVariant();
}

void mbServerDevice::setValue(mb::Address address, mb::DataType dataType, const QVariant &v)
{
    switch (dataType)
    {
    case mb::Bit:
        switch (address.type)
        {
        case Modbus::Memory_0x: setBool_0x(address.offset, v.toBool()); break;
        case Modbus::Memory_1x: setBool_1x(address.offset, v.toBool()); break;
        case Modbus::Memory_3x: setBool_3x(address.offset*MB_REGE_SZ_BITES, v.toBool()); break;
        case Modbus::Memory_4x: setBool_4x(address.offset*MB_REGE_SZ_BITES, v.toBool()); break;
        }
        break;
    case mb::Int8:
        switch (address.type)
        {
        case Modbus::Memory_0x: setInt8_0x(address.offset, static_cast<qint8>(v.toInt())); break;
        case Modbus::Memory_1x: setInt8_1x(address.offset, static_cast<qint8>(v.toInt())); break;
        case Modbus::Memory_3x: setInt8_3x(address.offset*MB_REGE_SZ_BYTES, static_cast<qint8>(v.toInt())); break;
        case Modbus::Memory_4x: setInt8_4x(address.offset*MB_REGE_SZ_BYTES, static_cast<qint8>(v.toInt())); break;
        }
        break;
    case mb::UInt8:
        switch (address.type)
        {
        case Modbus::Memory_0x: setUInt8_0x(address.offset, static_cast<quint8>(v.toUInt())); break;
        case Modbus::Memory_1x: setUInt8_1x(address.offset, static_cast<quint8>(v.toUInt())); break;
        case Modbus::Memory_3x: setUInt8_3x(address.offset*MB_REGE_SZ_BYTES, static_cast<quint8>(v.toUInt())); break;
        case Modbus::Memory_4x: setUInt8_4x(address.offset*MB_REGE_SZ_BYTES, static_cast<quint8>(v.toUInt())); break;
        }
        break;
    case mb::Int16:
        switch (address.type)
        {
        case Modbus::Memory_0x: setInt16_0x(address.offset, static_cast<qint16>(v.toInt())); break;
        case Modbus::Memory_1x: setInt16_1x(address.offset, static_cast<qint16>(v.toInt())); break;
        case Modbus::Memory_3x: setInt16_3x(address.offset, static_cast<qint16>(v.toInt())); break;
        case Modbus::Memory_4x: setInt16_4x(address.offset, static_cast<qint16>(v.toInt())); break;
        }
        break;
    case mb::UInt16:
        switch (address.type)
        {
        case Modbus::Memory_0x: setUInt16_0x(address.offset, static_cast<quint16>(v.toUInt())); break;
        case Modbus::Memory_1x: setUInt16_1x(address.offset, static_cast<quint16>(v.toUInt())); break;
        case Modbus::Memory_3x: setUInt16_3x(address.offset, static_cast<quint16>(v.toUInt())); break;
        case Modbus::Memory_4x: setUInt16_4x(address.offset, static_cast<quint16>(v.toUInt())); break;
        }
        break;
    case mb::Int32:
        switch (address.type)
        {
        case Modbus::Memory_0x: setInt32_0x(address.offset, static_cast<qint32>(v.toInt())); break;
        case Modbus::Memory_1x: setInt32_1x(address.offset, static_cast<qint32>(v.toInt())); break;
        case Modbus::Memory_3x: setInt32_3x(address.offset, static_cast<qint32>(v.toInt())); break;
        case Modbus::Memory_4x: setInt32_4x(address.offset, static_cast<qint32>(v.toInt())); break;
        }
        break;
    case mb::UInt32:
        switch (address.type)
        {
        case Modbus::Memory_0x: setUInt32_0x(address.offset, static_cast<quint32>(v.toUInt())); break;
        case Modbus::Memory_1x: setUInt32_1x(address.offset, static_cast<quint32>(v.toUInt())); break;
        case Modbus::Memory_3x: setUInt32_3x(address.offset, static_cast<quint32>(v.toUInt())); break;
        case Modbus::Memory_4x: setUInt32_4x(address.offset, static_cast<quint32>(v.toUInt())); break;
        }
        break;
    case mb::Int64:
        switch (address.type)
        {
        case Modbus::Memory_0x: setInt64_0x(address.offset, v.toLongLong()); break;
        case Modbus::Memory_1x: setInt64_1x(address.offset, v.toLongLong()); break;
        case Modbus::Memory_3x: setInt64_3x(address.offset, v.toLongLong()); break;
        case Modbus::Memory_4x: setInt64_4x(address.offset, v.toLongLong()); break;
        }
        break;
    case mb::UInt64:
        switch (address.type)
        {
        case Modbus::Memory_0x: setUInt64_0x(address.offset, v.toULongLong()); break;
        case Modbus::Memory_1x: setUInt64_1x(address.offset, v.toULongLong()); break;
        case Modbus::Memory_3x: setUInt64_3x(address.offset, v.toULongLong()); break;
        case Modbus::Memory_4x: setUInt64_4x(address.offset, v.toULongLong()); break;
        }
        break;
    case mb::Float32:
        switch (address.type)
        {
        case Modbus::Memory_0x: setFloat_0x(address.offset, v.toFloat()); break;
        case Modbus::Memory_1x: setFloat_1x(address.offset, v.toFloat()); break;
        case Modbus::Memory_3x: setFloat_3x(address.offset, v.toFloat()); break;
        case Modbus::Memory_4x: setFloat_4x(address.offset, v.toFloat()); break;
        }
        break;
    case mb::Double64:
        switch (address.type)
        {
        case Modbus::Memory_0x: setDouble_0x(address.offset, v.toDouble()); break;
        case Modbus::Memory_1x: setDouble_1x(address.offset, v.toDouble()); break;
        case Modbus::Memory_3x: setDouble_3x(address.offset, v.toDouble()); break;
        case Modbus::Memory_4x: setDouble_4x(address.offset, v.toDouble()); break;
        }
        break;
    }
}
