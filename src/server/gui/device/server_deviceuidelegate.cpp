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
#include "server_deviceuidelegate.h"

#include <QEvent>

mbServerDeviceUiDelegate::mbServerDeviceUiDelegate(QObject *parent) : QStyledItemDelegate(parent)
{
}

QWidget *mbServerDeviceUiDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QWidget* w = QStyledItemDelegate::createEditor(parent, option, index);
    QStyledItemDelegate::setEditorData(w, index);
    return w;
}

void mbServerDeviceUiDelegate::setEditorData(QWidget * /*editor*/, const QModelIndex & /*index*/) const
{
}

mbServerDeviceUiDelegateBool::mbServerDeviceUiDelegateBool(QObject *parent) : mbServerDeviceUiDelegate(parent)
{

}

bool mbServerDeviceUiDelegateBool::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        bool v = model->data(index).toBool();
        model->setData(index, !v);
    }
        return true;
    default:
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
}
