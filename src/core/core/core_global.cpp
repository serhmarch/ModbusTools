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

#include <project/core_device.h>

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

DataOrder getByteOrder(mbCoreDevice *device, DataOrder byteOrder)
{
    if (byteOrder == mb::DefaultOrder)
    {
        if (device && (device->byteOrder() != mb::DefaultOrder))
            return device->byteOrder();
        return mbCoreDevice::Defaults::instance().byteOrder;
    }
    return byteOrder;
}

RegisterOrder getRegisterOrder(mbCoreDevice *device, RegisterOrder registerOrder)
{
    if (registerOrder == mb::DefaultRegisterOrder)
    {
        if (device && (device->registerOrder() != mb::DefaultRegisterOrder))
            return device->registerOrder();
        return mb::R0R1R2R3;
    }
    return registerOrder;
}

void fillProtocolTypeComboBox(QComboBox *cmb)
{
    cmb->addItem(Modbus::toString(Modbus::ASC    ));
    cmb->addItem(Modbus::toString(Modbus::RTU    ));
    cmb->addItem(Modbus::toString(Modbus::TCP    ));
    cmb->addItem(Modbus::toString(Modbus::ASCvTCP));
    cmb->addItem(Modbus::toString(Modbus::RTUvTCP));
}

} // namespace mb
