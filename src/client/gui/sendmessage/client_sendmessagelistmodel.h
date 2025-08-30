#ifndef CLIENT_SENDMESSAGELISTMODEL_H
#define CLIENT_SENDMESSAGELISTMODEL_H

#include <QAbstractTableModel>

#include "client_sendmessage_global.h"

class mbClientSendMessageListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Columns
    {
        Column_Func,
        Column_Param,
        ColumnCount
    };

public:
    explicit mbClientSendMessageListModel(QObject *parent = nullptr);
    ~mbClientSendMessageListModel();

public: // QAbstractItemModel interface
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QVariant data(const QModelIndex &index, int role) const override;

public:
    QList<const mbClientSendMessageParams*> messages() const;
    const mbClientSendMessageParams* message(int i) const;
    QString messageRepr(int i);
    void setMessages(const QList<const mbClientSendMessageParams*> messages);
    void insertMessage(int i, const mbClientSendMessageParams &params);
    inline void addMessage(const mbClientSendMessageParams &params) { insertMessage(-1, params); }
    void editMessage(int i, const mbClientSendMessageParams &params);
    void removeMessage(int i);
    bool moveUp(int i);
    bool moveDown(int i);
    void clear();

private:
    struct Item;
    bool moveTo(int oldPos, int newPos);
    QString paramsRepr(const mbClientSendMessageParams &params) const;

private:
    QList<Item*> m_items;
};

#endif // CLIENT_SENDMESSAGELISTMODEL_H
