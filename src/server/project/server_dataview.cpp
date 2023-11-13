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
#include "server_dataview.h"

#include "server_device.h"

mbServerDataViewItem::mbServerDataViewItem(QObject *parent) :
    mbCoreDataViewItem(parent)
{
}

QVariant mbServerDataViewItem::value() const
{
    QVariant value;
    if (!device())
        return value;
    QByteArray data = device()->readData(m_address, count());
    if (data.isEmpty())
        return value;
    return toVariant(data);
}

void mbServerDataViewItem::setValue(const QVariant &value)
{
    if (!device())
        return;
    QByteArray data = toByteArray(value);
    if (data.length())
    {
        quint16 count;
        switch (m_address.type)
        {
        case Modbus::Memory_0x:
        case Modbus::Memory_1x:
            if (m_format == mb::Bool)
                count = 1;
            else
                count = static_cast<quint16>(data.length()*MB_BYTE_SZ_BITES);
            break;
        case Modbus::Memory_3x:
        case Modbus::Memory_4x:
        default:
            count = static_cast<quint16>(data.length()/MB_REGE_SZ_BYTES);
            break;
        }
        device()->writeData(m_address, count, data);
    }
}

mbServerDataView::mbServerDataView(QObject *parent)
    : mbCoreDataView{parent}
{

}
