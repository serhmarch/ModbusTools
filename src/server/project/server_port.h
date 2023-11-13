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
#ifndef SERVER_PORT_H
#define SERVER_PORT_H

#include <QSet>
#include <QSerialPort>

#include <Modbus.h>

#include <project/core_port.h>

class mbServerProject;
class mbServerDevice;
class mbServerDeviceRef;

class mbServerPort : public mbCorePort
{
    Q_OBJECT

public:
    explicit mbServerPort(QObject* parent = nullptr);
    virtual ~mbServerPort();

public:
    inline mbServerProject* project() const { return reinterpret_cast<mbServerProject*>(mbCorePort::projectCore()); }
    inline void setProject(mbServerProject* project) { mbCorePort::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // devices
    int freeDeviceUnit() const;
    inline bool hasDevice(mbServerDeviceRef* device) const { return m_devices.contains(device); }
    inline QList<mbServerDeviceRef*> devices() const { return m_devices; }
    inline int deviceIndex(mbServerDeviceRef* device) const { return m_devices.indexOf(device); }
    inline mbServerDeviceRef* device(int i) const { return m_devices.value(i); }
    mbServerDeviceRef* deviceRef(mbServerDevice *device) const;
    inline mbServerDeviceRef* deviceAt(int i) const { return m_devices.at(i); }
    inline int deviceCount() const { return m_devices.count(); }
    int deviceInsert(mbServerDeviceRef* device, int index = -1);
    inline int deviceAdd(mbServerDeviceRef* device) { return deviceInsert(device); }
    int deviceRemove(int index);
    inline int deviceRemove(mbServerDeviceRef* device) { return deviceRemove(deviceIndex(device)); }
    void deviceRemove(mbServerDevice* device);
    bool deviceRename(mbServerDeviceRef* device, const QString& newName);
    inline mbServerDeviceRef *deviceByUnit(quint8 unit) const { return m_units[unit]; }
    QList<quint8> deviceFilterUnits(mbServerDeviceRef *device, const QList<quint8> &units) const;
    QList<quint8> deviceSetUnits(mbServerDeviceRef *device, const QList<quint8> &units);

public:
    inline mbServerDevice *device(uint8_t unit) const;

Q_SIGNALS:
    void deviceAdded(mbServerDeviceRef*);
    void deviceRemoving(mbServerDeviceRef*);
    void deviceRemoved(mbServerDeviceRef*);

private:
    void deviceRemoveUnits(mbServerDeviceRef *device);

private: // devices
    static const int UnitsSize = 256;
    mbServerDeviceRef* m_units[UnitsSize];
    typedef QList<mbServerDeviceRef*> Devices_t;
    typedef QHash<QString, mbServerDeviceRef*> HashDevices_t;
    Devices_t m_devices;
};

#endif // SERVER_PORT_H
