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
#include "core_global.h"

#include <QComboBox>
#include <QSpinBox>

namespace mb {

MBSETTINGS parseExtendedAttributesStr(const QString &str)
{
    MBSETTINGS res;
    QStringList lsPairs = str.split(QLatin1Char(';'));
    Q_FOREACH(QString sPair, lsPairs)
    {
        QStringList keyValue = sPair.split(QLatin1Char('='));
        if (keyValue.count() == 2)
        {
            QString key = keyValue.at(0).trimmed();
            QString value = keyValue.at(1).trimmed();
            res.insert(key, value);
        }
    }
    return res;
}

QVariantList availableBaudRate()
{
    QVariantList ls;
    Q_FOREACH(int32_t v, Modbus::availableBaudRate())
        ls.append(v);
    return ls;
}

QVariantList availableDataBits()
{
    QVariantList ls;
    Q_FOREACH(int8_t v, Modbus::availableDataBits())
        ls.append(v);
    return ls;
}

QVariantList availableParity()
{
    QVariantList ls;
    Q_FOREACH(Modbus::Parity v, Modbus::availableParity())
        ls.append(Modbus::toString(v));
    return ls;
}

QVariantList availableStopBits()
{
    QVariantList ls;
    Q_FOREACH(Modbus::StopBits v, Modbus::availableStopBits())
        ls.append(Modbus::toString(v));
    return ls;
}

QVariantList availableFlowControl()
{
    QVariantList ls;
    Q_FOREACH(Modbus::FlowControl v, Modbus::availableFlowControl())
        ls.append(Modbus::toString(v));
    return ls;
}

Address getModbusAddress(QComboBox *cmb, QSpinBox *sp, AddressNotation notation)
{
    mb::Address adr;
    switch (cmb->currentIndex())
    {
    case 0:
        adr.type = Modbus::Memory_0x;
        break;
    case 1:
        adr.type = Modbus::Memory_1x;
        break;
    case 2:
        adr.type = Modbus::Memory_3x;
        break;
    default:
        adr.type = Modbus::Memory_4x;
        break;
    }

    switch (notation)
    {
    case mb::Address_IEC61131:
        adr.offset = static_cast<quint16>(sp->value());
        break;
    default:
        adr.offset = static_cast<quint16>(sp->value()-1);
        break;
    }
    return adr;
}

void setModbusAddress(QComboBox *cmb, QSpinBox *sp, Address adr, AddressNotation notation)
{
    switch (adr.type)
    {
    case Modbus::Memory_0x:
        cmb->setCurrentIndex(0);
        break;
    case Modbus::Memory_1x:
        cmb->setCurrentIndex(1);
        break;
    case Modbus::Memory_3x:
        cmb->setCurrentIndex(2);
        break;
    default:
        cmb->setCurrentIndex(3);
        break;
    }

    switch (notation)
    {
    case mb::Address_IEC61131:
        sp->setValue(adr.offset);
        break;
    default:
        sp->setValue(adr.offset+1);
        break;
    }
}

void fillModbusAddressUi(QComboBox *cmb, QSpinBox *sp, Address adr, AddressNotation notation)
{
    cmb->clear();
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x, notation));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x, notation));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x, notation));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x, notation));

    switch (notation)
    {
    case mb::Address_IEC61131:
        sp->setMinimum(0);
        sp->setMaximum(USHRT_MAX);
        break;
    default:
        sp->setMinimum(1);
        sp->setMaximum(USHRT_MAX+1);
        break;
    }

    setModbusAddress(cmb, sp, adr, notation);
}

} // namespace mb
