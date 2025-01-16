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
#include "core_dataviewdelegate.h"

#include <QEvent>
#include <QComboBox>

#include <core.h>
#include <project/core_project.h>
#include <project/core_device.h>
#include <project/core_dataview.h>
#include "core_dataviewmodel.h"

mbCoreDataViewDelegate::mbCoreDataViewDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

bool mbCoreDataViewDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type())
    {
    //case QEvent::MouseButtonDblClick:
    //    Q_EMIT doubleClick(index);
    //    return true;
    case QEvent::ContextMenu:
        Q_EMIT contextMenu(index);
        return true;
    default:
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
}

QWidget *mbCoreDataViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    const mbCoreDataView *dataView = static_cast<const mbCoreDataViewModel*>(index.model())->dataViewCore();
    int type = dataView->getColumnTypeByIndex(index.column());
    switch (type)
    {
    case mbCoreDataView::Device:
    {
        QComboBox *cmb = new QComboBox(parent);
        QList<mbCoreDevice*> ls = mbCore::globalCore()->projectCore()->devicesCore();
        Q_FOREACH (mbCoreDevice* d, ls)
            cmb->addItem(d->name());
        return cmb;
    }
    case mbCoreDataView::Format:
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

void mbCoreDataViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    const mbCoreDataView *dataView = static_cast<const mbCoreDataViewModel*>(index.model())->dataViewCore();
    int type = dataView->getColumnTypeByIndex(index.column());
    switch (type)
    {
    case mbCoreDataView::Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
    break;
    case mbCoreDataView::Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
    break;
    case mbCoreDataView::Value:
        break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void mbCoreDataViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    const mbCoreDataView *dataView = static_cast<const mbCoreDataViewModel*>(index.model())->dataViewCore();
    int type = dataView->getColumnTypeByIndex(index.column());
    switch (type)
    {
    case mbCoreDataView::Device:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
    break;
    case mbCoreDataView::Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
    break;
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
    if (index.column() == mbCoreDataView::Value)
        return;
}
