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
#include "core_dialogport.h"

#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QDialogButtonBox>

#include <project/core_port.h>

mbCoreDialogPort::Strings::Strings() :
    title(QStringLiteral("Port")),
    settings_prefix(QStringLiteral("Core.Ui.Dialogs.Port."))
{
}

const mbCoreDialogPort::Strings &mbCoreDialogPort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogPort::mbCoreDialogPort(QWidget *parent) :
    mbCoreDialogSettings(parent)
{
    memset(&m_ui, 0, sizeof(m_ui));
}

void mbCoreDialogPort::initializeBaseUi()
{
    Modbus::Defaults d = Modbus::Defaults::instance();

    //QLineEdit* ln;
    QSpinBox* sp;
    QComboBox* cmb;

    // Type
    cmb = m_ui.cmbType;
    cmb->addItem(Modbus::toString(Modbus::ASC));
    cmb->addItem(Modbus::toString(Modbus::RTU));
    cmb->addItem(Modbus::toString(Modbus::TCP));
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));
    m_ui.stackedWidget->setCurrentWidget(m_ui.pgTCP);
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));

    //--------------------- SERIAL ---------------------
    // Serial Port
    cmb = m_ui.cmbSerialPortName;
    QStringList ports = Modbus::availableSerialPortList();
    Q_FOREACH(const QString &port, ports)
        cmb->addItem(port);

    // Baud Rate
    cmb = m_ui.cmbBaudRate;
    Modbus::List<int32_t> baudRates = Modbus::availableBaudRate();
    for (int32_t v : baudRates)
        cmb->addItem(QString::number(v));
    cmb->setCurrentText(QString::number(d.baudRate));

    // Data Bits
    cmb = m_ui.cmbDataBits;
    Modbus::List<int8_t> dataBits = Modbus::availableDataBits();
    for (int8_t v : dataBits)
        cmb->addItem(QString::number(v));
    cmb->setCurrentText(QString::number(d.dataBits));

    // Parity
    cmb = m_ui.cmbParity;
    Modbus::List<Modbus::Parity> parity = Modbus::availableParity();
    for (Modbus::Parity v : parity)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.parity));

    // Stop Bits
    cmb = m_ui.cmbStopBits;
    Modbus::List<Modbus::StopBits> stopBits = Modbus::availableStopBits();
    for (Modbus::StopBits v : stopBits)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.stopBits));

    // Flow Control
    cmb = m_ui.cmbFlowControl;
    Modbus::List<Modbus::FlowControl> flowControl = Modbus::availableFlowControl();
    for (Modbus::FlowControl v : flowControl)
        cmb->addItem(Modbus::toString(v));
    cmb->setCurrentText(Modbus::toString(d.flowControl));

    // Timeout first byte
    sp = m_ui.spTimeoutFB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeoutFirstByte); // default slave address
    // Timeout first byte
    sp = m_ui.spTimeoutIB;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeoutInterByte); // default slave address

    //--------------------- TCP ---------------------
    // Port
    sp = m_ui.spPort;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(d.port);
    // Timeout
    sp = m_ui.spTimeout;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.timeout);
    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

MBSETTINGS mbCoreDialogPort::cachedSettings() const
{
    MBSETTINGS m;
    mbCorePort::Strings ms = mbCorePort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();
    const QString &prefix = Strings().settings_prefix;

    m[prefix+ms.name             ] = m_ui.lnName->text();
    m[prefix+ms.type             ] = m_ui.cmbType->currentText();
    m[prefix+ss.serialPortName   ] = m_ui.cmbSerialPortName->currentText();
    m[prefix+ss.baudRate         ] = m_ui.cmbBaudRate      ->currentText();
    m[prefix+ss.dataBits         ] = m_ui.cmbDataBits      ->currentText();
    m[prefix+ss.parity           ] = m_ui.cmbParity        ->currentText();
    m[prefix+ss.stopBits         ] = m_ui.cmbStopBits      ->currentText();
    m[prefix+ss.flowControl      ] = m_ui.cmbFlowControl   ->currentText();
    m[prefix+ss.timeoutFirstByte ] = m_ui.spTimeoutFB      ->value();
    m[prefix+ss.timeoutInterByte ] = m_ui.spTimeoutIB      ->value();
    m[prefix+ss.port             ] = m_ui.spPort   ->value();
    m[prefix+ss.timeout          ] = m_ui.spTimeout->value();

    return m;
}

