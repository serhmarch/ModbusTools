/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#ifndef SERVER_DEVICEUIMODEL_H
#define SERVER_DEVICEUIMODEL_H

#include <QAbstractTableModel>

#include <mbcore.h>

class mbServerDevice;

class mbServerDeviceUiModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column
    {
        Column0,
        Column1,
        Column2,
        Column3,
        Column4,
        Column5,
        Column6,
        Column7,
        Column8,
        Column9,
        ColumnCount
    };

    struct Strings
    {
        const QString Column0;
        const QString Column1;
        const QString Column2;
        const QString Column3;
        const QString Column4;
        const QString Column5;
        const QString Column6;
        const QString Column7;
        const QString Column8;
        const QString Column9;
        const QString sym_0x;
        const QString sym_1x;
        const QString sym_3x;
        const QString sym_4x;


        Strings();
        static const Strings &instance();
    };

public:
    mbServerDeviceUiModel(mbServerDevice* device, QObject* parent = nullptr);

public:
    inline mbServerDevice* device() const { return m_device; }
    inline mb::DigitalFormat format() const { return m_format; }
    virtual void refresh();

public: // QAbstractItemModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

protected Q_SLOTS:
    void setRowCount(int count);
    void setFormat(int format);

protected:
    QString m_sym;

    mbServerDevice* m_device;
    int m_rowCount;
    mb::DigitalFormat m_format;
    uint m_changeCounter;
};

class mbServerDeviceUiModel_0x : public mbServerDeviceUiModel
{
public:
    mbServerDeviceUiModel_0x(mbServerDevice* device, QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void refresh() override;
};

class mbServerDeviceUiModel_1x : public mbServerDeviceUiModel
{
public:
    mbServerDeviceUiModel_1x(mbServerDevice* device, QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void refresh() override;
};

class mbServerDeviceUiModel_3x : public mbServerDeviceUiModel
{
public:
    mbServerDeviceUiModel_3x(mbServerDevice* device, QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void refresh() override;
};

class mbServerDeviceUiModel_4x : public mbServerDeviceUiModel
{
public:
    mbServerDeviceUiModel_4x(mbServerDevice* device, QObject* parent = nullptr);
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void refresh() override;
};


#endif // SERVER_DEVICEUIMODEL_H
