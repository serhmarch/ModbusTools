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

    const Modbus::Defaults &d = Modbus::Defaults::instance();

    QSpinBox *sp;
    // Max conncetions
    sp = ui->spMaxConn;
    sp->setMinimum(1);
    sp->setMaximum(INT32_MAX);
    sp->setValue(d.maxconn);

    m_ui.lnName             = ui->lnName             ;
    m_ui.cmbType            = ui->cmbType            ;
    m_ui.cmbSerialPortName  = ui->cmbSerialPortName  ;
    m_ui.cmbBaudRate        = ui->cmbBaudRate        ;
    m_ui.cmbDataBits        = ui->cmbDataBits        ;
    m_ui.cmbParity          = ui->cmbParity          ;
    m_ui.cmbStopBits        = ui->cmbStopBits        ;
    m_ui.cmbFlowControl     = ui->cmbFlowControl     ;
    m_ui.spTimeoutFB        = ui->spTimeoutFB        ;
    m_ui.spTimeoutIB        = ui->spTimeoutIB        ;
    m_ui.spPort             = ui->spPort             ;
    m_ui.spTimeout          = ui->spTimeout          ;
    m_ui.chbBroadcastEnable = ui->chbBroadcastEnabled;
    m_ui.stackedWidget      = ui->stackedWidget      ;
    m_ui.pgTCP              = ui->pgTCP              ;
    m_ui.pgSerial           = ui->pgSerial           ;
    m_ui.buttonBox          = ui->buttonBox          ;

    initializeBaseUi();
}

mbServerDialogPort::~mbServerDialogPort()
{
    delete ui;
}

MBSETTINGS mbServerDialogPort::cachedSettings() const
{
    Modbus::Strings vs = Modbus::Strings::instance();
    const QString &prefix = Strings().cachePrefix;
    MBSETTINGS m = mbCoreDialogPort::cachedSettings();
    m[prefix+vs.maxconn] = ui->spMaxConn->value();
    return m;
}

void mbServerDialogPort::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogPort::setCachedSettings(m);

    Modbus::Strings vs = Modbus::Strings::instance();
    const QString &prefix = Strings().cachePrefix;
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(prefix+vs.maxconn); if (it != end) ui->spMaxConn->setValue(it.value().toInt());
}

void mbServerDialogPort::fillFormInner(const MBSETTINGS &settings)
{
    Modbus::Strings vs = Modbus::Strings::instance();
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    it = settings.find(vs.maxconn); if (it != end) ui->spMaxConn->setValue(it.value().toInt());
}

void mbServerDialogPort::fillDataInner(MBSETTINGS &settings) const
{
    Modbus::Strings vs = Modbus::Strings::instance();

    settings[vs.maxconn] = ui->spMaxConn->value();
}
