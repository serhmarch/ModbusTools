#ifndef CLIENT_ITEMSDELEGATE_H
#define CLIENT_ITEMSDELEGATE_H

#include <QStyledItemDelegate>

class mbClientItemsDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit mbClientItemsDelegate(QObject *parent = nullptr);

public:
    QWidget* createEditor (QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const override;
    void setEditorData (QWidget* editor, const QModelIndex& index ) const override;
    void setModelData (QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

Q_SIGNALS:
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);
};

#endif // CLIENT_ITEMSDELEGATE_H
