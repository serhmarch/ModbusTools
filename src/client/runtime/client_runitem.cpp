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
#include "client_runitem.h"

#include <client.h>
#include "client_runmessage.h"

void mbClientRunItem::init(mb::Client::ItemHandle_t handle, Modbus::MemoryType memoryType, uint16_t offset, uint16_t count)
{
    m_message = nullptr;
    m_handle = handle;
    m_memoryType = memoryType;
    m_offset = offset;
    m_count = count;
}

mbClientRunItem::mbClientRunItem(mb::Client::ItemHandle_t handle, Modbus::MemoryType memoryType, uint16_t offset, uint16_t count, uint32_t period, int size)
{
    init(handle, memoryType, offset, count);
    m_period = period;
    m_data = QByteArray(size, '\0');
}

mbClientRunItem::mbClientRunItem(mb::Client::ItemHandle_t handle, Modbus::MemoryType memoryType, uint16_t offset, uint16_t count, const QByteArray &data)
{
    init(handle, memoryType, offset, count);
    m_period = 0;
    m_data = data;
}

mbClientRunItem::~mbClientRunItem()
{
    m_data.clear();
}

void mbClientRunItem::setData(const QByteArray &data)
{
    m_data = data;
}

void mbClientRunItem::readDataFromMessage()
{
    Modbus::StatusCode status = m_message->status();
    mb::Timestamp_t timestamp = m_message->timestamp();
    if (Modbus::StatusIsGood(status))
    {
        uint16_t innerOffset = m_offset - m_message->offset();
        if (m_message->getData(innerOffset, count(), m_data.data()))
        {
            update(m_data, status, timestamp);
            return;
        }
        else
            status = Modbus::Status_Bad;
    }
    update(status, timestamp);
}

void mbClientRunItem::writeDataToMessage()
{
    uint16_t innerOffset = m_offset - m_message->offset();
    m_message->setData(innerOffset, count(), m_data.data());
}

void mbClientRunItem::update(const QByteArray &data, Modbus::StatusCode status, mb::Timestamp_t timestamp)
{
    mbClient::global()->updateItem(m_handle, data, status, timestamp);
}

void mbClientRunItem::update(Modbus::StatusCode status, mb::Timestamp_t timestamp)
{
    mbClient::global()->updateItem(m_handle, QByteArray(), status, timestamp);
}
