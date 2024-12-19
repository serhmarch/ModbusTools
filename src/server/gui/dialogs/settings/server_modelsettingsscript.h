#ifndef SERVER_MODELSETTINGSSCRIPT_H
#define SERVER_MODELSETTINGSSCRIPT_H

#include <QAbstractItemModel>

class mbServerModelSettingsScript : public QAbstractItemModel
{
public:
    explicit mbServerModelSettingsScript(QObject *parent = nullptr);

public:
    inline QModelIndex indexAuto  () const { return createIndex(0, 0, -1); }
    inline QModelIndex indexManual() const { return createIndex(1, 0, -1); }

    inline QModelIndex indexAutoChild  (int i) const { return createIndex(i, 0, static_cast<quintptr>(0)); }
    inline QModelIndex indexManualChild(int i) const { return createIndex(i, 0, static_cast<quintptr>(1)); }

    inline bool isAuto  (const QModelIndex &index) { return index.isValid() && index.internalId() == 0; }
    inline bool isManual(const QModelIndex &index) { return index.isValid() && index.internalId() == 1; }

public: // QAbstractItemModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public:
    inline QStringList autoDetected() const { return m_autoDetected; }
    inline QString autoDetected(int i) const { return m_autoDetected.value(i); }
    void setAutoDetected(const QStringList& exec);

    inline QStringList manual() const { return m_manual; }
    inline QString manual(int i) const { return m_manual.value(i); }
    void setManual(const QStringList& exec);
    void clearManual();

    inline QString scriptDefaultExecutable() const { return m_defaultExec; }
    void scriptSetDefaultExecutable(const QString exec);

    void scriptAddExecutable(const QString &exec);
    void scriptSetExecutable(const QModelIndex &index, const QString &exec);
    void scriptRemoveExecutable(const QModelIndex &index);

private:
    QStringList m_autoDetected;
    QStringList m_manual      ;
    QString     m_defaultExec ;
};

#endif // SERVER_MODELSETTINGSSCRIPT_H
