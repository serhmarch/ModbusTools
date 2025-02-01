#ifndef MBSERVERDELEGATESETTINGSSCRIPTEDITORCOLORS_H
#define MBSERVERDELEGATESETTINGSSCRIPTEDITORCOLORS_H

#include <QStyledItemDelegate>

class mbServerDelegateSettingsScriptEditorColors : public QStyledItemDelegate
{
public:
    explicit mbServerDelegateSettingsScriptEditorColors(QObject *parent = nullptr);

public:
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;
};

#endif // MBSERVERDELEGATESETTINGSSCRIPTEDITORCOLORS_H
