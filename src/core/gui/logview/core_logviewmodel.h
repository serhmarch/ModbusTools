#ifndef XCHG_MESSAGEBUFFERMODEL_H
#define XCHG_MESSAGEBUFFERMODEL_H

#include <QDateTime>
#include <QAbstractTableModel>

#include <mbcore.h>

class mbCoreLogViewModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        Column_DateTime,
        Column_Source  ,
        Column_Category,
        Column_Text    ,
        ColumnCount
    };

    struct Message
    {
        QDateTime datetime;
        mb::LogFlag category;
        QString source;
        QString text;
    };

public:
    explicit mbCoreLogViewModel(QObject *parent = 0);
    ~mbCoreLogViewModel();

public:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &index = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    void logMessage(mb::LogFlag flag, const QString &source, const QString &text);
    void clear();

private:
    inline int getActualIndex(int row) const { return (m_ptr + m_buff.size() - m_count + row) % m_buff.size(); }

private:
    typedef QVector<Message> MessageBuffer;
    MessageBuffer m_buff;
    int m_ptr;
    int m_count;
};

#endif // XCHG_MESSAGEBUFFERMODEL_H
