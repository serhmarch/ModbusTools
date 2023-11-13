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
#include "client_dataviewdelegate.h"

#include <QComboBox>

#include <client.h>
#include <project/client_project.h>
#include <project/client_device.h>

#include "client_dataviewmodel.h"

mbClientDataViewDelegate::mbClientDataViewDelegate(QObject *parent) :
    mbCoreDataViewDelegate(parent)
{
}

QWidget *mbClientDataViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbClientDataViewModel::Column_Device:
    {
        QComboBox *cmb = new QComboBox(parent);
        QList<mbClientDevice*> ls = mbClient::global()->project()->devices();
        Q_FOREACH (mbClientDevice* d, ls)
            cmb->addItem(d->name());
        return cmb;
    }
    case mbClientDataViewModel::Column_Format:
    {
        QComboBox *cmb = new QComboBox(parent);
        QMetaEnum formats = QMetaEnum::fromType<mb::Format>();
        for (int i = 0; i < formats.keyCount(); i++)
            cmb->addItem(formats.key(i));
        return cmb;
    }
    default:
        QWidget *w = QStyledItemDelegate::createEditor(parent, option, index);
        QStyledItemDelegate::setEditorData(w, index);
        return w;
    }
}

void mbClientDataViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbClientDataViewModel::Column_Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
        break;
    case mbClientDataViewModel::Column_Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
        break;
    case mbClientDataViewModel::Column_Value:
        break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void mbClientDataViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbClientDataViewModel::Column_Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
        break;
    case mbClientDataViewModel::Column_Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
        break;
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
    if (index.column() == mbClientDataViewModel::Column_Value)
        return;
}

