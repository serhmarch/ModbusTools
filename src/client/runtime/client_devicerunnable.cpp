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
#include "client_devicerunnable.h"

#include <ModbusClientPort.h>
#include <ModbusClient.h>

#include <client.h>

#include "client_rundevice.h"
#include "client_runmessage.h"
#include "client_runitem.h"

mbClientDeviceRunnable::mbClientDeviceRunnable(mbClientRunDevice *device, ModbusClientPort *port)
{
    m_state = STATE_PAUSE;
    m_device = device;
    m_port = port;
    m_modbusClient = new ModbusClient(m_device->unit(), m_port);
    createReadMessages();
}

mbClientDeviceRunnable::~mbClientDeviceRunnable()
{
    //qDeleteAll(m_writeMessages);
    //delete m_currentMessage;
    //qDeleteAll(m_readMessages);
    delete m_modbusClient;
}

QString mbClientDeviceRunnable::name() const
{
    return m_device->name();
}

void mbClientDeviceRunnable::run()
{
    createWriteMessage();
    Modbus::StatusCode r;
    bool fRepeat;
    do
    {
        fRepeat = false;
        switch (m_state)
        {
        case STATE_PAUSE:
            if (m_device->hasExternalMessage())
            {
                m_device->popExternalMessage(&m_currentMessage);
                m_currentMessage->prepareToSend();
                m_state = STATE_EXEC_EXTERNAL;
                fRepeat = true;
                break;
            }
            if (hasWriteMessage())
            {
                popWriteMessage(&m_currentMessage);
                m_currentMessage->prepareToSend();
                m_state = STATE_EXEC_WRITE;
                fRepeat = true;
                break;
            }
            if (hasReadMessageOnDuty())
            {
                m_state = STATE_EXEC_READ;
                m_currentMessage->prepareToSend();
                fRepeat = true;
                break;
            }
            break;
        case STATE_EXEC_EXTERNAL:
            r = execExternalMessage();
            if (Modbus::StatusIsProcessing(r))
                return;
            m_currentMessage = nullptr;
            m_state = STATE_PAUSE;
            break;
        case STATE_EXEC_WRITE:
            r = execWriteMessage();
            if (Modbus::StatusIsProcessing(r))
                return;
            m_currentMessage = nullptr;
            m_state = STATE_PAUSE;
            break;
        case STATE_EXEC_READ:
            r = execReadMessage();
            if (Modbus::StatusIsProcessing(r))
                return;
            m_currentMessage = nullptr;
            m_state = STATE_PAUSE;
            break;
        }
    }
    while (fRepeat);
}

void mbClientDeviceRunnable::createReadMessages()
{
    QList<mbClientRunItem*> items;
    m_device->popItemsToRead(items);
    Q_FOREACH (mbClientRunItem *item, items)
    {
        mbClientRunMessagePtr m = nullptr;
        for (mbClientRunMessagePtr &message: m_readMessages)
        {
            if (message->addItem(item))
            {
                m = message;
                break;
            }
        }
        if (m == nullptr)
        {
            switch (item->memoryType())
            {
            case Modbus::Memory_0x:
                m = new mbClientRunMessageReadCoils(item, m_device->maxReadCoils());
                pushReadMessage(m);
                break;
            case Modbus::Memory_1x:
                m = new mbClientRunMessageReadDiscreteInputs(item, m_device->maxReadDiscreteInputs());
                pushReadMessage(m);
                break;
            case Modbus::Memory_3x:
                m = new mbClientRunMessageReadInputRegisters(item, m_device->maxReadInputRegisters());
                pushReadMessage(m);
                break;
            case Modbus::Memory_4x:
                m = new mbClientRunMessageReadHoldingRegisters(item, m_device->maxReadHoldingRegisters());
                pushReadMessage(m);
                break;
            default:
                delete item;
                break;
            }
        }
    }
}

void mbClientDeviceRunnable::pushReadMessage(const mbClientRunMessagePtr &message)
{
    message->setDeleteItems(false);
    m_readMessages.append(message);
}

bool mbClientDeviceRunnable::createWriteMessage()
{
    QList<mbClientRunItem*> itemsToWrite;
    if (m_device->popItemsToWrite(itemsToWrite))
    {
        Q_FOREACH (mbClientRunItem *item, itemsToWrite)
        {
            mbClientRunMessagePtr m = nullptr;
            Q_FOREACH (const mbClientRunMessagePtr &message, m_writeMessages)
            {
                if (message->addItem(item))
                {
                    m = message;
                    break;
                }
            }
            if (m == nullptr)
            {
                switch (item->memoryType())
                {
                case Modbus::Memory_0x:
                    m = new mbClientRunMessageWriteMultipleCoils(item, m_device->maxWriteMultipleCoils());
                    pushWriteMessage(m);
                    break;
                case Modbus::Memory_4x:
                    m = new mbClientRunMessageWriteMultipleRegisters(item, m_device->maxWriteMultipleRegisters());
                    pushWriteMessage(m);
                    break;
                default:
                    delete item;
                    break;
                }
            }
        }
        return true;
    }
    return false;
}

