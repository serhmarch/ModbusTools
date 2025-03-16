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
#ifndef CLIENT_DEVICERUNNABLE_H
#define CLIENT_DEVICERUNNABLE_H

#include <QQueue>
#include <QRunnable>

#include <Modbus.h>
#include <client_global.h>

class ModbusClientPort;
class ModbusClient;

class mbClientRunDevice;

class mbClientDeviceRunnable : public QRunnable
{
public:
    enum State
    {
        STATE_PAUSE        ,
        STATE_EXEC_EXTERNAL,
        STATE_EXEC_WRITE   ,
        STATE_EXEC_READ
    };

public:
    mbClientDeviceRunnable(mbClientRunDevice *device, ModbusClientPort *modbusClientPort);
    virtual ~mbClientDeviceRunnable();

public:
    QString name() const;
    inline mbClientRunDevice *device() const { return m_device; }
    inline ModbusClient *modbusClient() const { return m_modbusClient; }
    inline mbClientRunMessagePtr currentMessage() const { return m_currentMessage; }

public:
    void run() override;

private:
    void createReadMessages();
    void pushReadMessage(const mbClientRunMessagePtr &message);

private:
    bool createWriteMessage();
    inline bool hasWriteMessage() const { return m_writeMessages.count() > 0; }
    void pushWriteMessage(const mbClientRunMessagePtr &message);
    bool popWriteMessage(mbClientRunMessagePtr *message);

private:
    bool hasReadMessageOnDuty();

private:
    Modbus::StatusCode execExternalMessage();
    Modbus::StatusCode execWriteMessage();
    Modbus::StatusCode execReadMessage();

private:
    State m_state;

private:
    mbClientRunDevice *m_device;
    ModbusClientPort *m_modbusClientPort;
    ModbusClient *m_modbusClient;
    uint8_t m_byteCount;

private:
    typedef QQueue<mbClientRunMessagePtr> Messages_t;
    Messages_t m_writeMessages;
    Messages_t m_readMessages;

    mbClientRunMessagePtr m_currentMessage;
};

#endif // CLIENT_DEVICERUNNABLE_H
