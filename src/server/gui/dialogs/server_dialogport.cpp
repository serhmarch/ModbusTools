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
#include "server_dialogport.h"
#include "ui_server_dialogport.h"

#include <QMetaEnum>
#include <QIntValidator>

#include <server.h>
#include <project/server_port.h>

mbServerDialogPort::mbServerDialogPort(QWidget *parent) :
    mbCoreDialogPort(parent),
    ui(new Ui::mbServerDialogPort)
{
    ui->setupUi(this);

    m_ui.lnName            = ui->lnName           ;
    m_ui.cmbType           = ui->cmbType          ;
    m_ui.cmbSerialPortName = ui->cmbSerialPortName;
    m_ui.cmbBaudRate       = ui->cmbBaudRate      ;
    m_ui.cmbDataBits       = ui->cmbDataBits      ;
    m_ui.cmbParity         = ui->cmbParity        ;
    m_ui.cmbStopBits       = ui->cmbStopBits      ;
    m_ui.cmbFlowControl    = ui->cmbFlowControl   ;
    m_ui.spTimeoutFB       = ui->spTimeoutFB      ;
    m_ui.spTimeoutIB       = ui->spTimeoutIB      ;
    m_ui.spPort            = ui->spPort           ;
    m_ui.spTimeout         = ui->spTimeout        ;
    m_ui.stackedWidget     = ui->stackedWidget    ;
    m_ui.pgTCP             = ui->pgTCP            ;
    m_ui.pgSerial          = ui->pgSerial         ;
    m_ui.buttonBox         = ui->buttonBox        ;

    initializeBaseUi();
}

mbServerDialogPort::~mbServerDialogPort()
{
    delete ui;
}
