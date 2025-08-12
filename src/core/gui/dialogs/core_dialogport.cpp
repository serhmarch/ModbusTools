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
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QStackedWidget>
#include <QDialogButtonBox>

#include <core_global.h>
#include <project/core_port.h>

mbCoreDialogPort::Strings::Strings() : mbCoreDialogEdit::Strings(),
    title(QStringLiteral("Port")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Port."))
{
}

const mbCoreDialogPort::Strings &mbCoreDialogPort::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogPort::mbCoreDialogPort(QWidget *parent) :
    mbCoreDialogEdit(Strings::instance().cachePrefix, parent)
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
    mb::fillProtocolTypeComboBox(cmb);
    cmb->setCurrentText(Modbus::toString(Modbus::TCP));
    m_ui.stackedWidget->setCurrentWidget(m_ui.pgTCP);
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setType(int)));

    //--------------------- SERIAL ---------------------
    // Serial Port
    cmb = m_ui.cmbSerialPortName;
    QStringList ports = Modbus::availableSerialPortList();
    Q_FOREACH(const QString &port, ports)
        cmb->addItem(port);
    cmb->setEditable(true); // Note: Allow user right to enter port name if it's absent in list

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

    // Advanced
    m_ui.chbBroadcastEnable->setChecked(d.isBroadcastEnabled);
}

MBSETTINGS mbCoreDialogPort::cachedSettings() const
{
    const mbCorePort::Strings &vs = mbCorePort::Strings::instance();
    Modbus::Strings ms = Modbus::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS m = mbCoreDialogEdit::cachedSettings();
    m[prefix+vs.name              ] = m_ui.lnName->text();
    m[prefix+vs.type              ] = m_ui.cmbType->currentText();
    m[prefix+ms.serialPortName    ] = m_ui.cmbSerialPortName->currentText();
    m[prefix+ms.baudRate          ] = m_ui.cmbBaudRate      ->currentText();
    m[prefix+ms.dataBits          ] = m_ui.cmbDataBits      ->currentText();
    m[prefix+ms.parity            ] = m_ui.cmbParity        ->currentText();
    m[prefix+ms.stopBits          ] = m_ui.cmbStopBits      ->currentText();
    m[prefix+ms.flowControl       ] = m_ui.cmbFlowControl   ->currentText();
    m[prefix+ms.timeoutFirstByte  ] = m_ui.spTimeoutFB      ->value();
    m[prefix+ms.timeoutInterByte  ] = m_ui.spTimeoutIB      ->value();
    m[prefix+ms.port              ] = m_ui.spPort   ->value();
    m[prefix+ms.timeout           ] = m_ui.spTimeout->value();
    m[prefix+ms.isBroadcastEnabled] = m_ui.chbBroadcastEnable->isChecked();
    return m;
}

