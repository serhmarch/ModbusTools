#include "client_sendbyteslistmodel.h"

#include <runtime/client_runmessage.h>

struct mbClientSendBytesListModel::Item
{
    QString repr;
    mbClientRunMessageRawPtr message;
};

mbClientSendBytesListModel::mbClientSendBytesListModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

mbClientSendBytesListModel::~mbClientSendBytesListModel()
{
    qDeleteAll(m_items);
}

int mbClientSendBytesListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_items.count();
}

QVariant mbClientSendBytesListModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        Item *item = m_items.value(index.row());
        if (item)
        {
            return item->repr;
        }
    }
        break;
    }
    return QVariant();
}

mbClientRunMessageRaw *mbClientSendBytesListModel::createMessage(const QByteArray &bytes) const
{
    mbClientRunMessageRaw *msg = new mbClientRunMessageRaw();
    fillMessage(msg, bytes);
    return msg;
}

QList<mbClientRunMessageRawPtr> mbClientSendBytesListModel::messages() const
{
    QList<mbClientRunMessageRawPtr> res;
    Q_FOREACH (auto item, m_items)
        res.append(item->message);
    return res;
}

mbClientRunMessageRawPtr mbClientSendBytesListModel::message(int i) const
{
    Item *item = m_items.value(i);
    if (item)
        return item->message;
    return nullptr;
}

QString mbClientSendBytesListModel::messageRepr(int i)
{
    Item *item = m_items.value(i);
    if (item)
        return item->repr;
    return QString();
}

void mbClientSendBytesListModel::insertMessage(int i, const QByteArray &bytes)
{
    mbClientRunMessageRaw *msg = createMessage(bytes);
    if (i < 0 || i > m_items.count())
        i = m_items.count();
    Item *item = new Item{bytesRepr(msg->inputBuffer(), msg->inputBaseCount()), msg};
    beginInsertRows(QModelIndex(), i, i);
    m_items.insert(i, item);
    endInsertRows();
}

void mbClientSendBytesListModel::editMessage(int i, const QByteArray &bytes)
{
    Item *item = m_items.value(i);
    if (item)
    {
        mbClientRunMessageRaw *msg = item->message;
        fillMessage(msg, bytes);
        item->repr = bytesRepr(msg->inputBuffer(), msg->inputBaseCount());
        QModelIndex index = createIndex(i, 0);
        Q_EMIT dataChanged(index, index);
    }
}

void mbClientSendBytesListModel::removeMessage(int i)
{
    Item *item = m_items.value(i);
    if (item)
    {
        beginRemoveRows(QModelIndex(), i, i);
        m_items.removeAt(i);
        delete item;
        endRemoveRows();
    }
}

bool mbClientSendBytesListModel::moveUp(int i)
{
    if (i > 0)
        return moveTo(i, i-1);
    return false;
}

bool mbClientSendBytesListModel::moveDown(int i)
{
    if (i < m_items.count()-1)
        return moveTo(i, i+1);
    return false;
}

void mbClientSendBytesListModel::clear()
{
    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();
    endResetModel();
}

bool mbClientSendBytesListModel::moveTo(int oldPos, int newPos)
{
    Item *item = m_items.value(oldPos);
    if (item)
    {
        beginRemoveRows(QModelIndex(), oldPos, oldPos);
        m_items.removeAt(oldPos);
        endRemoveRows();
        beginInsertRows(QModelIndex(), newPos, newPos);
        m_items.insert(newPos, item);
        endInsertRows();
        return true;
    }
    return false;
}

void mbClientSendBytesListModel::fillMessage(mbClientRunMessageRaw *msg, const QByteArray &bytes) const
{
    // TODO: check input buff bounds
    memcpy(msg->inputBuffer(), bytes.constData(), bytes.length());
    msg->setInputBaseCount(bytes.length());
}

QString mbClientSendBytesListModel::bytesRepr(void *buffer, int len) const
{
    QString s;
    if (len > 0)
    {
        uint8_t *buff = reinterpret_cast<uint8_t*>(buffer);
        s = QString::asprintf("%02X", buff[0]);
        for (int i = 1; i < len; i++)
            s += ' ' + QString::asprintf("%02X", buff[i]);
    }
    return s;
}
