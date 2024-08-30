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
#include "server_dialogdevice.h"
#include "ui_server_dialogdevice.h"

#include <QMetaEnum>
#include <QTextCodec>
#include <QIntValidator>

#include <server.h>

#include <project/server_project.h>
#include <project/server_deviceref.h>

mbServerDialogDevice::Strings::Strings() :
    mode (QStringLiteral("device_dialog_mode"))
{
}

const mbServerDialogDevice::Strings &mbServerDialogDevice::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogDevice::mbServerDialogDevice(QWidget *parent) :
    mbCoreDialogDevice(parent),
    ui(new Ui::mbServerDialogDevice)
{
    ui->setupUi(this);

    m_mode = EditDevice;

    QStringList ls;

    mbServerDeviceRef::Defaults dDevice = mbServerDeviceRef::Defaults::instance();

    QSpinBox *sp;
    QLineEdit *ln;
    QComboBox *cmb;

    // Devices
    cmb = ui->cmbDevice;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentDevice(int)));

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

    // Units
    ln = ui->lnUnits;
    ln->setText(dDevice.units);
    // Count 0x
    sp = ui->spCount0x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(dDevice.count0x);
    // Count 1x
    sp = ui->spCount1x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(dDevice.count1x);
    // Count 3x
    sp = ui->spCount3x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(dDevice.count3x);
    // Count 4x
    sp = ui->spCount4x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setValue(dDevice.count4x);
    // Save Data
    ui->chbSaveData->setChecked(dDevice.isSaveData);
    // Read Only
    ui->chbReadOnly->setChecked(dDevice.isReadOnly);

    // Exception Status Address Type
    cmb = ui->cmbExceptionStatusAddressType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(3);

    // Exception Status Address Offset
    sp = ui->spExceptionStatusAddressOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    // Delay
    sp = ui->spDelay;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
}

mbServerDialogDevice::~mbServerDialogDevice()
{
    delete ui;
}

MBSETTINGS mbServerDialogDevice::cachedSettings() const
{
    const mbServerDeviceRef::Strings &ms = mbServerDeviceRef::Strings::instance();
    const QString &prefix = Strings().settings_prefix;
    MBSETTINGS m = mbCoreDialogDevice::cachedSettings();

    m[prefix+ms.units     ] = ui->lnUnits       ->text     ();
    m[prefix+ms.count0x   ] = ui->spCount0x     ->value    ();
    m[prefix+ms.count1x   ] = ui->spCount1x     ->value    ();
    m[prefix+ms.count3x   ] = ui->spCount3x     ->value    ();
    m[prefix+ms.count4x   ] = ui->spCount4x     ->value    ();
    m[prefix+ms.isSaveData] = ui->chbSaveData   ->isChecked();
    m[prefix+ms.isReadOnly] = ui->chbReadOnly   ->isChecked();
    m[prefix+ms.delay     ] = ui->spDelay       ->value    ();
    m[prefix+ms.maxWriteMultipleRegisters] = m_ui.spMaxWriteMultipleRegisters->value      ();
    m[prefix+ms.maxWriteMultipleRegisters] = m_ui.spMaxWriteMultipleRegisters->value      ();
    m[prefix+ms.maxWriteMultipleRegisters] = m_ui.spMaxWriteMultipleRegisters->value      ();

    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbExceptionStatusAddressType->currentText());
    adr.offset = static_cast<quint16>(ui->spExceptionStatusAddressOffset->value()-1);
    m[prefix+ms.exceptionStatusAddress] = mb::toInt(adr);

    return m;
}

void mbServerDialogDevice::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogDevice::setCachedSettings(m);
    const mbServerDeviceRef::Strings &ms = mbServerDeviceRef::Strings::instance();
    const QString &prefix = Strings().settings_prefix;

    ui->lnUnits    ->setText   (m.value(prefix+ms.units     ).toString());
    ui->spCount0x  ->setValue  (m.value(prefix+ms.count0x   ).toInt   ());
    ui->spCount1x  ->setValue  (m.value(prefix+ms.count1x   ).toInt   ());
    ui->spCount3x  ->setValue  (m.value(prefix+ms.count3x   ).toInt   ());
    ui->spCount4x  ->setValue  (m.value(prefix+ms.count4x   ).toInt   ());
    ui->chbSaveData->setChecked(m.value(prefix+ms.isSaveData).toBool  ());
    ui->chbReadOnly->setChecked(m.value(prefix+ms.isReadOnly).toBool  ());
    ui->spDelay    ->setValue  (m.value(prefix+ms.delay     ).toInt   ());

    mb::Address adr = mb::toAddress(m.value(prefix+ms.exceptionStatusAddress).toInt());
    ui->cmbExceptionStatusAddressType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
    ui->spExceptionStatusAddressOffset->setValue(adr.offset+1);
}

