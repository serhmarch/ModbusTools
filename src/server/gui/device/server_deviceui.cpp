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
#include "server_deviceui.h"
#include "ui_server_deviceui.h"

#include <QEvent>

#include <project/server_device.h>

#include "server_deviceuimodel.h"
#include "server_deviceuidelegate.h"

mbServerDeviceUi::mbServerDeviceUi(mbServerDevice *device, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbServerDeviceUi)
{
    ui->setupUi(this);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_timerId = 0;

    QTableView *tbl;

    m_device = device;
    connect(device, &mbServerDevice::nameChanged, this, &mbServerDeviceUi::deviceChanged);
    connect(device, &mbServerDevice::changed, this, &mbServerDeviceUi::deviceChanged);
    // memory models
    m_model_0x = new mbServerDeviceUiModel_0x(m_device, this);
    tbl = ui->tableView_0x;
    tbl->setModel(m_model_0x);
    tbl->setItemDelegate(new mbServerDeviceUiDelegateBool(this));
    tbl->setSelectionMode(QAbstractItemView::ContiguousSelection);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("QHeaderView::section { background-color:lightgray; color:black }");

    m_model_1x = new mbServerDeviceUiModel_1x(m_device, this);
    tbl = ui->tableView_1x;
    tbl->setModel(m_model_1x);
    tbl->setItemDelegate(new mbServerDeviceUiDelegateBool(this));
    tbl->setSelectionMode(QAbstractItemView::ContiguousSelection);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("QHeaderView::section { background-color:lightgray; color:black }");

    m_model_3x = new mbServerDeviceUiModel_3x(m_device, this);
    tbl = ui->tableView_3x;
    tbl->setModel(m_model_3x);
    tbl->setItemDelegate(new mbServerDeviceUiDelegate(this));
    tbl->setSelectionMode(QAbstractItemView::ContiguousSelection);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("QHeaderView::section { background-color:lightgray; color:black }");

    m_model_4x = new mbServerDeviceUiModel_4x(m_device, this);
    tbl = ui->tableView_4x;
    tbl->setModel(m_model_4x);
    tbl->setItemDelegate(new mbServerDeviceUiDelegate(this));
    tbl->setSelectionMode(QAbstractItemView::ContiguousSelection);
    tbl->setAlternatingRowColors(true);
    tbl->setStyleSheet("QHeaderView::section { background-color:lightgray; color:black }");

    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &mbServerDeviceUi::tabChanged);
}

mbServerDeviceUi::~mbServerDeviceUi()
{
    stopScanning();
}

QString mbServerDeviceUi::name() const
{
    return m_device->name();
}

Modbus::MemoryType mbServerDeviceUi::currentMemoryType() const
{
    if (ui->tabWidget->currentWidget() == ui->tab_0x) return Modbus::Memory_0x;
    if (ui->tabWidget->currentWidget() == ui->tab_1x) return Modbus::Memory_1x;
    if (ui->tabWidget->currentWidget() == ui->tab_3x) return Modbus::Memory_3x;
    return Modbus::Memory_4x;
}

QByteArray mbServerDeviceUi::getData(Modbus::MemoryType memoryType) const
{
    QByteArray data;
    switch (memoryType)
    {
    case Modbus::Memory_0x:
        data.resize(m_device->count_0x());
        m_device->read_0x_bool(0, data.count(), reinterpret_cast<bool*>(data.data()));
        break;
    case Modbus::Memory_1x:
        data.resize(m_device->count_1x());
        m_device->read_1x_bool(0, data.count(), reinterpret_cast<bool*>(data.data()));
        break;
    case Modbus::Memory_3x:
        data.resize(m_device->count_3x_bytes());
        m_device->read_3x(0, data.count() / MB_REGE_SZ_BYTES, reinterpret_cast<quint16*>(data.data()));
        break;
    default:
        data.resize(m_device->count_4x_bytes());
        m_device->read_4x(0, data.count() / MB_REGE_SZ_BYTES, reinterpret_cast<quint16*>(data.data()));
        break;
    }
    return data;
}

void mbServerDeviceUi::setData(Modbus::MemoryType memoryType, const QByteArray &data)
{
    switch (memoryType)
    {
    case Modbus::Memory_0x:
        m_device->write_0x_bool(0, data.count(), reinterpret_cast<const bool*>(data.data()));
        break;
    case Modbus::Memory_1x:
        m_device->write_1x_bool(0, data.count(), reinterpret_cast<const bool*>(data.data()));
        break;
    case Modbus::Memory_3x:
        m_device->write_3x(0, data.count() / MB_REGE_SZ_BYTES, reinterpret_cast<const quint16*>(data.data()));
        break;
    default:
        m_device->write_4x(0, data.count() / MB_REGE_SZ_BYTES, reinterpret_cast<const quint16*>(data.data()));
        break;
    }
}

void mbServerDeviceUi::slotMemoryZerro()
{
    if      (ui->tabWidget->currentWidget() == ui->tab_0x)
        m_device->zerroAll_0x();
    else if (ui->tabWidget->currentWidget() == ui->tab_1x)
        m_device->zerroAll_1x();
    else if (ui->tabWidget->currentWidget() == ui->tab_3x)
        m_device->zerroAll_3x();
    else
        m_device->zerroAll_4x();
}

void mbServerDeviceUi::slotMemoryZerroAll()
{
    m_device->zerroAll_0x();
    m_device->zerroAll_1x();
    m_device->zerroAll_3x();
    m_device->zerroAll_4x();
}

void mbServerDeviceUi::deviceChanged()
{
    Q_EMIT nameChanged(this->name());
}

void mbServerDeviceUi::tabChanged(int)
{
    stopScanning();
    startScanning(500);
}

bool mbServerDeviceUi::event(QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        startScanning(500);
    }
    else if (event->type() == QEvent::Hide)
    {
        stopScanning();
    }
    else if (event->type() == QEvent::Timer)
    {
        refreshData();
    }
    return QWidget::event(event);
}

void mbServerDeviceUi::startScanning(int period)
{
    if (!isScanning())
    {
        m_timerId = startTimer(period);
    }
}

void mbServerDeviceUi::stopScanning()
{
    if (isScanning())
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}

void mbServerDeviceUi::refreshData()
{
    QWidget *tab = ui->tabWidget->currentWidget();
    if (tab == ui->tab_0x)
        m_model_0x->refresh();
    else if (tab == ui->tab_1x)
        m_model_1x->refresh();
    else if (tab == ui->tab_3x)
        m_model_3x->refresh();
    else if (tab == ui->tab_4x)
        m_model_4x->refresh();
}


