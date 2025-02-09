#include "server_modelsettingsscriptinterpreters.h"

#include <QPalette>
#include <QIcon>
#include <QApplication>

mbServerModelSettingsScriptInterpreters::mbServerModelSettingsScriptInterpreters(QObject *parent)
    : QAbstractItemModel{parent}
{

}

QVariant mbServerModelSettingsScriptInterpreters::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
            return "Path";
    }
    return QVariant();
}

QModelIndex mbServerModelSettingsScriptInterpreters::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    // Top-level items: "Auto Detected" or "Manual"
    if (!parent.isValid())
        return createIndex(row, column, -1); // Top-level items have parent ID -1

    // Sub-items: Return the child item index based on parent row (0 or 1)
    if (parent.internalId() == -1)
        return createIndex(row, column, parent.row());

    return QModelIndex();
}

QModelIndex mbServerModelSettingsScriptInterpreters::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    int parentId = static_cast<int>(index.internalId());
    if (parentId == -1) // Top-level items have no parent
        return QModelIndex();

    return createIndex(parentId, 0, -1); // Parent is a top-level item
}

int mbServerModelSettingsScriptInterpreters::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        // Top-level items
        return 2;

    if (parent.internalId() == -1)
    {
        // Sub-items
        if (parent.row() == 0) // "Auto Detected"
            return m_autoDetected.size();
        if (parent.row() == 1) // "Manual"
            return m_manual.size();
    }

    return 0;
}

int mbServerModelSettingsScriptInterpreters::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1; // Single column model
}

QVariant mbServerModelSettingsScriptInterpreters::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
        if (index.internalId() == -1)
        {
            // Top-level items
            if (index.row() == 0)
                return "Auto Detected";
            if (index.row() == 1)
                return "Manual";
        }
        else
        {
            // Sub-items
            if (index.internalId() == 0) // "Auto Detected" children
                return m_autoDetected.value(index.row());
            if (index.internalId() == 1) // "Manual" children
                return m_manual.value(index.row());
        }
        break;
    case Qt::DecorationRole:
        if (index.internalId() != -1)
        {
            // Sub-items
            if (index.internalId() == 0) // "Auto Detected" children
                return m_autoDetected.value(index.row()) == m_defaultExec ? QIcon(":/server/icons/greenmark.ico") : QVariant();
            if (index.internalId() == 1) // "Manual" children
                return m_manual.value(index.row()) == m_defaultExec ? QIcon(":/server/icons/greenmark.ico") : QVariant();
        }
        break;
    case Qt::BackgroundRole:
        if (index.internalId() != -1)
        {
            // Sub-items
            if (index.internalId() == 0) // "Auto Detected" children
                return m_autoDetected.value(index.row()) == m_defaultExec ? QApplication::palette().window().color().darker(150) : QVariant();
            if (index.internalId() == 1) // "Manual" children
                return m_manual.value(index.row()) == m_defaultExec ? QApplication::palette().window().color().darker(150) : QVariant();
        }
        break;
    }


    return QVariant();
}

void mbServerModelSettingsScriptInterpreters::setAutoDetected(const QStringList &exec)
{
    beginResetModel();
    m_autoDetected = exec;
    endResetModel();
}

void mbServerModelSettingsScriptInterpreters::setManual(const QStringList &exec)
{
    clearManual();
    if (exec.count())
    {
        beginInsertRows(indexManual(), 0, exec.count()-1);
        m_manual = exec;
        endInsertRows();
    }
}

void mbServerModelSettingsScriptInterpreters::clearManual()
{
    if (m_manual.count())
    {
        beginRemoveRows(indexManual(), 0, m_manual.count()-1);
        m_manual.clear();
        endRemoveRows();
    }
}

void mbServerModelSettingsScriptInterpreters::scriptSetDefaultExecutable(const QString exec)
{
    m_defaultExec = exec;
    Q_EMIT dataChanged(indexAutoChild  (0), indexAutoChild  (m_autoDetected.count()-1));
    Q_EMIT dataChanged(indexManualChild(0), indexManualChild(m_manual      .count()-1));
}

void mbServerModelSettingsScriptInterpreters::scriptAddExecutable(const QString &exec)
{
    int c = m_manual.count();
    beginInsertRows(indexManual(), c, c);
    m_manual.append(exec);
    endInsertRows();
}

void mbServerModelSettingsScriptInterpreters::scriptSetExecutable(const QModelIndex &index, const QString &exec)
{
    if (exec.count())
    {
        QModelIndex parent = index.parent();
        QModelIndex manual = indexManual();
        if (parent == manual)
        {
            m_manual[index.row()] = exec;
            Q_EMIT dataChanged(index, index);
        }
    }
}

void mbServerModelSettingsScriptInterpreters::scriptRemoveExecutable(const QModelIndex &index)
{
    QModelIndex parent = index.parent();
    QModelIndex manual = indexManual();
    if (parent == manual)
    {
        beginRemoveRows(parent, index.row(), index.row());
        m_manual.removeAt(index.row());
        endRemoveRows();
    }
}

