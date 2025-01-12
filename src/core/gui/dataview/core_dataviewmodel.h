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
#ifndef CORE_DATAVIEWMODEL_H
#define CORE_DATAVIEWMODEL_H

#include <QAbstractTableModel>

#include <mbcore.h>

class mbCoreDataView;
class mbCoreDataViewItem;

class MB_EXPORT mbCoreDataViewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    mbCoreDataViewModel(mbCoreDataView *dataView, QObject* parent = nullptr);
    ~mbCoreDataViewModel();

public:
    inline mbCoreDataView *dataViewCore() const { return m_dataView; }
    QModelIndex itemIndex(mbCoreDataViewItem *item) const;
    mbCoreDataViewItem *itemCore(const QModelIndex &index) const;

public: // table model interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private Q_SLOTS:
    void itemAdded(mbCoreDataViewItem* item);
    void itemRemoving(mbCoreDataViewItem* item);
    void itemChanged(mbCoreDataViewItem* item);
    void addressNotationChanged();

private:
    mbCoreDataView *m_dataView;
};

#endif // CORE_DATAVIEWMODEL_H
