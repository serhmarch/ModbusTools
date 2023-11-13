/*
    Modbus

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
#include "ModbusPort.h"

namespace Modbus {

Port::Strings::Strings() :
    type(QStringLiteral("type")),
    server(QStringLiteral("server"))
{
}

const Port::Strings &Port::Strings::instance()
{
    static const Strings s;
    return s;
}

Port::Defaults::Defaults() :
    type(Modbus::TCP)
{
}

const Port::Defaults &Port::Defaults::instance()
{
    static const Defaults d;
    return d;
}

Port::Port(QObject *parent) : QObject(parent)
{
    m_state = STATE_UNKNOWN;
    m_unit = 0;
    m_func = 0;
    m_block = false;
    m_modeServer = false;
    clearChanged();
}

Port::~Port()
{
}

Modbus::Settings Port::settings() const
{
    return Modbus::Settings();
}

bool Port::setSettings(const Modbus::Settings & /* settings */)
{
    return true;
}

void Port::setNextRequestRepeated(bool /* v */)
{
}

void Port::setServerMode(bool mode)
{
    m_modeServer = mode;
}

} // namespace Modbus
