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
    createDeviceForPort(QStringLiteral("create_device_for_port")),
    portName           (QStringLiteral("portName"))
{
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

    m_ui.lnName                      = ui->lnName                     ;
    m_ui.spMaxReadCoils              = ui->spMaxReadCoils             ;
    m_ui.spMaxReadDiscreteInputs     = ui->spMaxReadDiscreteInputs    ;
    m_ui.spMaxReadHoldingRegisters   = ui->spMaxReadHoldingRegisters  ;
    m_ui.spMaxReadInputRegisters     = ui->spMaxReadInputRegisters    ;
    m_ui.spMaxWriteMultipleCoils     = ui->spMaxWriteMultipleCoils    ;
    m_ui.spMaxWriteMultipleRegisters = ui->spMaxWriteMultipleRegisters;
    m_ui.cmbRegisterOrder            = ui->cmbRegisterOrder           ;
    m_ui.cmbByteArrayFormat          = ui->cmbByteArrayFormat         ;
    m_ui.lnByteArraySeparator        = ui->lnByteArraySeparator       ;
    m_ui.cmbStringLengthType         = ui->cmbStringLengthType        ;
    m_ui.cmbStringEncoding           = ui->cmbStringEncoding          ;
    m_ui.buttonBox                   = ui->buttonBox                  ;

    initializeBaseUi();

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
    cmb->setEditable(true); // Note: Allow user right to enter port name if it's absent in list

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

}

mbClientDialogDevice::~mbClientDialogDevice()
{
    delete ui;
}

MBSETTINGS mbClientDialogDevice::cachedSettings() const
{
    const mbClientPort::Strings &ms = mbClientPort::Strings();
    const Modbus::Strings ss = Modbus::Strings::instance();
    const Strings &ds = Strings();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS m = mbCoreDialogDevice::cachedSettings();

    m[prefix+ds.portName        ] = ui->lnPortName       ->text       ();
    m[prefix+ms.type            ] = ui->cmbPortType      ->currentText();
    m[prefix+ss.serialPortName  ] = ui->cmbSerialPortName->currentText();
    m[prefix+ss.baudRate        ] = ui->cmbBaudRate      ->currentText();
    m[prefix+ss.dataBits        ] = ui->cmbDataBits      ->currentText();
    m[prefix+ss.parity          ] = ui->cmbParity        ->currentText();
    m[prefix+ss.stopBits        ] = ui->cmbStopBits      ->currentText();
    m[prefix+ss.flowControl     ] = ui->cmbFlowControl   ->currentText();
    m[prefix+ss.timeoutFirstByte] = ui->spTimeoutFB      ->value      ();
    m[prefix+ss.timeoutInterByte] = ui->spTimeoutIB      ->value      ();
    m[prefix+ss.host            ] = ui->lnHost           ->text       ();
    m[prefix+ss.port            ] = ui->spPort           ->value      ();
    m[prefix+ss.timeout         ] = ui->spTimeout        ->value      ();

    return m;
}

void mbClientDialogDevice::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogDevice::setCachedSettings(m);

    const mbClientPort::Strings &ms = mbClientPort::Strings();
    const Modbus::Strings ss = Modbus::Strings::instance();
    const Strings &ds = Strings();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+ds.portName        ); if (it != end)  ui->lnPortName       ->setText       (it.value().toString());
    it = m.find(prefix+ms.type            ); if (it != end)  ui->cmbPortType      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.serialPortName  ); if (it != end)  ui->cmbSerialPortName->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.baudRate        ); if (it != end)  ui->cmbBaudRate      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.dataBits        ); if (it != end)  ui->cmbDataBits      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.parity          ); if (it != end)  ui->cmbParity        ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.stopBits        ); if (it != end)  ui->cmbStopBits      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.flowControl     ); if (it != end)  ui->cmbFlowControl   ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.timeoutFirstByte); if (it != end)  ui->spTimeoutFB      ->setValue      (it.value().toInt   ());
    it = m.find(prefix+ss.timeoutInterByte); if (it != end)  ui->spTimeoutIB      ->setValue      (it.value().toInt   ());
    it = m.find(prefix+ss.host            ); if (it != end)  ui->lnHost           ->setText       (it.value().toString());
    it = m.find(prefix+ss.port            ); if (it != end)  ui->spPort           ->setValue      (it.value().toInt   ());
    it = m.find(prefix+ss.timeout         ); if (it != end)  ui->spTimeout        ->setValue      (it.value().toInt   ());

    QString portName = ui->lnPortName->text();
    if (!portName.isEmpty())
        ui->cmbPort->setCurrentText(portName);
}

