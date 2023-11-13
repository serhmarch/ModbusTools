#include "client_itemsmodel.h"

#include <QComboBox>

#include <client.h>

#include <project/client_project.h>
#include <project/client_item.h>
#include <project/client_device.h>

#include "../client_ui.h"
#include "../client_undo.h"

#define REFRESH_msec 500

mbClientItemsModel::mbClientItemsModel(QObject* parent) :
    QAbstractTableModel(parent)
{
    m_project = nullptr;
    setProject(mbClient::global()->project());
//    m_timerID = -1;
    connect(mbClient::global(), SIGNAL(projectChanged(mbCoreProject*)), this, SLOT(setProject(mbCoreProject*)));
//    connect(core, SIGNAL(statusChanged(int)), this, SLOT(setStatus(int)));
}

QVariant mbClientItemsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
            switch(section)
            {
            case Column_Device   : return QStringLiteral("Device");
            case Column_Address  : return QStringLiteral("Address");
            case Column_Format   : return QStringLiteral("Format");
            case Column_Period   : return QStringLiteral("Period");
            case Column_Comment  : return QStringLiteral("Comment");
            case Column_Status   : return QStringLiteral("Status");
            case Column_Timestamp: return QStringLiteral("Timestamp");
            case Column_Value    : return QStringLiteral("Value");
            }
            break;
        case Qt::Vertical:
            return section+1;
        }
    }
    return QVariant();
}

int mbClientItemsModel::rowCount(const QModelIndex & /*parent*/) const
{
    if (m_project)
        return m_project->itemCount();
    return 0;
}

int mbClientItemsModel::columnCount(const QModelIndex & /*parent*/) const
{
    return ColumnCount;
}

QVariant mbClientItemsModel::data(const QModelIndex &index, int role) const
{
    int r = index.row();
    int c = index.column();
    if (m_project &&
        (r >= 0) && (r < rowCount()) &&
        (c >= 0) && (c < columnCount()) &&
        ((role == Qt::DisplayRole) || (role == Qt::EditRole)))
    {
        mbClientItem* d = m_project->itemAt(r);
        switch(c)
        {
        case Column_Device   : return d->deviceName();
        case Column_Address  : return d->addressStr();
        case Column_Format   : return d->formatStr();
        case Column_Period   : return d->period();
        case Column_Comment  : return d->comment();
        case Column_Status   : return mb::toString(d->status());
        case Column_Timestamp: return mb::toString(d->timestamp());
        case Column_Value    : return d->value();
        }
    }
    return QVariant();
}

bool mbClientItemsModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int r = index.row();
    int c = index.column();
    if (m_project &&
        (r >= 0) && (r < rowCount()) &&
        (c >= 0) && (c < columnCount()) &&
        (role == Qt::EditRole))
    {
        mbClientItem* d = m_project->itemAt(r);
        MBPARAMS cur, old;
        switch(c)
        {
        case Column_Device:
            old[mbClientItem::DeviceName] = d->deviceName();
            //d->setDeviceName(value.toString());
            cur[mbClientItem::DeviceName] = value.toString();
            break;
        case Column_Address:
            old[mbClientItem::Address] = d->addressInt();
            //d->setAddressStr(value.toString());
            cur[mbClientItem::Address] = value.toInt();
            break;
        case Column_Format:
            old[mbClientItem::Format] = d->format();
            //d->setFormatStr(value.toString());
            cur[mbClientItem::Format] = mb::enumValueTypeStr<mb::Format>(value.toString());
            break;
        case Column_Period:
            old[mbClientItem::Period] = d->period();
            //d->setPeriod(value.toInt());
            cur[mbClientItem::Period] = value.toInt();
            break;
        case Column_Comment:
            old[mbClientItem::Comment] = d->comment();
            //d->setComment(value.toString());
            cur[mbClientItem::Comment] = value.toString();
            break;
        case Column_Value:
            d->setValue(value.toString());
            return true;
        default:
            return false;
        }
        mbClientUndoEditData* cmd = new mbClientUndoEditData(d, cur, old);
        mbClient::global()->ui()->m_undoStack->push(cmd);
        return true;
    }
    return false;
}

Qt::ItemFlags mbClientItemsModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags f = QAbstractItemModel::flags(index);
    if (m_project)
    {
        int c = index.column();
        switch (c)
        {
        case Column_Device:
        case Column_Address:
        case Column_Format:
        case Column_Period:
        case Column_Comment:
            if (!mbClient::global()->isRunning()) // can be editable when it's no polling mode
                f |= Qt::ItemIsEditable;
            break;
        case Column_Value:
            if (mbClient::global()->isRunning()) // value can be editable only when it's polling mode
                f |= Qt::ItemIsEditable;
            break;
        }
    }
    return f;
}

QModelIndex mbClientItemsModel::itemIndex(mbClientItem *item) const
{
    if (m_project)
        return createIndex(m_project->itemIndex(item), 0);
    return QModelIndex();
}

mbClientItem *mbClientItemsModel::item(const QModelIndex &index) const
{
    if (m_project)
        return m_project->item(index.row());
    return nullptr;
}

void mbClientItemsModel::changed()
{
    int i = m_project->deviceIndex(static_cast<mbClientDevice*>(sender()));
    Q_EMIT dataChanged(createIndex(i, 0), createIndex(i, columnCount()-2));
}

void mbClientItemsModel::valueChanged()
{
    int i = m_project->deviceIndex(static_cast<mbClientDevice*>(sender()));
    Q_EMIT dataChanged(createIndex(i, Column_Value), createIndex(i, Column_Value));
}

void mbClientItemsModel::setProject(mbCoreProject* project)
{
    beginResetModel();
    if (m_project)
        m_project->disconnect(this);
    m_project = static_cast<mbClientProject*>(project);
    if (m_project)
    {
        connect(m_project, &mbClientProject::itemAdded   , this, &mbClientItemsModel::itemAdd);
        connect(m_project, &mbClientProject::itemRemoving, this, &mbClientItemsModel::itemRemove);
        Q_FOREACH (mbClientItem* d, m_project->items())
            itemAdd(d);
    }
    endResetModel();
}

//void mbClientItemsModel::setStatus(int status)
//{
//    beginResetModel();
//    switch (status)
//    {
//    case mbClient::Running:
//        m_timerID = startTimer(REFRESH_msec);
//        break;
//    default:
//        if (m_timerID >= 0)
//        {
//            killTimer(m_timerID);
//            m_timerID = -1;
//        }
//        break;
//    }
//    endResetModel();
//}

void mbClientItemsModel::itemAdd(mbClientItem * d)
{
    int i = m_project->itemIndex(d);
    beginInsertRows(QModelIndex(), i, i);
    connect(d, SIGNAL(changed()), this, SLOT(changed()));
    connect(d, SIGNAL(valueChanged()), this, SLOT(valueChanged()));
    endInsertRows();
}

void mbClientItemsModel::itemRemove(mbClientItem *d)
{
    int i = m_project->itemIndex(d);
    beginRemoveRows(QModelIndex(), i, i);
    d->disconnect(this);
    endRemoveRows();
}

//void mbClientItemsModel::timerEvent(QTimerEvent * /*event*/)
//{
//    // single timer, no need to use timer id
//    Q_EMIT dataChanged(createIndex(0, Column_Value), createIndex(rowCount()-1, Column_Value));
//}
