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
#ifndef SERVER_PROJECTMODEL_H
#define SERVER_PROJECTMODEL_H

#include <core/gui/project/core_projectmodel.h>

class mbServerProject;
class mbServerPort;
class mbServerDeviceRef;

class mbServerProjectModel : public mbCoreProjectModel
{
    Q_OBJECT

public:
    mbServerProjectModel(QObject* parent = nullptr);

public:
    mbServerProject *project() const { return reinterpret_cast<mbServerProject*>(projectCore()); }

public: // table model interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public:
    inline QModelIndex portIndex(mbServerPort *port) const { return mbCoreProjectModel::portIndex(reinterpret_cast<mbCorePort*>(port)); }
    inline mbServerPort *port(const QModelIndex &index) const { return reinterpret_cast<mbServerPort*>(portCore(index)); }
    mbCorePort *getPortByIndex(const QModelIndex &index) const override;
    mbCoreDevice *getDeviceByIndex(const QModelIndex &index) const override;

public:
    QModelIndex deviceIndex(mbServerDeviceRef *port) const;
    mbServerDeviceRef *deviceRef(const QModelIndex &index) const;

protected:
    QString deviceName(const mbServerDeviceRef *device) const;

private Q_SLOTS:
    void portAdd(mbCorePort *port) override;
    void portRemove(mbCorePort *port) override;
    void deviceAdd(mbServerDeviceRef *device);
    void deviceRemove(mbServerDeviceRef *device);
    void deviceChanged();
};

#endif // SERVER_PROJECTMODEL_H