MBSETTINGS mbClientDialogDevice::getSettings(const MBSETTINGS &settings, const QString &title)
{
    fillPortNames();
    return mbCoreDialogDevice::getSettings(settings, title);

}

void mbClientDialogDevice::fillForm(const MBSETTINGS &m)
{
    const mbClientDevice::Strings &ms = mbClientDevice::Strings::instance();

    auto it = m.find(Strings::instance().createDeviceForPort);
    auto end = m.end();

    if (it != end)
    {
        ui->cmbPort->setEnabled(false);
        setPortName(it.value().toString());
    }
    else
    {
        mbCoreDialogDevice::fillForm(m);
        ui->cmbPort->setEnabled(true);
        it = m.find(ms.unit    ); if (it != end) ui->spUnit->setValue   (it.value().toInt   ());
        it = m.find(ms.portName); if (it != end) this->      setPortName(it.value().toString());
    }
}

void mbClientDialogDevice::fillData(MBSETTINGS &m) const
{
    mbClientDevice::Strings ms = mbClientDevice::Strings();

    m[ms.unit] = ui->spUnit->value();
    mbCoreDialogDevice::fillData(m);

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
    mbClientPort::Strings ms = mbClientPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(ms.name            ); if (it != end) ui->lnPortName       ->setText       (it.value().toString());
    it = m.find(ms.type            ); if (it != end) ui->cmbPortType      ->setCurrentText(it.value().toString());
    it = m.find(ss.serialPortName  ); if (it != end) ui->cmbSerialPortName->setCurrentText(it.value().toString());
    it = m.find(ss.baudRate        ); if (it != end) ui->cmbBaudRate      ->setCurrentText(it.value().toString());
    it = m.find(ss.dataBits        ); if (it != end) ui->cmbDataBits      ->setCurrentText(it.value().toString());
    it = m.find(ss.parity          ); if (it != end) ui->cmbParity        ->setCurrentText(it.value().toString());
    it = m.find(ss.stopBits        ); if (it != end) ui->cmbStopBits      ->setCurrentText(it.value().toString());
    it = m.find(ss.flowControl     ); if (it != end) ui->cmbFlowControl   ->setCurrentText(it.value().toString());
    it = m.find(ss.timeoutFirstByte); if (it != end) ui->spTimeoutFB      ->setValue      (it.value().toInt   ());
    it = m.find(ss.timeoutInterByte); if (it != end) ui->spTimeoutIB      ->setValue      (it.value().toInt   ());
    it = m.find(ss.host            ); if (it != end) ui->lnHost           ->setText       (it.value().toString());
    it = m.find(ss.port            ); if (it != end) ui->spPort           ->setValue      (it.value().toInt   ());
    it = m.find(ss.timeout         ); if (it != end) ui->spTimeout        ->setValue      (it.value().toInt   ());
}

void mbClientDialogDevice::fillPortData(MBSETTINGS &m) const
{
    mbClientPort::Strings ms = mbClientPort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m[ms.name            ] = ui->lnPortName       ->text       ();
    m[ms.type            ] = ui->cmbPortType      ->currentText();
    m[ss.serialPortName  ] = ui->cmbSerialPortName->currentText();
    m[ss.baudRate        ] = ui->cmbBaudRate      ->currentText();
    m[ss.dataBits        ] = ui->cmbDataBits      ->currentText();
    m[ss.parity          ] = ui->cmbParity        ->currentText();
    m[ss.stopBits        ] = ui->cmbStopBits      ->currentText();
    m[ss.flowControl     ] = ui->cmbFlowControl   ->currentText();
    m[ss.timeoutFirstByte] = ui->spTimeoutFB      ->value      ();
    m[ss.timeoutInterByte] = ui->spTimeoutIB      ->value      ();
    m[ss.host            ] = ui->lnHost           ->text       ();
    m[ss.port            ] = ui->spPort           ->value      ();
    m[ss.timeout         ] = ui->spTimeout        ->value      ();
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
