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
