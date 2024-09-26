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
#include "server_runaction.h"

#include <project/server_device.h>

mbServerRunAction::mbServerRunAction(const MBSETTINGS &settings)
{
    const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
    m_device  = reinterpret_cast<mbServerDevice*>(settings.value(sAction.device).value<void*>());
    m_address = mb::toAddress(settings.value(sAction.address).toInt());
    m_period  = settings.value(sAction.period).toInt();
    m_byteOrder = mb::enumDataOrderValue(settings.value(sAction.byteOrder), mb::LessSignifiedFirst);
    m_registerOrder = mb::enumDataOrderValue(settings.value(sAction.registerOrder), mb::LessSignifiedFirst);
}

mbServerRunAction::~mbServerRunAction()
{

}

QVariant mbServerRunAction::value() const
{
    return m_device->value(address(), dataType());
}

void mbServerRunAction::setValue(const QVariant &value)
{
    m_device->setValue(address(), dataType(), value);
}

void mbServerRunAction::trySwap(void *d, int size)
{
    // TODO: resolve this conditions in compiling stage
    if ((size > 1) && (m_byteOrder == mb::MostSignifiedFirst))
        mb::changeByteOrder(d, size);
    switch (size)
    {
    case 4:
        if (m_registerOrder == mb::MostSignifiedFirst)
            mb::swapRegisters32(d);
        break;
    case 8:
        if (m_registerOrder == mb::MostSignifiedFirst)
            mb::swapRegisters64(d);
        break;
    }
}

int mbServerRunAction::init(qint64 time)
{
    m_last = time;
    return 0;
}

int mbServerRunAction::exec(qint64 /*time*/)
{
    return 0;
}

int mbServerRunAction::final(qint64 /*time*/)
{
    return 0;
}

mbServerRunAction *createRunActionIncrement(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunActionIncrement<bool>   (settings);
    case mb::Int8    : return new mbServerRunActionIncrement<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunActionIncrement<quint8> (settings);
    case mb::Int16   : return new mbServerRunActionIncrement<qint16> (settings);
    case mb::UInt16  : return new mbServerRunActionIncrement<quint16>(settings);
    case mb::Int32   : return new mbServerRunActionIncrement<qint32> (settings);
    case mb::UInt32  : return new mbServerRunActionIncrement<quint32>(settings);
    case mb::Int64   : return new mbServerRunActionIncrement<qint64> (settings);
    case mb::UInt64  : return new mbServerRunActionIncrement<quint64>(settings);
    case mb::Float32 : return new mbServerRunActionIncrement<float>  (settings);
    case mb::Double64: return new mbServerRunActionIncrement<double> (settings);
    }
    return nullptr;
}

mbServerRunAction *createRunActionSine(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunActionSine<bool>   (settings);
    case mb::Int8    : return new mbServerRunActionSine<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunActionSine<quint8> (settings);
    case mb::Int16   : return new mbServerRunActionSine<qint16> (settings);
    case mb::UInt16  : return new mbServerRunActionSine<quint16>(settings);
    case mb::Int32   : return new mbServerRunActionSine<qint32> (settings);
    case mb::UInt32  : return new mbServerRunActionSine<quint32>(settings);
    case mb::Int64   : return new mbServerRunActionSine<qint64> (settings);
    case mb::UInt64  : return new mbServerRunActionSine<quint64>(settings);
    case mb::Float32 : return new mbServerRunActionSine<float>  (settings);
    case mb::Double64: return new mbServerRunActionSine<double> (settings);
    }
    return nullptr;
}

mbServerRunAction *createRunActionRandom(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunActionRandom<bool>   (settings);
    case mb::Int8    : return new mbServerRunActionRandom<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunActionRandom<quint8> (settings);
    case mb::Int16   : return new mbServerRunActionRandom<qint16> (settings);
    case mb::UInt16  : return new mbServerRunActionRandom<quint16>(settings);
    case mb::Int32   : return new mbServerRunActionRandom<qint32> (settings);
    case mb::UInt32  : return new mbServerRunActionRandom<quint32>(settings);
    case mb::Int64   : return new mbServerRunActionRandom<qint64> (settings);
    case mb::UInt64  : return new mbServerRunActionRandom<quint64>(settings);
    case mb::Float32 : return new mbServerRunActionRandom<float>  (settings);
    case mb::Double64: return new mbServerRunActionRandom<double> (settings);
    }
    return nullptr;
}

mbServerRunAction *createRunActionCopy(const MBSETTINGS &settings)
{
    return new mbServerRunActionCopy(settings);
}

