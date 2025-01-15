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
#include "server_dataviewdelegate.h"

#include <QEvent>
#include <QComboBox>

#include <server.h>
#include <project/server_project.h>
#include <project/server_dataview.h>
#include <project/server_device.h>

#include "server_dataviewmodel.h"

mbServerDataViewDelegate::mbServerDataViewDelegate(QObject *parent) :
    mbCoreDataViewDelegate(parent)
{
}

QWidget *mbServerDataViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbServerDataView::Device:
    {
        QComboBox *cmb = new QComboBox(parent);
        QList<mbServerDevice*> ls = mbServer::global()->project()->devices();
        Q_FOREACH (mbServerDevice* d, ls)
            cmb->addItem(d->name());
        return cmb;
    }
    case mbServerDataView::Format:
    {
        QComboBox *cmb = new QComboBox(parent);
        QStringList ls = mb::enumFormatKeyList();
        Q_FOREACH (const QString &s, ls)
            cmb->addItem(s);
        return cmb;
    }
    default:
        QWidget *w = QStyledItemDelegate::createEditor(parent, option, index);
        QStyledItemDelegate::setEditorData(w, index);
        return w;
    }
}

void mbServerDataViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbServerDataView::Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
    break;
    case mbServerDataView::Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
    break;
    case mbServerDataView::Value:
        break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void mbServerDataViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbServerDataView::Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
    break;
    case mbServerDataView::Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
    break;
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
    if (index.column() == mbServerDataView::Value)
        return;
}
