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
#include "core_dialogdevice.h"

#include <QTextCodec>

#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDialogButtonBox>

#include <project/core_device.h>

mbCoreDialogDevice::Strings::Strings() :
    title(QStringLiteral("Device")),
    settings_prefix(QStringLiteral("Ui.Dialogs.Device."))
{
}

const mbCoreDialogDevice::Strings &mbCoreDialogDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogDevice::mbCoreDialogDevice(QWidget *parent) :
    mbCoreDialogSettings(parent)
{
    memset(&m_ui, 0, sizeof(m_ui));
}

void mbCoreDialogDevice::initializeBaseUi()
{
    const mbCoreDevice::Defaults &dDevice = mbCoreDevice::Defaults::instance();

    Modbus::Defaults d = Modbus::Defaults::instance();

    QStringList ls;

    QSpinBox* sp;
    QLineEdit* ln;
    QComboBox* cmb;

    // Name
    ln = m_ui.lnName;
    ln->setText(dDevice.name);
    
    //--------------------- ADVANCED ---------------------
    // Max Read Coils
    sp = m_ui.spMaxReadCoils;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxReadCoils);

    // Max Read Discrete Inputs
    sp = m_ui.spMaxReadDiscreteInputs;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxReadDiscreteInputs);

    // Max Read Holding Registers
    sp = m_ui.spMaxReadHoldingRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxReadHoldingRegisters);

    // Max Read Input Registers
    sp = m_ui.spMaxReadInputRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxReadInputRegisters);

    // Max Write Multiple Coils
    sp = m_ui.spMaxWriteMultipleCoils;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS);
    sp->setValue(dDevice.maxWriteMultipleCoils);

    // Max Write Multiple Registers
    sp = m_ui.spMaxWriteMultipleRegisters;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_REGISTERS);
    sp->setValue(dDevice.maxWriteMultipleRegisters);

    // Register Order
    cmb = m_ui.cmbRegisterOrder;
    ls = mb::enumDataOrderKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultOrder' for device
        cmb->addItem(QString(ls.at(i)));

    // ByteArray format
    cmb = m_ui.cmbByteArrayFormat;
    ls = mb::enumDigitalFormatKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultDigitalFormat' for device
        cmb->addItem(QString(ls.at(i)));

    // ByteArray separator
    ln = m_ui.lnByteArraySeparator;
    ln->setText(mb::makeEscapeSequnces(dDevice.byteArraySeparator));

    // String Length Type
    cmb = m_ui.cmbStringLengthType;
    ls = mb::enumStringLengthTypeKeyList();
    for (int i = 1 ; i < ls.count(); i++) // pass 'DefaultStringLengthType' for device
        cmb->addItem(QString(ls.at(i)));

    // String Encoding
    cmb = m_ui.cmbStringEncoding;
    QList<QByteArray> codecs = QTextCodec::availableCodecs();
    Q_FOREACH (const QByteArray &s, codecs)
        cmb->addItem(s);

    connect(m_ui.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_ui.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

MBSETTINGS mbCoreDialogDevice::cachedSettings() const
{
    const mbCoreDevice::Strings &ms = mbCoreDevice::Strings::instance();
    const QString &prefix = Strings().settings_prefix;
    MBSETTINGS m;

    m[prefix+ms.name                     ] = m_ui.lnName                     ->text       ();
    m[prefix+ms.maxReadCoils             ] = m_ui.spMaxReadCoils             ->value      ();
    m[prefix+ms.maxReadDiscreteInputs    ] = m_ui.spMaxReadDiscreteInputs    ->value      ();
    m[prefix+ms.maxReadHoldingRegisters  ] = m_ui.spMaxReadHoldingRegisters  ->value      ();
    m[prefix+ms.maxReadInputRegisters    ] = m_ui.spMaxReadInputRegisters    ->value      ();
    m[prefix+ms.maxWriteMultipleCoils    ] = m_ui.spMaxWriteMultipleCoils    ->value      ();
    m[prefix+ms.maxWriteMultipleRegisters] = m_ui.spMaxWriteMultipleRegisters->value      ();
    m[prefix+ms.registerOrder            ] = m_ui.cmbRegisterOrder           ->currentText(); // TODO: Default order special processing
    m[prefix+ms.byteArrayFormat          ] = m_ui.cmbByteArrayFormat         ->currentText();
    m[prefix+ms.byteArraySeparator       ] = m_ui.lnByteArraySeparator       ->text       ();
    m[prefix+ms.stringLengthType         ] = m_ui.cmbStringLengthType        ->currentText();
    m[prefix+ms.stringEncoding           ] = m_ui.cmbStringEncoding          ->currentText();

    return m;
}

void mbCoreDialogDevice::setCachedSettings(const MBSETTINGS &m)
{
    const mbCoreDevice::Strings &ms = mbCoreDevice::Strings::instance();
    const QString &prefix = Strings().settings_prefix;

    m_ui.lnName                     ->setText       (m.value(prefix+ms.name                     ).toString());
    m_ui.spMaxReadCoils             ->setValue      (m.value(prefix+ms.maxReadCoils             ).toInt   ());
    m_ui.spMaxReadDiscreteInputs    ->setValue      (m.value(prefix+ms.maxReadDiscreteInputs    ).toInt   ());
    m_ui.spMaxReadHoldingRegisters  ->setValue      (m.value(prefix+ms.maxReadHoldingRegisters  ).toInt   ());
    m_ui.spMaxReadInputRegisters    ->setValue      (m.value(prefix+ms.maxReadInputRegisters    ).toInt   ());
    m_ui.spMaxWriteMultipleCoils    ->setValue      (m.value(prefix+ms.maxWriteMultipleCoils    ).toInt   ());
    m_ui.spMaxWriteMultipleRegisters->setValue      (m.value(prefix+ms.maxWriteMultipleRegisters).toInt   ());
    m_ui.cmbRegisterOrder           ->setCurrentText(m.value(prefix+ms.registerOrder            ).toString());
    m_ui.cmbByteArrayFormat         ->setCurrentText(m.value(prefix+ms.byteArrayFormat          ).toString());
    m_ui.lnByteArraySeparator       ->setText       (m.value(prefix+ms.byteArraySeparator       ).toString());
    m_ui.cmbStringLengthType        ->setCurrentText(m.value(prefix+ms.stringLengthType         ).toString());
    m_ui.cmbStringEncoding          ->setCurrentText(m.value(prefix+ms.stringEncoding           ).toString());
}

MBSETTINGS mbCoreDialogDevice::getSettings(const MBSETTINGS &settings, const QString &title)
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

void mbCoreDialogDevice::fillForm(const MBSETTINGS &m)
{
    const mbCoreDevice::Strings &ms = mbCoreDevice::Strings::instance();

    m_ui.lnName                     ->setText       (m.value(ms.name                     ).toString());
    m_ui.spMaxReadCoils             ->setValue      (m.value(ms.maxReadCoils             ).toInt   ());
    m_ui.spMaxReadDiscreteInputs    ->setValue      (m.value(ms.maxReadDiscreteInputs    ).toInt   ());
    m_ui.spMaxReadHoldingRegisters  ->setValue      (m.value(ms.maxReadHoldingRegisters  ).toInt   ());
    m_ui.spMaxReadInputRegisters    ->setValue      (m.value(ms.maxReadInputRegisters    ).toInt   ());
    m_ui.spMaxWriteMultipleCoils    ->setValue      (m.value(ms.maxWriteMultipleCoils    ).toInt   ());
    m_ui.spMaxWriteMultipleRegisters->setValue      (m.value(ms.maxWriteMultipleRegisters).toInt   ());
    m_ui.cmbRegisterOrder           ->setCurrentText(m.value(ms.registerOrder            ).toString());
    m_ui.cmbByteArrayFormat         ->setCurrentText(m.value(ms.byteArrayFormat          ).toString());
    m_ui.lnByteArraySeparator       ->setText       (m.value(ms.byteArraySeparator       ).toString());
    m_ui.cmbStringLengthType        ->setCurrentText(m.value(ms.stringLengthType         ).toString());
    m_ui.cmbStringEncoding          ->setCurrentText(m.value(ms.stringEncoding           ).toString());
}

void mbCoreDialogDevice::fillData(MBSETTINGS &m) const
{
    const mbCoreDevice::Strings &ms = mbCoreDevice::Strings::instance();

    m[ms.name                     ] = m_ui.lnName                     ->text        ();
    m[ms.maxReadCoils             ] = m_ui.spMaxReadCoils             ->value       ();
    m[ms.maxReadDiscreteInputs    ] = m_ui.spMaxReadDiscreteInputs    ->value       ();
    m[ms.maxReadHoldingRegisters  ] = m_ui.spMaxReadHoldingRegisters  ->value       ();
    m[ms.maxReadInputRegisters    ] = m_ui.spMaxReadInputRegisters    ->value       ();
    m[ms.maxWriteMultipleCoils    ] = m_ui.spMaxWriteMultipleCoils    ->value       ();
    m[ms.maxWriteMultipleRegisters] = m_ui.spMaxWriteMultipleRegisters->value       ();
    m[ms.registerOrder            ] = m_ui.cmbRegisterOrder           ->currentText (); // TODO: Default order special processing
    m[ms.byteArrayFormat          ] = m_ui.cmbByteArrayFormat         ->currentText ();
    m[ms.byteArraySeparator       ] = m_ui.lnByteArraySeparator       ->text        ();
    m[ms.stringLengthType         ] = m_ui.cmbStringLengthType        ->currentText ();
    m[ms.stringEncoding           ] = m_ui.cmbStringEncoding          ->currentText ();
}

