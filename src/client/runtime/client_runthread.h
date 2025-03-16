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
#ifndef CLIENT_RUNTHREAD_H
#define CLIENT_RUNTHREAD_H

#include <QThread>

#include <client_global.h>

class ModbusClient;

class mbClientRunPort;
class mbClientRunDevice;
class mbClientDeviceRunnable;

class mbClientRunThread : public QThread
{
public:
    explicit mbClientRunThread(mbClientRunPort *port, QObject *parent = nullptr);
    ~mbClientRunThread();

public:
    inline void stop() { m_ctrlRun = false; }

protected:
    void run() override;

private:
    bool m_ctrlRun;

private:
    mbClientRunPort *m_port;
    Modbus::Settings m_settings;
    QList<mbClientRunDevice*> m_devices;
};

#endif // CLIENT_RUNTHREAD_H
