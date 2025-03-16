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
#ifndef CLIENT_RUNTIME_H
#define CLIENT_RUNTIME_H

#include <client_global.h>
#include <project/client_project.h>
#include <runtime/core_runtime.h>

class mbClientPort;
class mbClientDevice;
class mbClientDataViewItem;
class mbClientRunPort;
class mbClientRunDevice;
class mbClientRunItem;
class mbClientRunThread;

class mbClientRuntime : public mbCoreRuntime
{
    Q_OBJECT
public:
    explicit mbClientRuntime(QObject *parent = nullptr);

public:
    inline mbClientProject *project() const { return static_cast<mbClientProject*>(projectCore()); }

public:
    void sendPortMessage(mb::Client::PortHandle_t handle, const mbClientRunMessagePtr &message);
    void sendMessage(mb::Client::DeviceHandle_t handle, const mbClientRunMessagePtr &message);
    void updateItem(mb::Client::ItemHandle_t handle, const QByteArray &data, mb::StatusCode status, mb::Timestamp_t timestamp);
    inline void updateItem(mb::Client::ItemHandle_t handle, const QByteArray &data, Modbus::StatusCode status, mb::Timestamp_t timestamp) { updateItem(handle, data, static_cast<mb::StatusCode>(status), timestamp); }
    void writeItemData(mb::Client::ItemHandle_t handle, const QByteArray &data);

private:
    void createComponents() override;
    void startComponents() override;
    void beginStopComponents() override;
    bool tryStopComponents() override;
    void clearComponents() override;

private:
    mbClientRunItem *createRunItem(mbClientDataViewItem *item);
    mbClientRunItem *createRunItem(mbClientDataViewItem *item, const QByteArray &data);
    mbClientRunPort *createRunPort(mbClientPort *port);
    mbClientRunDevice *createRunDevice(mbClientDevice *device);
    mbClientRunThread *createRunThread(mbClientRunPort *port);

private: // items
    typedef QHash<mbClientDataViewItem*, mbClientRunItem*> Items_t;
    Items_t m_items;

private: // ports
    typedef QHash<mbClientPort*, mbClientRunPort*> Ports_t;
    Ports_t m_ports;

private: // devices
    typedef QHash<mbClientDevice*, mbClientRunDevice*> Devices_t;
    Devices_t m_devices;

private: // threads
    typedef QHash<mbClientRunPort*, mbClientRunThread*> Threads_t;
    Threads_t m_threads;
};

#endif // CLIENT_RUNTIME_H
