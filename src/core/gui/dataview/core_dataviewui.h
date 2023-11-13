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
#ifndef CORE_DATAVIEWUI_H
#define CORE_DATAVIEWUI_H

#include <QModelIndex>
#include <QWidget>

#include <mbcore.h>

class QTableView;
class mbCoreDataView;
class mbCoreDataViewItem;
class mbCoreDataViewModel;
class mbCoreDataViewDelegate;

class MB_EXPORT mbCoreDataViewUi : public QWidget
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString name;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDataViewUi(mbCoreDataView *dataView, mbCoreDataViewModel *model, mbCoreDataViewDelegate *delegate, QWidget *parent = nullptr);

public: // QWidget
    QString name() const;

public:
    mbCoreDataView *dataViewCore() const { return m_dataView; }
    mbCoreDataViewModel *modelCore() const { return m_model; }
    QModelIndex currentItemModelIndex() const;
    inline int currentItemIndex() const { return currentItemModelIndex().row(); }
    mbCoreDataViewItem *currentItemCore() const;
    QList<mbCoreDataViewItem*> selectedItemsCore() const;
    void selectItem(mbCoreDataViewItem *item);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void itemDoubleClick(mbCoreDataViewItem *item);
    void itemContextMenu(mbCoreDataViewItem *item);

public Q_SLOTS:
    void selectAll();

protected Q_SLOTS:
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);

protected:
    QTableView *m_view;
    mbCoreDataView *m_dataView;
    mbCoreDataViewModel *m_model;
    mbCoreDataViewDelegate *m_delegate;
};

#endif // CORE_DATAVIEWUI_H
