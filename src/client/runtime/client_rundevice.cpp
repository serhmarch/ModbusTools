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
#include "client_rundevice.h"

#include <project/client_device.h>
#include "client_runitem.h"
#include "client_runmessage.h"

mbClientRunDevice::mbClientRunDevice(const Modbus::Settings &settings)
{
    // TODO: make default settings values
    setSettings(settings);
}

mbClientRunDevice::~mbClientRunDevice()
{
    qDeleteAll(m_itemsToWrite);
}

void mbClientRunDevice::pushItemsToRead(const QList<mbClientRunItem *> &itemsToRead)
{
    QWriteLocker _(&m_lock);
    m_itemsToRead.append(itemsToRead);
}

bool mbClientRunDevice::popItemsToRead(QList<mbClientRunItem*> &items)
{
    QWriteLocker _(&m_lock);
    if (m_itemsToRead.count())
    {
        items = m_itemsToRead;
        m_itemsToRead.clear();
        return true;
    }
    return false;
}

void mbClientRunDevice::pushItemsToWrite(const QList<mbClientRunItem *> &items)
{
    QWriteLocker _(&m_lock);
    m_itemsToWrite.append(items);
}

void mbClientRunDevice::pushItemToWrite(mbClientRunItem *item)
{
    QWriteLocker _(&m_lock);
    m_itemsToWrite.append(item);
}

bool mbClientRunDevice::popItemsToWrite(QList<mbClientRunItem *> &items)
{
    QWriteLocker _(&m_lock);
    if (m_itemsToWrite.count())
    {
        items = m_itemsToWrite;
        m_itemsToWrite.clear();
        return true;
    }
    return false;
}

bool mbClientRunDevice::hasExternalMessage() const
{
    QReadLocker _(&m_lock);
    return m_externalMessages.count();
}

void mbClientRunDevice::pushExternalMessage(const mbClientRunMessagePtr &message)
{
    QWriteLocker _(&m_lock);
    m_externalMessages.enqueue(message);
}

bool mbClientRunDevice::popExternalMessage(mbClientRunMessagePtr *message)
{
    QWriteLocker _(&m_lock);
    if (m_externalMessages.count())
    {
        *message = m_externalMessages.dequeue();
        return true;
    }
    return false;
}

void mbClientRunDevice::setSettings(const Modbus::Settings &settings)
{
    QWriteLocker _(&m_lock);
    const mbClientDevice::Strings &s = mbClientDevice::Strings::instance();

    Modbus::Settings::const_iterator it;
    Modbus::Settings::const_iterator end = settings.end();

    it = settings.find(s.name);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.name = var.toString();
    }

    it = settings.find(s.unit);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.unit = static_cast<uint8_t>(var.toUInt());
    }

    it = settings.find(s.maxReadCoils);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxReadCoils = static_cast<uint16_t>(var.toUInt());
    }

    it = settings.find(s.maxReadDiscreteInputs);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxReadDiscreteInputs = static_cast<uint16_t>(var.toUInt());
    }

    it = settings.find(s.maxReadHoldingRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxReadHoldingRegisters = static_cast<uint16_t>(var.toUInt());
    }

    it = settings.find(s.maxReadInputRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxReadInputRegisters = static_cast<uint16_t>(var.toUInt());
    }

    it = settings.find(s.maxWriteMultipleCoils);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxWriteMultipleCoils = static_cast<uint16_t>(var.toUInt());
    }

    it = settings.find(s.maxWriteMultipleRegisters);
    if (it != end)
    {
        QVariant var = it.value();
        m_settings.maxWriteMultipleRegisters = static_cast<uint16_t>(var.toUInt());
    }
}
