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

mbServerDialogPort::Strings::Strings() :
    title(QStringLiteral("Port"))
{
}

const mbServerDialogPort::Strings &mbServerDialogPort::Strings::instance()
{
    static const Strings s;
    return s;
}


mbServerDialogPort::mbServerDialogPort(QWidget *parent) :
    mbCoreDialogPort(parent),
    ui(new Ui::mbServerDialogPort)
{
    ui->setupUi(this);

    Modbus::Defaults d = Modbus::Defaults::instance();
    mbServerPort::Defaults pd = mbServerPort::Defaults::instance();

    //QLineEdit* ln;
    QSpinBox* sp;
    QComboBox* cmb;

    // Type
    cmb = ui->cmbType;
    cmb->addItem(Modbus::toString(Modbus::ASC));
    cmb->addItem(Modbus::toString(Modbus::RTU));
    cmb->addItem(Modbus::toString(Modbus::TCP));
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));
    ui->stackedWidget->setCurrentWidget(ui->pgTCP);
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));

    //--------------------- SERIAL ---------------------
    // Serial Port
    cmb = ui->cmbSerialPortName;
    QStringList ports = Modbus::availableSerialPortList();
    Q_FOREACH(const QString &port, ports)
        cmb->addItem(port);

    // Baud Rate
    cmb = ui->cmbBaudRate;
    Modbus::List<int32_t> baudRates = Modbus::availableBaudRate();
    for (int32_t v : baudRates)
        cmb->addItem(QString::number(v));
    cmb->setCurrentText(QString::number(d.baudRate));

    // Data Bits
    cmb = ui->cmbDataBits;
    Modbus::List<int8_t> dataBits = Modbus::availableDataBits();
    for (int8_t v : dataBits)
        cmb->addItem(QString::number(v));
    cmb->setCurrentText(QString::number(d.dataBits));

    // Parity
    cmb = ui->cmbParity;
    Modbus::List<Modbus::Parity> parity = Modbus::availableParity();
    for (Modbus::Parity v : parity)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.parity));

    // Stop Bits
    cmb = ui->cmbStopBits;
    Modbus::List<Modbus::StopBits> stopBits = Modbus::availableStopBits();
    for (Modbus::StopBits v : stopBits)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.stopBits));

    // Flow Control
    cmb = ui->cmbFlowControl;
    Modbus::List<Modbus::FlowControl> flowControl = Modbus::availableFlowControl();
    for (Modbus::FlowControl v : flowControl)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.flowControl));

    // Timeout first byte
    sp = ui->spTimeoutFB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeoutFirstByte); // default slave address
    // Timeout first byte
    sp = ui->spTimeoutIB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeoutInterByte); // default slave address

    //--------------------- TCP ---------------------
    // Port
    sp = ui->spPort;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(d.port);
    // Timeout
    sp = ui->spTimeout;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeout);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbServerDialogPort::~mbServerDialogPort()
{
    delete ui;
}

MBSETTINGS mbServerDialogPort::getSettings(const MBSETTINGS &settings, const QString &title)
{
    Modbus::Settings r;

    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    if (settings.count())
        fillForm(settings);
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbServerDialogPort::fillForm(const Modbus::Settings &m)
{
    mbServerPort::Strings ms = mbServerPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    ui->lnName->setText(m.value(ms.name).toString());
    ui->cmbType->setCurrentText(m.value(ms.type).toString());
    //--------------------- SERIAL ---------------------
    ui->cmbSerialPortName->setCurrentText(m.value(ss.serialPortName  ).toString());
    ui->cmbBaudRate      ->setCurrentText(m.value(ss.baudRate        ).toString());
    ui->cmbDataBits      ->setCurrentText(m.value(ss.dataBits        ).toString());
    ui->cmbParity        ->setCurrentText(m.value(ss.parity          ).toString());
    ui->cmbStopBits      ->setCurrentText(m.value(ss.stopBits        ).toString());
    ui->cmbFlowControl   ->setCurrentText(m.value(ss.flowControl     ).toString());
    ui->spTimeoutFB      ->setValue      (m.value(ss.timeoutFirstByte).toInt());
    ui->spTimeoutIB      ->setValue      (m.value(ss.timeoutInterByte).toInt());
    //--------------------- TCP ---------------------
    ui->spPort   ->setValue(m.value(ss.port   ).toInt());
    ui->spTimeout->setValue(m.value(ss.timeout).toInt());
}

void mbServerDialogPort::fillData(Modbus::Settings &m)
{
    mbServerPort::Strings ms = mbServerPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m[ms.name] = ui->lnName->text();
    m[ms.type] = ui->cmbType->currentText();
    //--------------------- SERIAL ---------------------
    m[ss.serialPortName   ] = ui->cmbSerialPortName->currentText();
    m[ss.baudRate         ] = ui->cmbBaudRate      ->currentText();
    m[ss.dataBits         ] = ui->cmbDataBits      ->currentText();
    m[ss.parity           ] = ui->cmbParity        ->currentText();
    m[ss.stopBits         ] = ui->cmbStopBits      ->currentText();
    m[ss.flowControl      ] = ui->cmbFlowControl   ->currentText();
    m[ss.timeoutFirstByte ] = ui->spTimeoutFB      ->value();
    m[ss.timeoutInterByte ] = ui->spTimeoutIB      ->value();
    //--------------------- TCP ---------------------
    m[ss.port   ] = ui->spPort   ->value();
    m[ss.timeout] = ui->spTimeout->value();
}

void mbServerDialogPort::setType(int type)
{
    switch (type)
    {
    case Modbus::TCP:
        ui->stackedWidget->setCurrentWidget(ui->pgTCP);
        break;
    case Modbus::ASC:
    case Modbus::RTU:
        ui->stackedWidget->setCurrentWidget(ui->pgSerial);
        break;
    }
}
