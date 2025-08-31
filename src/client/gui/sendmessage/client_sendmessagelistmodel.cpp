#include "client_sendmessagelistmodel.h"

struct mbClientSendMessageListModel::Item
{
    QString repr;
    mbClientSendMessageParams params;
};

mbClientSendMessageListModel::mbClientSendMessageListModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

mbClientSendMessageListModel::~mbClientSendMessageListModel()
{
    qDeleteAll(m_items);
}

int mbClientSendMessageListModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

int mbClientSendMessageListModel::rowCount(const QModelIndex &parent) const
{
    return m_items.count();
}

QVariant mbClientSendMessageListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (orientation)
        {
        case Qt::Vertical:
            return section+1;
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Func:
                return QStringLiteral("Func");
            case Column_Param:
                return QStringLiteral("Params");
            }
            break;
        }
        break;
    }
    return QVariant();
}

QVariant mbClientSendMessageListModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
    {
        Item *item = m_items.value(index.row());
        if (item)
        {
            switch (index.column())
            {
            case Column_Func:
                return QString::asprintf("%02d", item->params.func);
            case Column_Param:
                return item->repr;
            }
        }
    }
    break;
    }
    return QVariant();
}

QList<const mbClientSendMessageParams *> mbClientSendMessageListModel::messages() const
{
    QList<const mbClientSendMessageParams*> res;
    Q_FOREACH (const auto item, m_items)
        res.append(&item->params);
    return res;
}

const mbClientSendMessageParams *mbClientSendMessageListModel::message(int i) const
{
    Item *item = m_items.value(i);
    if (item)
        return &item->params;
    return nullptr;
}

QString mbClientSendMessageListModel::messageRepr(int i)
{
    Item *item = m_items.value(i);
    if (item)
        return item->repr;
    return QString();
}

void mbClientSendMessageListModel::setMessages(const QList<const mbClientSendMessageParams *> messages)
{
    beginResetModel();
    clear();
    endResetModel();
    Q_FOREACH(const mbClientSendMessageParams *p, messages)
    {
        addMessage(*p);
    }
}

void mbClientSendMessageListModel::insertMessage(int i, const mbClientSendMessageParams &params)
{
    if (i < 0 || i > m_items.count())
        i = m_items.count();
    Item *item = new Item{paramsRepr(params), params};
    beginInsertRows(QModelIndex(), i, i);
    m_items.insert(i, item);
    endInsertRows();
}

void mbClientSendMessageListModel::editMessage(int i, const mbClientSendMessageParams &params)
{
    Item *item = m_items.value(i);
    if (item)
    {
        item->params = params;
        item->repr = paramsRepr(params);
        QModelIndex b = createIndex(i, Column_Func);
        QModelIndex e = createIndex(i, Column_Param);
        Q_EMIT dataChanged(b, e);
    }
}

void mbClientSendMessageListModel::removeMessage(int i)
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

bool mbClientSendMessageListModel::moveUp(int i)
{
    if (i > 0)
        return moveTo(i, i-1);
    return false;
}

bool mbClientSendMessageListModel::moveDown(int i)
{
    if (i < m_items.count()-1)
        return moveTo(i, i+1);
    return false;
}

void mbClientSendMessageListModel::clear()
{
    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();
    endResetModel();
}

bool mbClientSendMessageListModel::moveTo(int oldPos, int newPos)
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

QString mbClientSendMessageListModel::paramsRepr(const mbClientSendMessageParams &params) const
{
    switch(params.func)
    {
    case MBF_READ_COILS:
        return QString("offset=%1,count=%2").arg(params.offset).arg(params.count);
    case MBF_READ_DISCRETE_INPUTS:
        return QString("offset=%1,count=%2").arg(params.offset).arg(params.count);
    case MBF_READ_HOLDING_REGISTERS:
        return QString("offset=%1,count=%2").arg(params.offset).arg(params.count);
    case MBF_READ_INPUT_REGISTERS:
        return QString("offset=%1,count=%2").arg(params.offset).arg(params.count);
    case MBF_WRITE_SINGLE_COIL:
        return QString("offset=%1,data=%2").arg(params.offset).arg(params.data);
    case MBF_WRITE_SINGLE_REGISTER:
        return QString("offset=%1,data=%2").arg(params.offset).arg(params.data);
    case MBF_READ_EXCEPTION_STATUS:
        return QString();
    case MBF_WRITE_MULTIPLE_COILS:
        return QString("offset=%1,count=%2,data=%3").arg(params.offset).arg(params.count).arg(params.data);
    case MBF_WRITE_MULTIPLE_REGISTERS:
        return QString("offset=%1,count=%2,data=%3").arg(params.offset).arg(params.count).arg(params.data);
    case MBF_REPORT_SERVER_ID:
        return QString();
    case MBF_MASK_WRITE_REGISTER:
        return QString("offset=%1,data=%2").arg(params.offset).arg(params.data);
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        return QString("readoffset=%1,readcount=%2,writeoffset=%3,writecount=%4,data=%5")
            .arg(params.offset).arg(params.count).arg(params.writeOffset).arg(params.writeCount).arg(params.data);
    default:
        return QString();
    }
}