void mbServerDialogDevice::fillForm(const MBSETTINGS& settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS sets = settings;
    MBSETTINGS::iterator end = sets.end();
    MBSETTINGS::iterator it = sets.find(s.mode);
    if (it != end)
    {
        m_mode = static_cast<Mode>(it.value().toInt());
        sets.erase(it);
    }
    else
        m_mode = EditDevice;

    switch (m_mode)
    {
    case ShowDevices:
        ui->lblDevice->setVisible(true);
        ui->cmbDevice->setVisible(true);
        ui->lblUnits->setVisible(true);
        ui->lnUnits->setVisible(true);
        setEditEnabled(false);
        fillFormShowDevices(sets);
        break;
    case EditDeviceRef:
        ui->lblDevice->setVisible(false);
        ui->cmbDevice->setVisible(false);
        ui->lblUnits->setVisible(true);
        ui->lnUnits->setVisible(true);
        setEditEnabled(true);
        if (sets.count())
        {
            const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();
            ui->lnUnits->setText(sets[s.units].toString());
            fillFormDevice(sets);
        }
        break;
    default:
        ui->lblDevice->setVisible(false);
        ui->cmbDevice->setVisible(false);
        ui->lblUnits->setVisible(false);
        ui->lnUnits->setVisible(false);
        setEditEnabled(true);
        if (sets.count())
            fillFormDevice(sets);
        break;
    }
}

void mbServerDialogDevice::fillData(MBSETTINGS &settings) const
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    switch (m_mode)
    {
    case ShowDevices:
        fillDataShowDevices(settings);
        break;
    case EditDeviceRef:
        fillDataDevice(settings);
        settings[s.units] = ui->lnUnits->text();
        break;
    default:
        fillDataDevice(settings);
        break;
    }
}

void mbServerDialogDevice::fillFormShowDevices(const MBSETTINGS &settings)
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    mbServerProject *project = mbServer::global()->project();
    if (project)
    {
        QComboBox *cmb = ui->cmbDevice;
        cmb->clear();
        Q_FOREACH(mbServerDevice *d, project->devices())
            cmb->addItem(d->name());
    }

    ui->cmbDevice->setCurrentText(settings.value(s.name ).toString());
    ui->lnUnits  ->setText       (settings.value(s.units).toString());
}

void mbServerDialogDevice::fillDataShowDevices(MBSETTINGS &settings) const
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    settings[s.name ] = ui->lnName ->text();
    settings[s.units] = ui->lnUnits->text();
}

void mbServerDialogDevice::fillFormDevice(const MBSETTINGS &settings)
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    mbCoreDialogDevice::fillForm(settings);

    ui->spCount0x  ->setValue  (settings.value(s.count0x   ).toInt());
    ui->spCount1x  ->setValue  (settings.value(s.count1x   ).toInt());
    ui->spCount3x  ->setValue  (settings.value(s.count3x   ).toInt());
    ui->spCount4x  ->setValue  (settings.value(s.count4x   ).toInt());
    ui->chbSaveData->setChecked(settings.value(s.isSaveData).toBool());
    ui->chbReadOnly->setChecked(settings.value(s.isReadOnly).toBool());
    ui->spDelay    ->setValue  (settings.value(s.delay     ).toInt());

    mb::Address adr = mb::toAddress(settings.value(s.exceptionStatusAddress).toInt());
    ui->cmbExceptionStatusAddressType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
    ui->spExceptionStatusAddressOffset->setValue(adr.offset+1);

}

void mbServerDialogDevice::fillDataDevice(MBSETTINGS &settings) const
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbExceptionStatusAddressType->currentText());
    adr.offset = static_cast<quint16>(ui->spExceptionStatusAddressOffset->value()-1);

    mbCoreDialogDevice::fillData(settings);

    settings[s.count0x     ] = ui->spCount0x     ->value    ();
    settings[s.count1x     ] = ui->spCount1x     ->value    ();
    settings[s.count3x     ] = ui->spCount3x     ->value    ();
    settings[s.count4x     ] = ui->spCount4x     ->value    ();
    settings[s.isSaveData  ] = ui->chbSaveData   ->isChecked();
    settings[s.isReadOnly  ] = ui->chbReadOnly   ->isChecked();
    settings[s.delay       ] = ui->spDelay       ->value    ();

    settings[s.exceptionStatusAddress   ] = mb::toInt(adr);
}

void mbServerDialogDevice::setEditEnabled(bool enabled)
{
    ui->tabSettings->setEnabled(enabled);
    ui->tabAdvanced->setEnabled(enabled);
    ui->tabData    ->setEnabled(enabled);
}

void mbServerDialogDevice::setCurrentDevice(int i)
{
    mbServerProject *project = mbServer::global()->project();
    if (project)
    {
        mbServerDevice *d = project->device(i);
        if (d)
            fillFormDevice(d->settings());
    }
}

