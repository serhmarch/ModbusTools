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
#include "server_simactionsdelegate.h"

#include <QEvent>
#include <QComboBox>

#include <server.h>
#include <project/server_project.h>
#include <project/server_simaction.h>

#include "server_simactionsmodel.h"

mbServerSimActionsDelegate::mbServerSimActionsDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{

}

QWidget *mbServerSimActionsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbServerSimActionsModel::Column_Device:
    {
        QComboBox *cmb = new QComboBox(parent);
        QList<mbServerDevice*> ls = mbServer::global()->project()->devices();
        Q_FOREACH (mbServerDevice* d, ls)
            cmb->addItem(d->name());
        return cmb;
    }
    case mbServerSimActionsModel::Column_DataType:
    {
        QComboBox *cmb = new QComboBox(parent);
        QStringList dataTypes = mb::enumDataTypeKeyList();
        Q_FOREACH (const QString &type, dataTypes)
            cmb->addItem(type);
        return cmb;
    }
    case mbServerSimActionsModel::Column_ActionType:
    {
        QComboBox *cmb = new QComboBox(parent);
        QMetaEnum formats = QMetaEnum::fromType<mbServerSimAction::ActionType>();
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

void mbServerSimActionsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbServerSimActionsModel::Column_Device:
    case mbServerSimActionsModel::Column_DataType:
    case mbServerSimActionsModel::Column_ActionType:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
    break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void mbServerSimActionsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    //QVariant oldValue = model->data(index, Qt::DisplayRole);
    switch (index.column())
    {
    case mbServerSimActionsModel::Column_Device:
    case mbServerSimActionsModel::Column_DataType:
    case mbServerSimActionsModel::Column_ActionType:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
        break;
    default:
        QStyledItemDelegate::setModelData(editor, model, index);
        break;
    }
    //if (index.column() == mbServerSimActionsModel::Column_Value)
    //    return;
    //QVariant newValue = model->data(index, Qt::DisplayRole);
    //mbServerSimulationInfoUndoEdit* cmd = new mbServerSimulationInfoUndoEdit(static_cast<mbServerSimActionsModel*>(model), index, oldValue, newValue);
    //m_widget->m_undoStack->push(cmd);
}

bool mbServerSimActionsDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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