void mbCoreDialogPort::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogEdit::setCachedSettings(m);

    mbCorePort::Strings vs = mbCorePort::Strings();
    Modbus::Strings ms = Modbus::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.name              ); if (it != end) m_ui.lnName            ->setText(it.value().toString());
    it = m.find(prefix+vs.type              ); if (it != end) m_ui.cmbType           ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.serialPortName    ); if (it != end) m_ui.cmbSerialPortName ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.baudRate          ); if (it != end) m_ui.cmbBaudRate       ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.dataBits          ); if (it != end) m_ui.cmbDataBits       ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.parity            ); if (it != end) m_ui.cmbParity         ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.stopBits          ); if (it != end) m_ui.cmbStopBits       ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.flowControl       ); if (it != end) m_ui.cmbFlowControl    ->setCurrentText(it.value().toString());
    it = m.find(prefix+ms.timeoutFirstByte  ); if (it != end) m_ui.spTimeoutFB       ->setValue      (it.value().toInt());
    it = m.find(prefix+ms.timeoutInterByte  ); if (it != end) m_ui.spTimeoutIB       ->setValue      (it.value().toInt());
    it = m.find(prefix+ms.port              ); if (it != end) m_ui.spPort            ->setValue      (it.value().toInt());
    it = m.find(prefix+ms.timeout           ); if (it != end) m_ui.spTimeout         ->setValue      (it.value().toInt());
    it = m.find(prefix+ms.isBroadcastEnabled); if (it != end) m_ui.chbBroadcastEnable->setChecked    (it.value().toBool());
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

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(ms.name              ); if (it != end) m_ui.lnName            ->setText       (it.value().toString());
    it = m.find(ms.type              ); if (it != end) m_ui.cmbType           ->setCurrentText(it.value().toString());
    it = m.find(ss.serialPortName    ); if (it != end) m_ui.cmbSerialPortName ->setCurrentText(it.value().toString());
    it = m.find(ss.baudRate          ); if (it != end) m_ui.cmbBaudRate       ->setCurrentText(it.value().toString());
    it = m.find(ss.dataBits          ); if (it != end) m_ui.cmbDataBits       ->setCurrentText(it.value().toString());
    it = m.find(ss.parity            ); if (it != end) m_ui.cmbParity         ->setCurrentText(it.value().toString());
    it = m.find(ss.stopBits          ); if (it != end) m_ui.cmbStopBits       ->setCurrentText(it.value().toString());
    it = m.find(ss.flowControl       ); if (it != end) m_ui.cmbFlowControl    ->setCurrentText(it.value().toString());
    it = m.find(ss.timeoutFirstByte  ); if (it != end) m_ui.spTimeoutFB       ->setValue      (it.value().toInt   ());
    it = m.find(ss.timeoutInterByte  ); if (it != end) m_ui.spTimeoutIB       ->setValue      (it.value().toInt   ());
    it = m.find(ss.port              ); if (it != end) m_ui.spPort            ->setValue      (it.value().toInt   ());
    it = m.find(ss.timeout           ); if (it != end) m_ui.spTimeout         ->setValue      (it.value().toInt   ());
    it = m.find(ss.isBroadcastEnabled); if (it != end) m_ui.chbBroadcastEnable->setChecked    (it.value().toBool());

    fillFormInner(m);
}

void mbCoreDialogPort::fillData(MBSETTINGS &m) const
{
    mbCorePort::Strings ms = mbCorePort::Strings();
    Modbus::Strings ss = Modbus::Strings::instance();

    m[ms.name              ] = m_ui.lnName            ->text       ();
    m[ms.type              ] = m_ui.cmbType           ->currentText();
    m[ss.serialPortName    ] = m_ui.cmbSerialPortName ->currentText();
    m[ss.baudRate          ] = m_ui.cmbBaudRate       ->currentText();
    m[ss.dataBits          ] = m_ui.cmbDataBits       ->currentText();
    m[ss.parity            ] = m_ui.cmbParity         ->currentText();
    m[ss.stopBits          ] = m_ui.cmbStopBits       ->currentText();
    m[ss.flowControl       ] = m_ui.cmbFlowControl    ->currentText();
    m[ss.timeoutFirstByte  ] = m_ui.spTimeoutFB       ->value      ();
    m[ss.timeoutInterByte  ] = m_ui.spTimeoutIB       ->value      ();
    m[ss.port              ] = m_ui.spPort            ->value      ();
    m[ss.timeout           ] = m_ui.spTimeout         ->value      ();
    m[ss.isBroadcastEnabled] = m_ui.chbBroadcastEnable->isChecked  ();

    fillDataInner(m);
}

void mbCoreDialogPort::setType(int type)
{
    switch (type)
    {
    case Modbus::TCP:
    case Modbus::UDP:
    case Modbus::ASCvTCP:
    case Modbus::RTUvTCP:
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
    // Note: Base implementation do nothing
}

void mbCoreDialogPort::fillDataInner(MBSETTINGS &/*settings*/) const
{
    // Note: Base implementation do nothing
}
