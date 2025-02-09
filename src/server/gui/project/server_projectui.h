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
#ifndef SERVER_PROJECTUI_H
#define SERVER_PROJECTUI_H

#include <core/gui/project/core_projectui.h>

class mbServerPort;
class mbServerDevice;
class mbServerDeviceRef;

class mbServerProjectUi : public mbCoreProjectUi
{
    Q_OBJECT
public:
    explicit mbServerProjectUi(QWidget *parent = nullptr);

public:
    mbServerPort *currentPort() const { return reinterpret_cast<mbServerPort*>(currentPortCore()); }
    mbServerPort *selectedPort() const { return reinterpret_cast<mbServerPort*>(selectedPortCore()); }
    inline mbServerDevice *currentDevice() const { return reinterpret_cast<mbServerDevice*>(currentDeviceCore()); }
    inline mbServerDevice *selectedDevice() const { return reinterpret_cast<mbServerDevice*>(selectedDeviceCore()); }
    mbServerDeviceRef *currentDeviceRef() const;
    inline mbServerDeviceRef *selectedDeviceRef() const { return currentDeviceRef(); }

Q_SIGNALS:
    void deviceDoubleClick(mbServerDeviceRef *device);
    void deviceContextMenu(mbServerDeviceRef *device);

protected Q_SLOTS:
    void doubleClick(const QModelIndex &index) override;
    void contextMenu(const QModelIndex &index) override;
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected) override;
};

#endif // SERVER_PROJECTUI_H
