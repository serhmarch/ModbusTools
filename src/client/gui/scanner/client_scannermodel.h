#ifndef MBCLIENTSCANNERMODEL_H
#define MBCLIENTSCANNERMODEL_H

#include <QAbstractTableModel>

class mbClientScanner;

class mbClientScannerModel : public QAbstractTableModel
{
public:
    explicit mbClientScannerModel(mbClientScanner *scanner, QObject *parent = nullptr);

public: // table model interface
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

private Q_SLOTS:
    void deviceAdded(int index);
    void cleared();

private:
    mbClientScanner *m_scanner;
    QStringList m_devices;
};

#endif // MBCLIENTSCANNERMODEL_H
