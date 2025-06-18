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
#ifndef CLIENT_PROJECTUI_H
#define CLIENT_PROJECTUI_H

#include <core/gui/project/core_projectui.h>

class mbClientPort;
class mbClientDevice;
class mbClientProjectTreeView;

class mbClientProjectUi : public mbCoreProjectUi
{
    Q_OBJECT
public:
    explicit mbClientProjectUi(QWidget *parent = nullptr);

public:
    inline mbClientPort *currentPort() const { return reinterpret_cast<mbClientPort*>(currentPortCore ()); }
    inline mbClientPort *selectedPort() const { return reinterpret_cast<mbClientPort*>(selectedPortCore()); }
    inline mbClientDevice *currentDevice() const { return reinterpret_cast<mbClientDevice*>(currentDeviceCore ()); }
    inline mbClientDevice *selectedDevice() const { return reinterpret_cast<mbClientDevice*>(selectedDeviceCore()); }

Q_SIGNALS:
    void deviceDoubleClick(mbClientDevice *device);
    void deviceContextMenu(mbClientDevice *device);

protected Q_SLOTS:
    void doubleClick(const QModelIndex &index) override;
    void contextMenu(const QModelIndex &index) override;
};

#endif // CLIENT_PROJECTUI_H
