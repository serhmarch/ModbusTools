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
#include "client_dialogdevice.h"
#include "ui_client_dialogdevice.h"

#include <QMetaEnum>
#include <QTextCodec>
#include <QIntValidator>

#include <ModbusQt.h>

#include <client.h>
#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

mbClientDialogDevice::Strings::Strings() :
    mbCoreDialogDevice::Strings(),
    createDeviceForPort(QStringLiteral("create_device_for_port")){

}

const mbClientDialogDevice::Strings &mbClientDialogDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogDevice::mbClientDialogDevice(QWidget *parent) :
    mbCoreDialogDevice(parent),
    ui(new Ui::mbClientDialogDevice)
{
    ui->setupUi(this);

    QStringList ls;

    mbClientPort::Defaults dPort = mbClientPort::Defaults::instance();
    mbClientDevice::Defaults dDevice = mbClientDevice::Defaults::instance();

    Modbus::Defaults d = Modbus::Defaults::instance();

    QSpinBox* sp;
    QLineEdit* ln;
    QComboBox* cmb;

    // Name
    ln = ui->lnName;
    ln->setText(dDevice.name);
    // Unit
    sp = ui->spUnit;
    //sp->setMinimum(Modbus::VALID_MODBUS_ADDRESS_BEGIN);
    //sp->setMaximum(Modbus::VALID_MODBUS_ADDRESS_END);
    sp->setMinimum(0);
    sp->setMaximum(255);
    sp->setValue(dDevice.unit); // default unit address
    // Port
    connect(ui->cmbPort, SIGNAL(currentIndexChanged(int)), this, SLOT(setPort(int)));

    // Port Name
    ln = ui->lnPortName;
    ln->setText(dPort.name);
    // Type
    cmb = ui->cmbPortType;
    cmb->addItem(Modbus::toString(Modbus::ASC));
    cmb->addItem(Modbus::toString(Modbus::RTU));
    cmb->addItem(Modbus::toString(Modbus::TCP));
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));
    ui->stackedWidget->setCurrentWidget(ui->pgTCP);
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setPortType(int)));

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
    // Host
    ln = ui->lnHost;
    ln->setText(d.host);
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

    //--------------------- ADVANCED ---------------------
    // Max Read Coils
    sp = ui->spMaxReadCoils;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxReadCoils);

    // Max Read Discrete Inputs
    sp = ui->spMaxReadDiscreteInputs;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxReadDiscreteInputs);

    // Max Read Holding Registers
    sp = ui->spMaxReadHoldingRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxReadHoldingRegisters);

    // Max Read Input Registers
    sp = ui->spMaxReadInputRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxReadInputRegisters);

    // Max Write Multiple Coils
    sp = ui->spMaxWriteMultipleCoils;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxWriteMultipleCoils);

    // Max Write Multiple Registers
    sp = ui->spMaxWriteMultipleRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxWriteMultipleRegisters);

    // Register Order
    cmb = ui->cmbRegisterOrder;
    ls = mb::enumDataOrderKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultOrder' for device
        cmb->addItem(QString(ls.at(i)));

    // ByteArray format
    cmb = ui->cmbByteArrayFormat;
    ls = mb::enumDigitalFormatKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultDigitalFormat' for device
        cmb->addItem(QString(ls.at(i)));

    // ByteArray separator
    ln = ui->lnByteArraySeparator;
    ln->setText(mb::makeEscapeSequnces(dDevice.byteArraySeparator));

    // String Length Type
    cmb = ui->cmbStringLengthType;
    ls = mb::enumStringLengthTypeKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultStringLengthType' for device
        cmb->addItem(QString(ls.at(i)));

    // String Encoding
    cmb = ui->cmbStringEncoding;
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    Q_FOREACH (const QByteArray &s, codecs)
        cmb->addItem(s);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbClientDialogDevice::~mbClientDialogDevice()
{
    delete ui;
}

MBSETTINGS mbClientDialogDevice::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;

    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillPortNames();
    if (settings.count())
        fillForm(settings);
    // show main tab
    ui->tabWidget->setCurrentIndex(0);
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbClientDialogDevice::fillForm(const MBSETTINGS &m)
{
    mbClientDevice::Strings ms = mbClientDevice::Strings();

    auto it = m.find(Strings::instance().createDeviceForPort);
    if (it != m.end())
    {
        ui->cmbPort->setEnabled(false);
        QString portName = it.value().toString();
        setPortName(portName);
    }
    else
    {
        ui->lnName->setText(m.value(ms.name).toString());
        ui->spUnit->setValue(m.value(ms.unit).toInt());
        //--------------------- ADVANCED ---------------------
        ui->spMaxReadCoils->setValue(m.value(ms.maxReadCoils).toInt());
        ui->spMaxReadDiscreteInputs->setValue(m.value(ms.maxReadDiscreteInputs).toInt());
        ui->spMaxReadHoldingRegisters->setValue(m.value(ms.maxReadHoldingRegisters).toInt());
        ui->spMaxReadInputRegisters->setValue(m.value(ms.maxReadInputRegisters).toInt());
        ui->spMaxWriteMultipleCoils->setValue(m.value(ms.maxWriteMultipleCoils).toInt());
        ui->spMaxWriteMultipleRegisters->setValue(m.value(ms.maxWriteMultipleRegisters).toInt());
        ui->cmbRegisterOrder->setCurrentText(m.value(ms.registerOrder).toString()); // TODO: Default order special processing
        ui->cmbByteArrayFormat->setCurrentText(m.value(ms.byteArrayFormat).toString());
        ui->lnByteArraySeparator->setText(m.value(ms.byteArraySeparator).toString());
        ui->cmbStringLengthType->setCurrentText(m.value(ms.stringLengthType).toString());
        ui->cmbStringEncoding->setCurrentText(m.value(ms.stringEncoding).toString());
        //----------------------- PORT -----------------------
        ui->cmbPort->setEnabled(true);
        QString portName = m.value(ms.portName).toString();
        setPortName(portName);
    }
}

