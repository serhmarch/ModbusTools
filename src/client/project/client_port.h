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
#ifndef CLIENT_PORT_H
#define CLIENT_PORT_H

#include <QSet>
#include <QSerialPort>

#include <Modbus.h>

#include <project/core_port.h>

class mbClientProject;
class mbClientDevice;

class mbClientPort : public mbCorePort
{
    Q_OBJECT

public:
    explicit mbClientPort(QObject* parent = nullptr);
    virtual ~mbClientPort();

public:
    inline mbClientProject* project() const { return reinterpret_cast<mbClientProject*>(mbCorePort::projectCore()); }
    inline void setProject(mbClientProject* project) { mbCorePort::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // devices
    inline bool hasDevice(const QString& name) const { return device(name); }
    inline bool hasDevice(mbClientDevice* device) const { return m_devices.contains(device); }
    inline QList<mbClientDevice*> devices() const { return m_devices; }
    inline int deviceIndex(mbClientDevice* device) const { return m_devices.indexOf(device); }
    inline int deviceIndex(const QString& name) const { return deviceIndex(device(name)); }
    inline mbClientDevice* device(int i) const { return m_devices.value(i); }
    mbClientDevice *device(const QString& name) const;
    inline mbClientDevice* deviceAt(int i) const { return m_devices.at(i); }
    inline int deviceCount() const { return m_devices.count(); }
    int deviceInsert(mbClientDevice* device, int index = -1);
    inline int deviceAdd(mbClientDevice* device) { return deviceInsert(device); }
    int deviceRemove(int index);
    inline int deviceRemove(const QString& name) { return deviceRemove(deviceIndex(name)); }
    inline int deviceRemove(mbClientDevice* device) { return deviceRemove(deviceIndex(device)); }

Q_SIGNALS:
    void deviceAdded(mbClientDevice*);
    void deviceRemoving(mbClientDevice*);
    void deviceRemoved(mbClientDevice*);

private:
    typedef QList<mbClientDevice*> Devices_t;
    Devices_t m_devices;
};

#endif // CLIENT_PORT_H
