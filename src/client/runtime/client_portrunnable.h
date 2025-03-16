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
#ifndef CLIENT_PORTRUNNABLE_H
#define CLIENT_PORTRUNNABLE_H

#include <QObject>

#include <client_global.h>

#include <project/client_port.h>

class ModbusClient;
class ModbusClientPort;

class mbClientRunPort;
class mbClientRunDevice;
class mbClientDeviceRunnable;

class mbClientPortRunnable : public QObject
{
    Q_OBJECT

public:
    enum State
    {
        STATE_PAUSE        ,
        STATE_EXEC_EXTERNAL
    };

public:
    explicit mbClientPortRunnable(mbClientRunPort *clientPort, const Modbus::Settings &settings, QObject *parent = nullptr);
    ~mbClientPortRunnable();

public:
    inline QString name() const { return objectName(); }
    inline void setName(const QString &name) { setObjectName(name); }
    
public:
    void run();
    void close();

private:
    inline mbClientDeviceRunnable *deviceRunnable(const ModbusClient *c) const { return m_hashRunnables.value(c); }

private:
    Modbus::StatusCode execExternalMessage();

private Q_SLOTS:
    void slotBytesTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotBytesRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotAsciiTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotAsciiRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);

private:
    State m_state;

private:
    mbClientRunPort *m_port;
    ModbusClientPort *m_modbusClientPort;
    uint8_t m_byteCount;
    QList<mbClientRunDevice*> m_devices;
    mbClientPort::Statistic m_stat;
    mbClientRunMessagePtr m_currentMessage;

private:
    typedef QList<mbClientDeviceRunnable*> Runnables_t;
    typedef QHash<const ModbusClient*, mbClientDeviceRunnable*> HashRunnables_t;
    
    Runnables_t m_runnables;
    HashRunnables_t m_hashRunnables;
};

#endif // CLIENT_PORTRUNNABLE_H
