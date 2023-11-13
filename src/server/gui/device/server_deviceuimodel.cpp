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
#include "server_deviceuimodel.h"

#include <server.h>
#include <gui/server_ui.h>

#include <project/server_device.h>

mbServerDeviceUiModel::Strings::Strings() :
    Column0(QStringLiteral("+0")),
    Column1(QStringLiteral("+1")),
    Column2(QStringLiteral("+2")),
    Column3(QStringLiteral("+3")),
    Column4(QStringLiteral("+4")),
    Column5(QStringLiteral("+5")),
    Column6(QStringLiteral("+6")),
    Column7(QStringLiteral("+7")),
    Column8(QStringLiteral("+8")),
    Column9(QStringLiteral("+9")),
    sym_0x(QStringLiteral("0")),
    sym_1x(QStringLiteral("1")),
    sym_3x(QStringLiteral("3")),
    sym_4x(QStringLiteral("4"))
{
}

const mbServerDeviceUiModel::Strings &mbServerDeviceUiModel::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDeviceUiModel::mbServerDeviceUiModel(mbServerDevice *device, QObject *parent) :
    QAbstractTableModel(parent),
    m_device(device)
{
    m_rowCount = 0;
    m_changeCounter = 0;
    m_format = mb::DefaultDigitalFormat;
}

void mbServerDeviceUiModel::refresh()
{
    Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
}

QVariant mbServerDeviceUiModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole)
    {
        switch (orientation)
        {
        case Qt::Horizontal:
        {
            Strings s = Strings::instance();
            switch (section)
            {
            case Column0: return s.Column0;
            case Column1: return s.Column1;
            case Column2: return s.Column2;
            case Column3: return s.Column3;
            case Column4: return s.Column4;
            case Column5: return s.Column5;
            case Column6: return s.Column6;
            case Column7: return s.Column7;
            case Column8: return s.Column8;
            case Column9: return s.Column9;
            }
        }
        break;
        case Qt::Vertical:
            return QString("%1%2").arg(m_sym).arg(section*ColumnCount+1, 5, 10, QChar('0'));
        }
    }
    return QVariant();
}

int mbServerDeviceUiModel::columnCount(const QModelIndex &/*parent*/) const
{
    return ColumnCount;
}

int mbServerDeviceUiModel::rowCount(const QModelIndex &/*parent*/) const
{
    return m_rowCount;
}

void mbServerDeviceUiModel::setRowCount(int count)
{
    beginResetModel();
    m_rowCount = (count+ColumnCount-1)/ColumnCount;
    endResetModel();
}

void mbServerDeviceUiModel::setFormat(int format)
{
    if (m_format != format)
    {
        m_format = static_cast<mb::DigitalFormat>(format);
        Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
    }
}

mbServerDeviceUiModel_0x::mbServerDeviceUiModel_0x(mbServerDevice *device, QObject *parent) :
    mbServerDeviceUiModel(device, parent)
{
    m_sym = Strings::instance().sym_0x;
    connect(device, SIGNAL(count_0x_changed(int)), this, SLOT(setRowCount(int)));
    setRowCount(device->count_0x());
}

QVariant mbServerDeviceUiModel_0x::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_0x())
            return static_cast<int>(m_device->bool_0x(static_cast<quint16>(offset)));
    }
        break;
    case Qt::BackgroundRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_0x())
            return m_device->bool_0x(static_cast<quint16>(offset)) ? QBrush(Qt::gray) : QBrush(Qt::white);
    }
        break;
    }
    return QVariant();
}

bool mbServerDeviceUiModel_0x::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_0x())
        {
            m_device->setBool_0x(static_cast<quint16>(offset), value.toBool());
            return true;
        }
    }
    break;
    }
    return false;
}

Qt::ItemFlags mbServerDeviceUiModel_0x::flags(const QModelIndex &index) const
{
    //int offset = index.row()*ColumnCount+index.column();
    //if (offset < m_device->count_0x())
    //    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

void mbServerDeviceUiModel_0x::refresh()
{
    uint c = m_device->changeCounter_0x();
    if (m_changeCounter != c)
    {
        Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
        m_changeCounter = c;
    }
}

mbServerDeviceUiModel_1x::mbServerDeviceUiModel_1x(mbServerDevice *device, QObject *parent) :
    mbServerDeviceUiModel(device, parent)
{
    m_sym = Strings::instance().sym_1x;
    connect(device, SIGNAL(count_1x_changed(int)), this, SLOT(setRowCount(int)));
    setRowCount(device->count_1x());
}

QVariant mbServerDeviceUiModel_1x::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_1x())
            return static_cast<int>(m_device->bool_1x(static_cast<quint16>(offset)));
    }
        break;
    case Qt::BackgroundRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_1x())
            return m_device->bool_1x(static_cast<quint16>(offset)) ? QBrush(Qt::gray) : QBrush(Qt::white);
    }
        break;
    }
    return QVariant();
}

bool mbServerDeviceUiModel_1x::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_1x())
        {
            m_device->setBool_1x(static_cast<quint16>(offset), value.toBool());
            return true;
        }
    }
    break;
    }
    return false;
}

