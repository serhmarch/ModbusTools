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

#include <gui/widgets/core_addresswidget.h>

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
    m_addressExcStatus = new mbCoreAddressWidget();
    ui->layoutExceptionStatus->addWidget(m_addressExcStatus);
    m_mode = EditDevice;

    QStringList ls;

    mbServerDeviceRef::Defaults dDevice = mbServerDeviceRef::Defaults::instance();

    QSpinBox *sp;
    QLineEdit *ln;
    QComboBox *cmb;

    connect(mbServer::global(), &mbServer::addressNotationChanged, this, &mbServerDialogDevice::setModbusAddresNotation);
    setModbusAddresNotation(mbServer::global()->addressNotation());

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
    m_ui.cmbByteOrder                = ui->cmbByteOrder               ;
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
    sp->setMaximum(USHRT_MAX+1);
    sp->setValue(dDevice.count0x);
    // Count 1x
    sp = ui->spCount1x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX+1);
    sp->setValue(dDevice.count1x);
    // Count 3x
    sp = ui->spCount3x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX+1);
    sp->setValue(dDevice.count3x);
    // Count 4x
    sp = ui->spCount4x;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX+1);
    sp->setValue(dDevice.count4x);
    // Save Data
    ui->chbSaveData->setChecked(dDevice.isSaveData);
    // Read Only
    ui->chbReadOnly->setChecked(dDevice.isReadOnly);

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
    const QString &prefix = Strings().cachePrefix;
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

    mb::Address adr = modbusExceptionStatusAddress();
    m[prefix+ms.exceptionStatusAddress] = mb::toInt(adr);

    return m;
}

void mbServerDialogDevice::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogDevice::setCachedSettings(m);
    const mbServerDeviceRef::Strings &vs = mbServerDeviceRef::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.units     ); if (it != end) ui->lnUnits    ->setText   (it.value().toString());
    it = m.find(prefix+vs.count0x   ); if (it != end) ui->spCount0x  ->setValue  (it.value().toInt   ());
    it = m.find(prefix+vs.count1x   ); if (it != end) ui->spCount1x  ->setValue  (it.value().toInt   ());
    it = m.find(prefix+vs.count3x   ); if (it != end) ui->spCount3x  ->setValue  (it.value().toInt   ());
    it = m.find(prefix+vs.count4x   ); if (it != end) ui->spCount4x  ->setValue  (it.value().toInt   ());
    it = m.find(prefix+vs.isSaveData); if (it != end) ui->chbSaveData->setChecked(it.value().toBool  ());
    it = m.find(prefix+vs.isReadOnly); if (it != end) ui->chbReadOnly->setChecked(it.value().toBool  ());
    it = m.find(prefix+vs.delay     ); if (it != end) ui->spDelay    ->setValue  (it.value().toInt   ());

    it = m.find(prefix+vs.exceptionStatusAddress);
    if (it != end)
    {
        setModbusExceptionStatusAddress(it.value());
    }
}

void mbServerDialogDevice::fillForm(const MBSETTINGS& settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS m = settings;
    MBSETTINGS::iterator end = m.end();
    MBSETTINGS::iterator it = m.find(s.mode);
    if (it != end)
    {
        m_mode = static_cast<Mode>(it.value().toInt());
        m.erase(it);
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
        fillFormShowDevices(m);
        break;
    case EditDeviceRef:
        ui->lblDevice->setVisible(false);
        ui->cmbDevice->setVisible(false);
        ui->lblUnits->setVisible(true);
        ui->lnUnits->setVisible(true);
        setEditEnabled(true);
        if (m.count())
        {
            const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();
            ui->lnUnits->setText(m[s.units].toString());
            fillFormDevice(m);
        }
        break;
    default:
        ui->lblDevice->setVisible(false);
        ui->cmbDevice->setVisible(false);
        ui->lblUnits->setVisible(false);
        ui->lnUnits->setVisible(false);
        setEditEnabled(true);
        if (m.count())
            fillFormDevice(m);
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

void mbServerDialogDevice::fillFormShowDevices(const MBSETTINGS &m)
{
    const mbServerDeviceRef::Strings &vs = mbServerDeviceRef::Strings::instance();
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    mbServerProject *project = mbServer::global()->project();
    if (project)
    {
        QComboBox *cmb = ui->cmbDevice;
        cmb->clear();
        Q_FOREACH(mbServerDevice *d, project->devices())
            cmb->addItem(d->name());
    }

    it = m.find(vs.name ); if (it != end) ui->cmbDevice->setCurrentText(it.value().toString());
    it = m.find(vs.units); if (it != end) ui->lnUnits  ->setText       (it.value().toString());
}

void mbServerDialogDevice::fillDataShowDevices(MBSETTINGS &m) const
{
    const mbServerDeviceRef::Strings &vs = mbServerDeviceRef::Strings::instance();

    m[vs.name ] = ui->lnName ->text();
    m[vs.units] = ui->lnUnits->text();
}

void mbServerDialogDevice::fillFormDevice(const MBSETTINGS &m)
{
    const mbServerDeviceRef::Strings &vs = mbServerDeviceRef::Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    mbCoreDialogDevice::fillForm(m);

    it = m.find(vs.count0x   ); if (it != end) ui->spCount0x  ->setValue  (it.value().toInt ());
    it = m.find(vs.count1x   ); if (it != end) ui->spCount1x  ->setValue  (it.value().toInt ());
    it = m.find(vs.count3x   ); if (it != end) ui->spCount3x  ->setValue  (it.value().toInt ());
    it = m.find(vs.count4x   ); if (it != end) ui->spCount4x  ->setValue  (it.value().toInt ());
    it = m.find(vs.isSaveData); if (it != end) ui->chbSaveData->setChecked(it.value().toBool());
    it = m.find(vs.isReadOnly); if (it != end) ui->chbReadOnly->setChecked(it.value().toBool());
    it = m.find(vs.delay     ); if (it != end) ui->spDelay    ->setValue  (it.value().toInt ());

    it = m.find(vs.exceptionStatusAddress);
    if (it != end)
    {
        setModbusExceptionStatusAddress(it.value());
    }
}

void mbServerDialogDevice::fillDataDevice(MBSETTINGS &settings) const
{
    const mbServerDeviceRef::Strings &s = mbServerDeviceRef::Strings::instance();

    mb::Address adr = modbusExceptionStatusAddress();

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

mb::Address mbServerDialogDevice::modbusExceptionStatusAddress() const
{
    return m_addressExcStatus->getAddress();
}

void mbServerDialogDevice::setModbusExceptionStatusAddress(const QVariant &v)
{
    m_addressExcStatus->setAddress(mb::toAddress(v.toInt()));
}

void mbServerDialogDevice::setModbusAddresNotation(mb::AddressNotation notation)
{
    m_addressExcStatus->setAddressNotation(notation);
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

