#include "server_delegatesettingsscripteditorcolors.h"

#include <QEvent>

#include <server.h>
#include <gui/server_ui.h>
#include <gui/dialogs/server_dialogs.h>

mbServerDelegateSettingsScriptEditorColors::mbServerDelegateSettingsScriptEditorColors(QObject *parent)
    : QStyledItemDelegate{parent}
{
}

bool mbServerDelegateSettingsScriptEditorColors::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    switch (event->type())
    {
    case QEvent::MouseButtonDblClick:
    {
        mbServerUi *ui = mbServer::global()->ui();
        QColor color1 = model->data(index, Qt::BackgroundRole).value<QColor>();
        QColor color2 = ui->dialogs()->getColor(color1, ui, "Color");
        if (color2.isValid())
            model->setData(index, QVariant::fromValue(color2), Qt::BackgroundRole);
    }
        return true;
    default:
        return QStyledItemDelegate::editorEvent(event, model, option, index);
    }
}