void mbClientDialogDevice::fillData(MBSETTINGS &m)
{
    mbClientDevice::Strings ms = mbClientDevice::Strings();

    m[ms.name] = ui->lnName->text();
    m[ms.unit] = ui->spUnit->value();
    //--------------------- ADVANCED ---------------------
    m[ms.maxReadCoils] = ui->spMaxReadCoils->value();
    m[ms.maxReadDiscreteInputs] = ui->spMaxReadDiscreteInputs->value();
    m[ms.maxReadHoldingRegisters] = ui->spMaxReadHoldingRegisters->value();
    m[ms.maxReadInputRegisters] = ui->spMaxReadInputRegisters->value();
    m[ms.maxWriteMultipleCoils] = ui->spMaxWriteMultipleCoils->value();
    m[ms.maxWriteMultipleRegisters] = ui->spMaxWriteMultipleRegisters->value();
    m[ms.registerOrder] = ui->cmbRegisterOrder->currentText(); // TODO: Default order special processing
    m[ms.byteArrayFormat] = ui->cmbByteArrayFormat->currentText();
    m[ms.byteArraySeparator] = ui->lnByteArraySeparator->text();
    m[ms.stringLengthType] = ui->cmbStringLengthType->currentText();
    m[ms.stringEncoding] = ui->cmbStringEncoding->currentText();
    //----------------------- PORT -----------------------
    if (ui->cmbPort->currentIndex() == 0) // Note: Create New Port
    {
        MBSETTINGS p;
        fillPortData(p);
        m[ms.portName] = p;
    }
    else
        m[ms.portName] = ui->lnPortName->text();
}

void mbClientDialogDevice::fillPortNames()
{
    ui->cmbPort->clear();
    mbClientProject *project = mbClient::global()->project();
    if (project)
    {
        ui->cmbPort->addItem(QStringLiteral("Create New Port ..."));
        QList<mbClientPort*> ports = project->ports();
        Q_FOREACH (mbClientPort *port, ports)
            ui->cmbPort->addItem(port->name());
    }
}

void mbClientDialogDevice::setPortName(const QString &portName)
{
    mbClientProject *project = mbClient::global()->project();
    if (project)
    {
        mbClientPort *p = project->port(portName);
        if (p)
            ui->cmbPort->setCurrentText(portName);
        else
            ui->cmbPort->setCurrentIndex(0);
    }
}

void mbClientDialogDevice::fillPortForm(const MBSETTINGS &m)
{
    mbClientPort::Strings       ms = mbClientPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    QString portName = m.value(ms.name).toString();
    //ui->cmbPort->setCurrentText(portName);
    ui->lnPortName->setText(portName);
    ui->cmbPortType->setCurrentText(m.value(ms.type).toString());
    //--------------------- SERIAL ---------------------
    ui->cmbSerialPortName->setCurrentText(m.value(ss.serialPortName).toString());
    ui->cmbBaudRate->setCurrentText(m.value(ss.baudRate).toString());
    ui->cmbDataBits->setCurrentText(m.value(ss.dataBits).toString());
    ui->cmbParity->setCurrentText(m.value(ss.parity).toString());
    ui->cmbStopBits->setCurrentText(m.value(ss.stopBits).toString());
    ui->cmbFlowControl->setCurrentText(m.value(ss.flowControl).toString());
    ui->spTimeoutFB->setValue(m.value(ss.timeoutFirstByte).toInt());
    ui->spTimeoutIB->setValue(m.value(ss.timeoutInterByte).toInt());
    //--------------------- TCP ---------------------
    ui->lnHost   ->setText (m.value(ss.host   ).toString());
    ui->spPort   ->setValue(m.value(ss.port   ).toInt());
    ui->spTimeout->setValue(m.value(ss.timeout).toInt());
}

void mbClientDialogDevice::fillPortData(MBSETTINGS &m)
{
    mbClientPort::Strings       ms = mbClientPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m[ms.name] = ui->lnPortName->text();
    m[ms.type] = ui->cmbPortType->currentText();
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
    m[ss.host   ] = ui->lnHost   ->text();
    m[ss.port   ] = ui->spPort   ->value();
    m[ss.timeout] = ui->spTimeout->value();
}

void mbClientDialogDevice::setPort(int i)
{
    if (i > 0)
    {
        i--;
        setPortEnable(false);
        mbClientProject *project = mbClient::global()->project();
        if (!project)
            return;
        mbClientPort *port = project->port(i);
        if (!port)
            return;
        MBSETTINGS s = port->settings();
        fillPortForm(s);
    }
    else
        setPortEnable(true);
}

void mbClientDialogDevice::setPortType(int type)
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

void mbClientDialogDevice::setPortEnable(bool enable)
{
    ui->cmbPortType->setEnabled(enable);
    ui->lnPortName->setEnabled(enable);
    ui->stackedWidget->setEnabled(enable);
}