mbServerRunActionCopy::mbServerRunActionCopy(const MBSETTINGS &settings) : mbServerRunAction(settings)
{
    const mbServerAction::Strings &s = mbServerAction::Strings::instance();
    m_dataType = mb::enumDataTypeValue(settings.value(s.dataType));
    mb::Address sourceAddress = mb::toAddress(settings.value(s.copySourceAddress).toInt());
    uint count = settings.value(s.copySize).toUInt();
    switch (m_dataType)
    {
    case mb::Bit:
        m_srcCount = count;
        m_dstCount = count;
        m_buffer.resize((count+7)/8);
        switch (sourceAddress.type)
        {
        case Modbus::Memory_0x: m_methodRead = &mbServerDevice::read_0x    ; m_src = sourceAddress.offset   ; break;
        case Modbus::Memory_1x: m_methodRead = &mbServerDevice::read_1x    ; m_src = sourceAddress.offset   ; break;
        case Modbus::Memory_3x: m_methodRead = &mbServerDevice::read_3x_bit; m_src = sourceAddress.offset*16; break;
        default               : m_methodRead = &mbServerDevice::read_4x_bit; m_src = sourceAddress.offset*16; break;
        }

        switch (m_address.type)
        {
        case Modbus::Memory_0x: m_methodWrite = &mbServerDevice::write_0x    ; m_dst = m_address.offset   ; break;
        case Modbus::Memory_1x: m_methodWrite = &mbServerDevice::write_1x    ; m_dst = m_address.offset   ; break;
        case Modbus::Memory_3x: m_methodWrite = &mbServerDevice::write_3x_bit; m_dst = m_address.offset*16; break;
        default               : m_methodWrite = &mbServerDevice::write_4x_bit; m_dst = m_address.offset*16; break;
        }
        break;
    case mb::Int8:
    case mb::UInt8:
        m_srcCount = count * 8;
        m_dstCount = count * 8;
        m_buffer.resize(count);
        switch (sourceAddress.type)
        {
        case Modbus::Memory_0x: m_methodRead = &mbServerDevice::read_0x    ; m_src = sourceAddress.offset   ; break;
        case Modbus::Memory_1x: m_methodRead = &mbServerDevice::read_1x    ; m_src = sourceAddress.offset   ; break;
        case Modbus::Memory_3x: m_methodRead = &mbServerDevice::read_3x_bit; m_src = sourceAddress.offset*16; break;
        default               : m_methodRead = &mbServerDevice::read_4x_bit; m_src = sourceAddress.offset*16; break;
        }

        switch (m_address.type)
        {
        case Modbus::Memory_0x: m_methodWrite = &mbServerDevice::write_0x    ; m_dst = m_address.offset   ; break;
        case Modbus::Memory_1x: m_methodWrite = &mbServerDevice::write_1x    ; m_dst = m_address.offset   ; break;
        case Modbus::Memory_3x: m_methodWrite = &mbServerDevice::write_3x_bit; m_dst = m_address.offset*16; break;
        default               : m_methodWrite = &mbServerDevice::write_4x_bit; m_dst = m_address.offset*16; break;
        }
        break;
    default:
    {
        uint sz = mb::sizeOfDataType(m_dataType) * count;
        m_src = sourceAddress.offset;
        m_dst = m_address.offset;
        m_buffer.resize(sz);
        switch (sourceAddress.type)
        {
        case Modbus::Memory_0x: m_methodRead = &mbServerDevice::read_0x; m_srcCount = sz*8; break;
        case Modbus::Memory_1x: m_methodRead = &mbServerDevice::read_1x; m_srcCount = sz*8; break;
        case Modbus::Memory_3x: m_methodRead = &mbServerDevice::read_3x; m_srcCount = sz/2; break;
        default               : m_methodRead = &mbServerDevice::read_4x; m_srcCount = sz/2; break;
        }

        switch (m_address.type)
        {
        case Modbus::Memory_0x: m_methodWrite = &mbServerDevice::write_0x; m_dstCount = sz*8; break;
        case Modbus::Memory_1x: m_methodWrite = &mbServerDevice::write_1x; m_dstCount = sz*8; break;
        case Modbus::Memory_3x: m_methodWrite = &mbServerDevice::write_3x; m_dstCount = sz/2; break;
        default               : m_methodWrite = &mbServerDevice::write_4x; m_dstCount = sz/2; break;
        }
    }
        break;
    }
}

int mbServerRunActionCopy::exec(qint64 time)
{
    if (((time-this->m_last) >= this->m_period))
    {
        (m_device->*m_methodRead )(m_src, m_srcCount, m_buffer.data(), nullptr);
        (m_device->*m_methodWrite)(m_dst, m_dstCount, m_buffer.data(), nullptr);
    }
    return 0;
}
