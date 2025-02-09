#include "core_logviewmodel.h"

#include <QColor>
#include <QDateTime>

mbCoreLogViewModel::mbCoreLogViewModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_ptr = 0;
    m_count = 0;
    m_buff.resize(1000);
}

mbCoreLogViewModel::~mbCoreLogViewModel()
{
}

QVariant mbCoreLogViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (orientation == Qt::Horizontal)
        {
            switch(section)
            {
            case Column_DateTime: return "DateTime";
            case Column_Source  : return "Source"  ;
            case Column_Category: return "Category";
            case Column_Text    : return "Text"    ;
            }
        }
        else
            return section;
    }
    return QVariant();
}

int mbCoreLogViewModel::rowCount(const QModelIndex &/*index*/) const
{
    return m_count;
}

int mbCoreLogViewModel::columnCount(const QModelIndex &) const
{
    return ColumnCount;
}

QVariant mbCoreLogViewModel::data(const QModelIndex &index, int role) const
{
    int c = index.column();
    int r = index.row();
    if (r < m_count)
    {
        int i = getActualIndex(r);
        switch (role)
        {
        case Qt::DisplayRole:
            switch(c)
            {
            case Column_DateTime: return m_buff.at(i).datetime.toString("hh:mm:ss.zzz");
            case Column_Source  : return m_buff.at(i).source;
            case Column_Category: return mb::toString(m_buff.at(i).category);
            case Column_Text    : return m_buff.at(i).text;
            }
            break;
        case Qt::BackgroundRole:
            switch (m_buff.at(i).category)
            {
            case mb::Log_Error  : return QColor(Qt::red);
            case mb::Log_Warning: return QColor(Qt::yellow);
            case mb::Log_Info   : return QColor(Qt::green);
            case mb::Log_Tx     : return QColor(Qt::lightGray);
            case mb::Log_Rx     : return QColor(Qt::lightGray);
            default:
                return QVariant();
            }
        case Qt::ForegroundRole:
            switch (m_buff.at(i).category)
            {
            case mb::Log_Error  : return QColor(Qt::white);
            case mb::Log_Warning: return QColor(Qt::white);
            case mb::Log_Info   : return QColor(Qt::black);
            case mb::Log_Tx     : return QColor(Qt::black);
            case mb::Log_Rx     : return QColor(Qt::black);
            default:
                return QVariant();
            }
        }
    }
    return QVariant();
}

void mbCoreLogViewModel::logMessage(mb::LogFlag flag, const QString &source, const QString &text)
{
    int sz = m_buff.size();
    Message message;
    message.datetime = QDateTime::currentDateTime();
    message.category = flag;
    message.source = source;
    message.text = text;
    if (m_count < sz)
    {
        beginInsertRows(QModelIndex(), m_count, m_count);
        m_buff[m_ptr] = message;
        m_count++;
        endInsertRows();
    }
    else
    {
        beginResetModel();
        m_buff[m_ptr] = message;
        endResetModel();
        //beginRemoveRows(QModelIndex(), 0, 0);
        //endRemoveRows();
        //beginInsertRows(QModelIndex(), m_count-1, m_count-1);
        //endInsertRows();
    }
    m_ptr = (m_ptr + 1) % sz;
}

void mbCoreLogViewModel::clear()
{
    beginResetModel();
    m_count = 0;
    endResetModel();
}

