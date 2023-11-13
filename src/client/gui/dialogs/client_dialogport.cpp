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
#include "client_dialogport.h"
#include "ui_client_dialogport.h"

#include <QMetaEnum>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIntValidator>

#include <ModbusPortTCP.h>
#include <ModbusPortSerial.h>
#include <client.h>
#include <project/client_port.h>


mbClientDialogPort::mbClientDialogPort(QWidget *parent) :
    mbCoreDialogPort(parent),
    ui(new Ui::mbClientDialogPort)
{
    ui->setupUi(this);

    QMetaEnum e;

    Modbus::PortTCP::Defaults td = Modbus::PortTCP::Defaults::instance();
    Modbus::PortSerial::Defaults sd = Modbus::PortSerial::Defaults::instance();
    mbClientPort::Defaults d = mbClientPort::Defaults::instance();

    QSpinBox* sp;
    QLineEdit* ln;
    QComboBox* cmb;

    // Type
    cmb = ui->cmbType;
    e = QMetaEnum::fromType<Modbus::Type>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentText(e.valueToKey(Modbus::TCP));
    ui->stackedWidget->setCurrentWidget(ui->pgTCP);
    connect(ui->cmbType, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));

    //--------------------- SERIAL ---------------------
    // Serial Port
    cmb = ui->cmbSerialPortName;
    QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    Q_FOREACH(QSerialPortInfo port, ports)
        cmb->addItem(port.portName());

    // Baud Rate
    cmb = ui->cmbBaudRate;
    e = QMetaEnum::fromType<QSerialPort::BaudRate>();
    for (int i = 0; i < e.keyCount(); i++)
    {
        if (e.value(i) != QSerialPort::UnknownBaud)
            cmb->addItem(QString::number(e.value(i)));
    }
    cmb->setCurrentText(QString::number(sd.baudRate));

    // Data Bits
    cmb = ui->cmbDataBits;
    e = QMetaEnum::fromType<QSerialPort::DataBits>();
    for (int i = 0; i < e.keyCount(); i++)
    {
        if (e.value(i) != QSerialPort::UnknownDataBits)
            cmb->addItem(e.key(i));
    }
    cmb->setCurrentText(mb::enumKeyTypeStr<QSerialPort::DataBits>(sd.dataBits));

    // Parity
    cmb = ui->cmbParity;
    e = QMetaEnum::fromType<QSerialPort::Parity>();
    for (int i = 0; i < e.keyCount(); i++)
    {
        if (e.value(i) != QSerialPort::UnknownParity)
            cmb->addItem(QString(e.key(i)));
    }
    cmb->setCurrentText(mb::enumKeyTypeStr<QSerialPort::Parity>(sd.parity));

    // Stop Bits
    cmb = ui->cmbStopBits;
    e = QMetaEnum::fromType<QSerialPort::StopBits>();
    for (int i = 0; i < e.keyCount(); i++)
    {
        if (e.value(i) != QSerialPort::UnknownStopBits)
            cmb->addItem(QString(e.key(i)));
    }
    cmb->setCurrentText(mb::enumKeyTypeStr<QSerialPort::StopBits>(sd.stopBits));

    // Flow Control
    cmb = ui->cmbFlowControl;
    e = QMetaEnum::fromType<QSerialPort::FlowControl>();
    for (int i = 0; i < e.keyCount(); i++)
    {
        if (e.value(i) != QSerialPort::UnknownFlowControl)
            cmb->addItem(QString(e.key(i)));
    }
    cmb->setCurrentText(mb::enumKeyTypeStr<QSerialPort::FlowControl>(sd.flowControl));

    // Timeout first byte
    sp = ui->spTimeoutFB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(sd.timeoutFirstByte); // default slave address
    // Timeout first byte
    sp = ui->spTimeoutIB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(sd.timeoutInterByte); // default slave address

    //--------------------- TCP ---------------------
    // Host
    ln = ui->lnHost;
    ln->setText(td.host);
    // Port
    sp = ui->spPort;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(td.port);
    // Timeout
    sp = ui->spTimeout;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(td.timeout);
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbClientDialogPort::~mbClientDialogPort()
{
    delete ui;
}

MBSETTINGS mbClientDialogPort::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;

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

void mbClientDialogPort::fillForm(const MBSETTINGS &settings)
{
    mbClientPort::Strings       ms = mbClientPort::Strings();
    Modbus::PortTCP::Strings    ts = Modbus::PortTCP::Strings::instance();
    Modbus::PortSerial::Strings ss = Modbus::PortSerial::Strings::instance();

    ui->lnName->setText(settings.value(ms.name).toString());
    ui->cmbType->setCurrentText(settings.value(ms.type).toString());
    //--------------------- SERIAL ---------------------
    ui->cmbSerialPortName->setCurrentText(settings.value(ss.serialPortName).toString());
    ui->cmbBaudRate->setCurrentText(settings.value(ss.baudRate).toString());
    ui->cmbDataBits->setCurrentText(settings.value(ss.dataBits).toString());
    ui->cmbParity->setCurrentText(settings.value(ss.parity).toString());
    ui->cmbStopBits->setCurrentText(settings.value(ss.stopBits).toString());
    ui->cmbFlowControl->setCurrentText(settings.value(ss.flowControl).toString());
    ui->spTimeoutFB->setValue(settings.value(ss.timeoutFirstByte).toInt());
    ui->spTimeoutIB->setValue(settings.value(ss.timeoutInterByte).toInt());
    //--------------------- TCP ---------------------
    ui->lnHost   ->setText (settings.value(ts.host   ).toString());
    ui->spPort   ->setValue(settings.value(ts.port   ).toInt());
    ui->spTimeout->setValue(settings.value(ts.timeout).toInt());
}

void mbClientDialogPort::fillData(MBSETTINGS &m)
{
    mbClientPort::Strings       ms = mbClientPort::Strings();
    Modbus::PortTCP::Strings    ts = Modbus::PortTCP::Strings::instance();
    Modbus::PortSerial::Strings ss = Modbus::PortSerial::Strings::instance();

    m[ms.name] = ui->lnName->text();
    m[ms.type] = ui->cmbType->currentText();
    //--------------------- SERIAL ---------------------
    m[ss.serialPortName  ] = ui->cmbSerialPortName->currentText();
    m[ss.baudRate        ] = ui->cmbBaudRate      ->currentText();
    m[ss.dataBits        ] = ui->cmbDataBits      ->currentText();
    m[ss.parity          ] = ui->cmbParity        ->currentText();
    m[ss.stopBits        ] = ui->cmbStopBits      ->currentText();
    m[ss.flowControl     ] = ui->cmbFlowControl   ->currentText();
    m[ss.timeoutFirstByte] = ui->spTimeoutFB      ->value();
    m[ss.timeoutInterByte] = ui->spTimeoutIB      ->value();
    //--------------------- TCP ---------------------
    m[ts.host   ] = ui->lnHost   ->text();
    m[ts.port   ] = ui->spPort   ->value();
    m[ts.timeout] = ui->spTimeout->value();
}

void mbClientDialogPort::setType(int type)
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