void mbCoreDialogPort::setCachedSettings(const MBSETTINGS &m)
{
    mbCorePort::Strings ms = mbCorePort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();
    const QString &prefix = Strings().settings_prefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(prefix+ms.name            ); if (it != end) m_ui.lnName           ->setText(it.value().toString());
    it = m.find(prefix+ms.type            ); if (it != end) m_ui.cmbType          ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.serialPortName  ); if (it != end) m_ui.cmbSerialPortName->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.baudRate        ); if (it != end) m_ui.cmbBaudRate      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.dataBits        ); if (it != end) m_ui.cmbDataBits      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.parity          ); if (it != end) m_ui.cmbParity        ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.stopBits        ); if (it != end) m_ui.cmbStopBits      ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.flowControl     ); if (it != end) m_ui.cmbFlowControl   ->setCurrentText(it.value().toString());
    it = m.find(prefix+ss.timeoutFirstByte); if (it != end) m_ui.spTimeoutFB      ->setValue      (it.value().toInt());
    it = m.find(prefix+ss.timeoutInterByte); if (it != end) m_ui.spTimeoutIB      ->setValue      (it.value().toInt());
    it = m.find(prefix+ss.port            ); if (it != end) m_ui.spPort           ->setValue      (it.value().toInt());
    it = m.find(prefix+ss.timeout         ); if (it != end) m_ui.spTimeout        ->setValue      (it.value().toInt());
}

MBSETTINGS mbCoreDialogPort::getSettings(const MBSETTINGS &settings, const QString &title)
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

void mbCoreDialogPort::fillForm(const MBSETTINGS &m)
{
    mbCorePort::Strings ms = mbCorePort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m_ui.lnName->setText(m.value(ms.name).toString());
    m_ui.cmbType->setCurrentText(m.value(ms.type).toString());
    //--------------------- SERIAL ---------------------
    m_ui.cmbSerialPortName->setCurrentText(m.value(ss.serialPortName  ).toString());
    m_ui.cmbBaudRate      ->setCurrentText(m.value(ss.baudRate        ).toString());
    m_ui.cmbDataBits      ->setCurrentText(m.value(ss.dataBits        ).toString());
    m_ui.cmbParity        ->setCurrentText(m.value(ss.parity          ).toString());
    m_ui.cmbStopBits      ->setCurrentText(m.value(ss.stopBits        ).toString());
    m_ui.cmbFlowControl   ->setCurrentText(m.value(ss.flowControl     ).toString());
    m_ui.spTimeoutFB      ->setValue      (m.value(ss.timeoutFirstByte).toInt());
    m_ui.spTimeoutIB      ->setValue      (m.value(ss.timeoutInterByte).toInt());
    //--------------------- TCP ---------------------
    m_ui.spPort   ->setValue(m.value(ss.port   ).toInt());
    m_ui.spTimeout->setValue(m.value(ss.timeout).toInt());

    fillFormInner(m);
}

void mbCoreDialogPort::fillData(MBSETTINGS &m) const
{
    mbCorePort::Strings ms = mbCorePort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m[ms.name] = m_ui.lnName->text();
    m[ms.type] = m_ui.cmbType->currentText();
    //--------------------- SERIAL ---------------------
    m[ss.serialPortName   ] = m_ui.cmbSerialPortName->currentText();
    m[ss.baudRate         ] = m_ui.cmbBaudRate      ->currentText();
    m[ss.dataBits         ] = m_ui.cmbDataBits      ->currentText();
    m[ss.parity           ] = m_ui.cmbParity        ->currentText();
    m[ss.stopBits         ] = m_ui.cmbStopBits      ->currentText();
    m[ss.flowControl      ] = m_ui.cmbFlowControl   ->currentText();
    m[ss.timeoutFirstByte ] = m_ui.spTimeoutFB      ->value();
    m[ss.timeoutInterByte ] = m_ui.spTimeoutIB      ->value();
    //--------------------- TCP ---------------------
    m[ss.port   ] = m_ui.spPort   ->value();
    m[ss.timeout] = m_ui.spTimeout->value();
    fillDataInner(m);
}

void mbCoreDialogPort::setType(int type)
{
    switch (type)
    {
    case Modbus::TCP:
        m_ui.stackedWidget->setCurrentWidget(m_ui.pgTCP);
        break;
    case Modbus::ASC:
    case Modbus::RTU:
        m_ui.stackedWidget->setCurrentWidget(m_ui.pgSerial);
        break;
    }
}

void mbCoreDialogPort::fillFormInner(const MBSETTINGS &/*settings*/)
{
}

void mbCoreDialogPort::fillDataInner(MBSETTINGS &/*settings*/) const
{
}
