#ifndef CLIENT_SENDBYTESLISTMODEL_H
#define CLIENT_SENDBYTESLISTMODEL_H

#include <QAbstractListModel>

#include <client_global.h>

class mbClientSendBytesListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit mbClientSendBytesListModel(QObject *parent = nullptr);
    ~mbClientSendBytesListModel();

public: // QAbstractItemModel interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role) const;

public:
    mbClientRunMessageRaw* createMessage(const QByteArray &bytes) const;
    QList<mbClientRunMessageRawPtr> messages() const;
    mbClientRunMessageRawPtr message(int i) const;
    QString messageRepr(int i);
    void insertMessage(int i, const QByteArray &bytes);
    inline void addMessage(const QByteArray &bytes) { insertMessage(-1, bytes); }
    void editMessage(int i, const QByteArray &bytes);
    void removeMessage(int i);
    bool moveUp(int i);
    bool moveDown(int i);
    void clear();

private:
    struct Item;
    bool moveTo(int oldPos, int newPos);
    void fillMessage(mbClientRunMessageRaw* msg, const QByteArray &bytes) const;
    QString bytesRepr(void *buff, int len) const;

private:
    QList<Item*> m_items;
};

#endif // CLIENT_SENDBYTESLISTMODEL_H
