#include "client_itemsdelegate.h"

#include <QEvent>
#include <QComboBox>

#include <project/client_project.h>
#include <project/client_item.h>
#include <project/client_device.h>

#include "client_itemsmodel.h"

mbClientItemsDelegate::mbClientItemsDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

QWidget *mbClientItemsDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbClientItemsModel::Column_Device:
    {
        QComboBox* cmb = new QComboBox(parent);
        Q_FOREACH (mbClientDevice* dev, mbClient::global()->project()->devices())
            cmb->addItem(dev->name());
        return cmb;
    }
    case mbClientItemsModel::Column_Format:
    {
        QComboBox* cmb = new QComboBox(parent);
        QMetaEnum formats = mb::metaEnum<mb::Format>();
        for (int i = 0; i < formats.keyCount(); i++)
            cmb->addItem(formats.key(i));
        return cmb;
    }
    default:
        QWidget* w = QStyledItemDelegate::createEditor(parent, option, index);
        QStyledItemDelegate::setEditorData(w, index);
        return w;
    }

}

void mbClientItemsDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    switch (index.column())
    {
    case mbClientItemsModel::Column_Device:
    case mbClientItemsModel::Column_Format:
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        const QAbstractItemModel* model = index.model();
        QString s = model->data(index).toString();
        cmb->setCurrentText(s);
    }
        break;
    case mbClientItemsModel::Column_Value:
        break;
    default:
        QStyledItemDelegate::setEditorData(editor, index);
        break;
    }
}

void mbClientItemsDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    if (editor->inherits("QComboBox"))
    {
        QComboBox* cmb = static_cast<QComboBox*>(editor);
        model->setData(index, cmb->currentText());
    }
    else
        QStyledItemDelegate::setModelData(editor, model, index);
}

bool mbClientItemsDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
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