Qt::ItemFlags mbServerDeviceUiModel_1x::flags(const QModelIndex &index) const
{
    //int offset = index.row()*ColumnCount+index.column();
    //if (offset < m_device->count_1x())
    //    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

void mbServerDeviceUiModel_1x::refresh()
{
    uint c = m_device->changeCounter_1x();
    if (m_changeCounter != c)
    {
        Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
        m_changeCounter = c;
    }
}

mbServerDeviceUiModel_3x::mbServerDeviceUiModel_3x(mbServerDevice *device, QObject *parent) :
    mbServerDeviceUiModel(device, parent)
{
    m_sym = Strings::instance().sym_3x;
    connect(device, SIGNAL(count_3x_changed(int)), this, SLOT(setRowCount(int)));
    setRowCount(device->count_3x());

    mbServerUi *ui = mbServer::global()->ui();
    connect(ui, &mbServerUi::formatChanged, this, &mbServerDeviceUiModel_3x::setFormat);
    m_format = ui->format();
}

QVariant mbServerDeviceUiModel_3x::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_3x())
        {
            switch (m_format)
            {
            case mb::Bin:
                return mb::toBinString(m_device->uint16_3x(static_cast<quint16>(offset)));
            case mb::Oct:
                return mb::toOctString(m_device->uint16_3x(static_cast<quint16>(offset)));
            case mb::Hex:
                return mb::toHexString(m_device->uint16_3x(static_cast<quint16>(offset)));
            case mb::UDec:
                return m_device->uint16_3x(static_cast<quint16>(offset));
            default:
                return m_device->int16_3x(static_cast<quint16>(offset));
            }
        }
    }
    break;
    }
    return QVariant();
}

bool mbServerDeviceUiModel_3x::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_3x())
        {
            bool ok = true;
            quint16 v;
            switch (m_format)
            {
            case mb::Bin:
                v = value.toString().toUShort(&ok, 2);
                break;
            case mb::Oct:
                v = value.toString().toUShort(&ok, 8);
                break;
            case mb::Hex:
                v = value.toString().toUShort(&ok, 16);
                break;
            case mb::UDec:
                v = value.toString().toUShort(&ok, 10);
                break;
            default:
                *reinterpret_cast<qint16*>(&v) = value.toString().toShort(&ok, 10);
                break;
            }
            if (ok)
                m_device->setUInt16_3x(static_cast<quint16>(offset), v);
            return ok;
        }
    }
    break;
    }
    return false;
}

Qt::ItemFlags mbServerDeviceUiModel_3x::flags(const QModelIndex &index) const
{
    int offset = index.row()*ColumnCount+index.column();
    if (offset < m_device->count_3x())
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

void mbServerDeviceUiModel_3x::refresh()
{
    uint c = m_device->changeCounter_3x();
    if (m_changeCounter != c)
    {
        Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
        m_changeCounter = c;
    }
}

mbServerDeviceUiModel_4x::mbServerDeviceUiModel_4x(mbServerDevice *device, QObject *parent) :
    mbServerDeviceUiModel(device, parent)
{
    m_sym = Strings::instance().sym_4x;
    connect(device, SIGNAL(count_4x_changed(int)), this, SLOT(setRowCount(int)));
    setRowCount(device->count_4x());

    mbServerUi *ui = mbServer::global()->ui();
    connect(ui, &mbServerUi::formatChanged, this, &mbServerDeviceUiModel_4x::setFormat);
    m_format = ui->format();
}

QVariant mbServerDeviceUiModel_4x::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_4x())
        {
            switch (m_format)
            {
            case mb::Bin:
                return mb::toBinString(m_device->uint16_4x(static_cast<quint16>(offset)));
            case mb::Oct:
                return mb::toOctString(m_device->uint16_4x(static_cast<quint16>(offset)));
            case mb::Hex:
                return mb::toHexString(m_device->uint16_4x(static_cast<quint16>(offset)));
            case mb::UDec:
                return m_device->uint16_4x(static_cast<quint16>(offset));
            default:
                return m_device->int16_4x(static_cast<quint16>(offset));
            }
        }
    }
    break;
    }
    return QVariant();
}

bool mbServerDeviceUiModel_4x::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::EditRole:
    {
        int offset = index.row()*ColumnCount+index.column();
        if (offset < m_device->count_4x())
        {
            bool ok = true;
            quint16 v;
            switch (m_format)
            {
            case mb::Bin:
                v = value.toString().toUShort(&ok, 2);
                break;
            case mb::Oct:
                v = value.toString().toUShort(&ok, 8);
                break;
            case mb::Hex:
                v = value.toString().toUShort(&ok, 16);
                break;
            case mb::UDec:
                v = value.toString().toUShort(&ok, 10);
                break;
            default:
                *reinterpret_cast<qint16*>(&v) = value.toString().toShort(&ok, 10);
                break;
            }
            if (ok)
                m_device->setUInt16_4x(static_cast<quint16>(offset), v);
            return ok;
        }
    }
    break;
    }
    return false;
}

Qt::ItemFlags mbServerDeviceUiModel_4x::flags(const QModelIndex &index) const
{
    int offset = index.row()*ColumnCount+index.column();
    if (offset < m_device->count_4x())
        return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
    return QAbstractTableModel::flags(index);
}

void mbServerDeviceUiModel_4x::refresh()
{
    uint c = m_device->changeCounter_4x();
    if (m_changeCounter != c)
    {
        Q_EMIT dataChanged(index(0, 0), index(rowCount()-1, columnCount()-1));
        m_changeCounter = c;
    }
}

