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
#ifndef SERVER_DEVICEMANAGER_H
#define SERVER_DEVICEMANAGER_H

#include <QHash>
#include <QObject>

class mbCoreProject;
class mbCoreDevice;

class mbServerProject;
class mbServerDevice;
class mbServerDeviceUi;

class mbServerDeviceManager : public QObject
{
    Q_OBJECT

public:
    explicit mbServerDeviceManager(QObject *parent = nullptr);

public: // project
    inline mbServerProject *project() const { return m_project; }
    inline QList<mbServerDeviceUi*> deviceUis() const { return m_deviceUis; }
    inline mbServerDeviceUi *activeDeviceUi() const { return m_activeDeviceUi; }
    mbServerDevice *activeDevice() const;

public: // watch list ui
    inline bool hasDeviceUi(const mbServerDevice *device) const { return m_hashDeviceUis.contains(device); }
    inline int deviceCount() const { return m_hashDeviceUis.count(); }
    inline mbServerDeviceUi *deviceUi(mbServerDevice *device) { return m_hashDeviceUis.value(device, nullptr); }

Q_SIGNALS:
    void deviceUiAdd(mbServerDeviceUi *ui);
    void deviceUiRemove(mbServerDeviceUi *ui);
    void deviceUiActivated(mbServerDeviceUi*);
    void deviceUiContextMenu(mbServerDeviceUi*);

public Q_SLOTS:
    void setActiveDeviceUi(mbServerDeviceUi *ui);

private Q_SLOTS:
    void setProject(mbCoreProject *project);
    void deviceAdd(mbCoreDevice *device);
    void deviceRemove(mbCoreDevice *device);
    void deviceContextMenu(const QPoint &pos);

private:
    mbServerProject *m_project;

    typedef QList<mbServerDeviceUi*> DeviceUis_t;
    typedef QHash<const mbServerDevice*, mbServerDeviceUi*> HashDeviceUis_t;
    DeviceUis_t m_deviceUis;
    HashDeviceUis_t m_hashDeviceUis;
    mbServerDeviceUi *m_activeDeviceUi;

};

#endif // SERVER_DEVICEMANAGER_H
