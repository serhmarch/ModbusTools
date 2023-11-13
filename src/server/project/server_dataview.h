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
#ifndef SERVER_DATAVIEW_H
#define SERVER_DATAVIEW_H

#include <QPointer>

#include <mbcore.h>

#include <project/core_dataview.h>

class mbServerProject;
class mbServerDevice;

class mbServerDataViewItem : public mbCoreDataViewItem
{
    Q_OBJECT

public:
    mbServerDataViewItem(QObject *parent = nullptr);

public:
    inline mbServerDevice *device() const { return reinterpret_cast<mbServerDevice*>(deviceCore()); }
    inline void setDevice(mbServerDevice *device) { setDeviceCore(reinterpret_cast<mbCoreDevice*>(device)); }

public:
    QVariant value() const override;
    void setValue(const QVariant& value) override;
};

class mbServerDataView : public mbCoreDataView
{
    Q_OBJECT

public:
    mbServerDataView(QObject *parent = nullptr);

public:
    inline mbServerProject* project() const { return reinterpret_cast<mbServerProject*>(mbCoreDataView::projectCore()); }
    inline void setProject(mbServerProject* project) { mbCoreDataView::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // items
    inline bool hasItem(mbServerDataViewItem* item) const { return mbCoreDataView::hasItem(static_cast<mbCoreDataViewItem*>(item)); }
    inline QList<mbServerDataViewItem*> items() const { return QList<mbServerDataViewItem*>(*(reinterpret_cast<const QList<mbServerDataViewItem*>*>(&m_items))); }
    inline int itemIndex(mbServerDataViewItem* item) const { return mbCoreDataView::itemIndex(reinterpret_cast<mbServerDataViewItem*>(item)); }
    inline mbServerDataViewItem *item(int i) const { return reinterpret_cast<mbServerDataViewItem*>(mbCoreDataView::itemCore(i)); }
    inline mbServerDataViewItem *itemAt(int i) const { return reinterpret_cast<mbServerDataViewItem*>(mbCoreDataView::itemCoreAt(i)); }
    inline int itemInsert(mbServerDataViewItem* item, int index = -1) { return mbCoreDataView::itemInsert(reinterpret_cast<mbCoreDataViewItem*>(item), index); }
    inline int itemAdd(mbServerDataViewItem* item) { return mbCoreDataView::itemAdd(reinterpret_cast<mbCoreDataViewItem*>(item)); }
    inline int itemRemove(mbServerDataViewItem* item) { return mbCoreDataView::itemRemove(reinterpret_cast<mbCoreDataViewItem*>(item)); }
};

#endif // SERVER_DATAVIEW_H