void mbClientDeviceRunnable::pushWriteMessage(const mbClientRunMessagePtr &message)
{
    message->setDeleteItems(true);
    m_writeMessages.enqueue(message);
}

bool mbClientDeviceRunnable::popWriteMessage(mbClientRunMessagePtr *message)
{
    if (m_writeMessages.count())
    {
        mbClientRunMessagePtr m = m_writeMessages.dequeue();
        *message = m;
        return true;
    }
    return false;
}

bool mbClientDeviceRunnable::hasReadMessageOnDuty()
{
    if (m_currentMessage)
           return true;
    mb::Timestamp_t tm = QDateTime::currentMSecsSinceEpoch();
    for (Messages_t::Iterator it = m_readMessages.begin(); it != m_readMessages.end(); ++it)
    {
        mbClientRunMessagePtr m = *it;
        if ((tm - m->timestamp()) >= m->period())
        {
            m_readMessages.erase(it);  // remove it from queue ...
            m_readMessages.enqueue(m); // and push it to back of queue
            m_currentMessage = m;
            return true;
        }
    }
    return false;
}

Modbus::StatusCode mbClientDeviceRunnable::execExternalMessage()
{
    Modbus::StatusCode res;
    int func = m_currentMessage->function();
    switch (func)
    {
    case MBF_READ_COILS:
        res = m_modbusClient->readCoils(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_DISCRETE_INPUTS:
        res = m_modbusClient->readDiscreteInputs(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_INPUT_REGISTERS:
        res = m_modbusClient->readInputRegisters(m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_HOLDING_REGISTERS:
        res = m_modbusClient->readHoldingRegisters(m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_EXCEPTION_STATUS:
        res = m_modbusClient->readExceptionStatus(reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_COIL:
        res = m_modbusClient->writeSingleCoil(m_currentMessage->offset(), *reinterpret_cast<const bool*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        res = m_modbusClient->writeSingleRegister(m_currentMessage->offset(), *reinterpret_cast<const uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        res = m_modbusClient->writeMultipleCoils(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusClient->writeMultipleRegisters(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg());
        break;
    case MBF_MASK_WRITE_REGISTER:
        res = m_modbusClient->maskWriteRegister(m_currentMessage->offset(), m_currentMessage->innerBufferReg()[0], m_currentMessage->innerBufferReg()[1]);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusClient->readWriteMultipleRegisters(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBufferReg(),
                                                         m_currentMessage->writeOffset(), m_currentMessage->writeCount(), m_currentMessage->innerBufferReg());
        break;
    default:
        return Modbus::Status_Bad;
    }
    if (Modbus::StatusIsProcessing(res))
        return res;
    if (Modbus::StatusIsBad(res))
    {
        QString text = m_port->lastErrorText();
        mbClient::LogError(m_device->name(), text);
    }
    m_currentMessage->setComplete(res, QDateTime::currentMSecsSinceEpoch());
    return res;
}

Modbus::StatusCode mbClientDeviceRunnable::execWriteMessage()
{
    Modbus::StatusCode res;
    int func = m_currentMessage->function();
    switch (func)
    {
    case MBF_WRITE_SINGLE_COIL:
        res = m_modbusClient->writeSingleCoil(m_currentMessage->offset(), *reinterpret_cast<const bool*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        res = m_modbusClient->writeSingleRegister(m_currentMessage->offset(), *reinterpret_cast<const uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        res = m_modbusClient->writeMultipleCoils(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        res = m_modbusClient->writeMultipleRegisters(m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<const uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    default:
        return Modbus::Status_Bad;
    }
    if (Modbus::StatusIsProcessing(res))
        return res;
    if (Modbus::StatusIsBad(res))
    {
        QString text = m_port->lastErrorText();
        mbClient::LogError(m_device->name(), text);
    }
    m_currentMessage->setComplete(res, QDateTime::currentMSecsSinceEpoch());
    return res;
}

Modbus::StatusCode mbClientDeviceRunnable::execReadMessage()
{
    Modbus::StatusCode res;
    int func = m_currentMessage->function();
    switch (func)
    {
    case MBF_READ_COILS:
        res = m_modbusClient->readCoils(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_DISCRETE_INPUTS:
        res = m_modbusClient->readDiscreteInputs(m_currentMessage->offset(), m_currentMessage->count(), m_currentMessage->innerBuffer());
        break;
    case MBF_READ_INPUT_REGISTERS:
        res = m_modbusClient->readInputRegisters(m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_HOLDING_REGISTERS:
        res = m_modbusClient->readHoldingRegisters(m_currentMessage->offset(), m_currentMessage->count(), reinterpret_cast<uint16_t*>(m_currentMessage->innerBuffer()));
        break;
    case MBF_READ_EXCEPTION_STATUS:
        res = m_modbusClient->readExceptionStatus(reinterpret_cast<uint8_t*>(m_currentMessage->innerBuffer()));
        break;
    default:
        return Modbus::Status_Bad;
    }
    if (Modbus::StatusIsProcessing(res))
        return res;
    if (Modbus::StatusIsBad(res))
    {
        QString text = m_port->lastErrorText();
        mbClient::LogError(m_device->name(), text);
    }
    m_currentMessage->setComplete(res, QDateTime::currentMSecsSinceEpoch());
    return res;
}
