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
#ifndef SERVER_DEVICEUIDELEGATE_H
#define SERVER_DEVICEUIDELEGATE_H

#include <QStyledItemDelegate>

class mbServerDeviceUiDelegate : public QStyledItemDelegate
{
public:
    mbServerDeviceUiDelegate(QObject *parent = nullptr);

public:
    virtual QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const override;
};

class mbServerDeviceUiDelegateBool : public mbServerDeviceUiDelegate
{
public:
    mbServerDeviceUiDelegateBool(QObject *parent = nullptr);

public:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // SERVER_DEVICEUIDELEGATE_H
