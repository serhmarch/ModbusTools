#include "client_scannermodel.h"

#include "client_scanner.h"

mbClientScannerModel::mbClientScannerModel(mbClientScanner *scanner, QObject *parent)
    : QAbstractTableModel{parent}
{
    m_scanner = scanner;
    connect(m_scanner, &mbClientScanner::deviceAdded, this, &mbClientScannerModel::deviceAdded);
    connect(m_scanner, &mbClientScanner::cleared, this, &mbClientScannerModel::cleared);
}

int mbClientScannerModel::rowCount(const QModelIndex &) const
{
    return m_devices.count();
}

int mbClientScannerModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QVariant mbClientScannerModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
        return m_devices.value(index.row());
    return QVariant();
}

void mbClientScannerModel::deviceAdded(int index)
{
    beginInsertRows(QModelIndex(), index, index);
    m_devices.append(m_scanner->deviceInfoStr(index));
    endInsertRows();
}

void mbClientScannerModel::cleared()
{
    beginResetModel();
    m_devices.clear();
    endResetModel();
}
