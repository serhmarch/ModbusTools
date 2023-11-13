#ifndef CLIENT_ITEMSMODEL_H
#define CLIENT_ITEMSMODEL_H

#include <QAbstractTableModel>
#include <QItemDelegate>

class mbClient;
class mbCoreProject;
class mbClientProject;
class mbClientItem;

class mbClientItemsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column
    {
        Column_Device,
        Column_Address,
        Column_Format,
        Column_Period,
        Column_Comment,
        Column_Status,
        Column_Timestamp,
        Column_Value,
        ColumnCount
    };

public:
    mbClientItemsModel(QObject* parent = nullptr);

public: // table model interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

public:
    QModelIndex itemIndex(mbClientItem *item) const;
    mbClientItem *item(const QModelIndex &index) const;

public Q_SLOTS:
    void changed();
    void valueChanged();

private Q_SLOTS:
    void setProject(mbCoreProject* project);
//    void setStatus(int status);
    void itemAdd(mbClientItem *d);
    void itemRemove(mbClientItem *d);

//private:
//    virtual void timerEvent(QTimerEvent *event);

private:
    mbClient* m_core;
    mbClientProject* m_project;
    //int m_timerID;
};

class mbDataDelegate : public QItemDelegate
{

public:
    mbDataDelegate(mbClient* core, QObject* parent = nullptr);

public:
    virtual QWidget* createEditor (QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    virtual void setEditorData (QWidget* editor, const QModelIndex& index ) const;
    virtual void setModelData (QWidget* editor, QAbstractItemModel* model, const QModelIndex& index ) const;

private:
    mbClient* m_core;
};
#endif // CLIENT_ITEMSMODEL_H
