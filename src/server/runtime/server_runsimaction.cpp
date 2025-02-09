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
#include "server_runsimaction.h"

#include <project/server_device.h>

mbServerRunSimAction::mbServerRunSimAction(const MBSETTINGS &settings)
{
    const mbServerSimAction::Strings &sAction = mbServerSimAction::Strings::instance();
    m_device  = reinterpret_cast<mbServerDevice*>(settings.value(sAction.device).value<void*>());
    m_address = mb::toAddress(settings.value(sAction.address).toInt());
    m_period  = settings.value(sAction.period).toInt();
    m_byteOrder = mb::enumDataOrderValue(settings.value(sAction.byteOrder), mb::LessSignifiedFirst);
    m_registerOrder = mb::toRegisterOrder(settings.value(sAction.registerOrder), mb::R0R1R2R3);
}

mbServerRunSimAction::~mbServerRunSimAction()
{

}

QVariant mbServerRunSimAction::value() const
{
    return m_device->value(address(), dataType());
}

void mbServerRunSimAction::setValue(const QVariant &value)
{
    m_device->setValue(address(), dataType(), value);
}

void mbServerRunSimAction::trySwap(void *d, int size)
{
    // TODO: resolve this conditions in compiling stage
    if ((size > 1) && (m_byteOrder == mb::MostSignifiedFirst))
        mb::changeByteOrder(d, size);
    switch (size)
    {
    case 4:
        if (mb::toDataOrder(m_registerOrder) == mb::MostSignifiedFirst)
            mb::swapRegisters32(d);
        break;
    case 8:
        mb::swapRegisters64(d, m_registerOrder);
        break;
    }
}

int mbServerRunSimAction::init(qint64 time)
{
    m_last = time;
    return 0;
}

int mbServerRunSimAction::exec(qint64 /*time*/)
{
    return 0;
}

int mbServerRunSimAction::final(qint64 /*time*/)
{
    return 0;
}

mbServerRunSimAction *createRunActionIncrement(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunSimActionIncrement<bool>   (settings);
    case mb::Int8    : return new mbServerRunSimActionIncrement<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunSimActionIncrement<quint8> (settings);
    case mb::Int16   : return new mbServerRunSimActionIncrement<qint16> (settings);
    case mb::UInt16  : return new mbServerRunSimActionIncrement<quint16>(settings);
    case mb::Int32   : return new mbServerRunSimActionIncrement<qint32> (settings);
    case mb::UInt32  : return new mbServerRunSimActionIncrement<quint32>(settings);
    case mb::Int64   : return new mbServerRunSimActionIncrement<qint64> (settings);
    case mb::UInt64  : return new mbServerRunSimActionIncrement<quint64>(settings);
    case mb::Float32 : return new mbServerRunSimActionIncrement<float>  (settings);
    case mb::Double64: return new mbServerRunSimActionIncrement<double> (settings);
    }
    return nullptr;
}

mbServerRunSimAction *createRunActionSine(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunSimActionSine<bool>   (settings);
    case mb::Int8    : return new mbServerRunSimActionSine<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunSimActionSine<quint8> (settings);
    case mb::Int16   : return new mbServerRunSimActionSine<qint16> (settings);
    case mb::UInt16  : return new mbServerRunSimActionSine<quint16>(settings);
    case mb::Int32   : return new mbServerRunSimActionSine<qint32> (settings);
    case mb::UInt32  : return new mbServerRunSimActionSine<quint32>(settings);
    case mb::Int64   : return new mbServerRunSimActionSine<qint64> (settings);
    case mb::UInt64  : return new mbServerRunSimActionSine<quint64>(settings);
    case mb::Float32 : return new mbServerRunSimActionSine<float>  (settings);
    case mb::Double64: return new mbServerRunSimActionSine<double> (settings);
    }
    return nullptr;
}

mbServerRunSimAction *createRunActionRandom(mb::DataType dataType, const MBSETTINGS &settings)
{
    switch (dataType)
    {
    case mb::Bit     : return new mbServerRunSimActionRandom<bool>   (settings);
    case mb::Int8    : return new mbServerRunSimActionRandom<qint8>  (settings);
    case mb::UInt8   : return new mbServerRunSimActionRandom<quint8> (settings);
    case mb::Int16   : return new mbServerRunSimActionRandom<qint16> (settings);
    case mb::UInt16  : return new mbServerRunSimActionRandom<quint16>(settings);
    case mb::Int32   : return new mbServerRunSimActionRandom<qint32> (settings);
    case mb::UInt32  : return new mbServerRunSimActionRandom<quint32>(settings);
    case mb::Int64   : return new mbServerRunSimActionRandom<qint64> (settings);
    case mb::UInt64  : return new mbServerRunSimActionRandom<quint64>(settings);
    case mb::Float32 : return new mbServerRunSimActionRandom<float>  (settings);
    case mb::Double64: return new mbServerRunSimActionRandom<double> (settings);
    }
    return nullptr;
}

mbServerRunSimAction *createRunActionCopy(const MBSETTINGS &settings)
{
    return new mbServerRunSimActionCopy(settings);
}

mbServerRunSimActionCopy::mbServerRunSimActionCopy(const MBSETTINGS &settings) : mbServerRunSimAction(settings)
{
    const mbServerSimAction::Strings &s = mbServerSimAction::Strings::instance();
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

int mbServerRunSimActionCopy::exec(qint64 time)
{
    if (((time-this->m_last) >= this->m_period))
    {
        (m_device->*m_methodRead )(m_src, m_srcCount, m_buffer.data(), nullptr);
        (m_device->*m_methodWrite)(m_dst, m_dstCount, m_buffer.data(), nullptr);
    }
    return 0;
}
