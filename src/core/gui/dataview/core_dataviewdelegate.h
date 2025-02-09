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
#ifndef CORE_DATAVIEWDELEGATE_H
#define CORE_DATAVIEWDELEGATE_H

#include <QStyledItemDelegate>

#include <mbcore.h>

class MB_EXPORT mbCoreDataViewDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    mbCoreDataViewDelegate(QObject *parent = nullptr);

public:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
    QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index ) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index ) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index ) const override;

Q_SIGNALS:
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);
};


#endif // CORE_DATAVIEWDELEGATE_H
