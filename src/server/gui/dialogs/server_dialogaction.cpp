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
#include "server_dialogaction.h"
#include "ui_server_dialogaction.h"

#include <QMetaEnum>

#include <server.h>
#include <project/server_project.h>
#include <project/server_action.h>

mbServerDialogAction::Strings::Strings() :
    title      (QStringLiteral("Action")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Action.")),
    count      (QStringLiteral("actionCount"))
{
}

const mbServerDialogAction::Strings &mbServerDialogAction::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogAction::mbServerDialogAction(QWidget *parent) :
    mbCoreDialogSettings(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbServerDialogAction)
{
    ui->setupUi(this);
    mbServerAction::Defaults d = mbServerAction::Defaults::instance();

    QStringList ls;
    QMetaEnum e;
    QSpinBox *sp;
    QComboBox *cmb;
    //QLineEdit *ln;

    // Device
    //cmb = ui->cmbDevice;
    //connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)));

    // Address type
    cmb = ui->cmbAdrType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(3);

    // Offset
    sp = ui->spOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    // Data type
    cmb = ui->cmbDataType;
    ls = mb::enumDataTypeKeyList();
    Q_FOREACH (const QString &s, ls)
        cmb->addItem(s);
    cmb->setCurrentText(mb::enumDataTypeKey(mb::Int16));

    // Period
    sp = ui->spPeriod;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.period);

    // Count
    sp = ui->spCount;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);

    // Action type
    cmb = ui->cmbActionType;
    e = mb::metaEnum<mbServerAction::ActionType>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setActionType(int)));
    cmb->setCurrentIndex(mbServerAction::Defaults::instance().actionType);
    setActionType(cmb->currentIndex());

    // Action Increment
    ui->lnActionIncrement->setText(QString::number(d.incrementValue));
    ui->lnActionIncrementMin->setText(QString::number(d.incrementMin));
    ui->lnActionIncrementMax->setText(QString::number(d.incrementMax));

    // Action Sine
    ui->lnActionSinePeriod->setText(QString::number(d.sinePeriod));
    ui->lnActionSinePhaseShift->setText(QString::number(d.sinePhaseShift));
    ui->lnActionSineAmplitude->setText(QString::number(d.sineAmplitude));
    ui->lnActionSineVerticalShift->setText(QString::number(d.sineVerticalShift));

    // Action Random
    ui->lnActionRandomMin->setText(QString::number(d.randomMin));
    ui->lnActionRandomMax->setText(QString::number(d.randomMax));

    // Action Copy
    cmb = ui->cmbCopySourceAdrType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(2);

    sp = ui->spCopySourceOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    sp = ui->spCopySize;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);

    //--------------------- ADVANCED ---------------------
    // Byte Order
    cmb = ui->cmbByteOrder;
    ls = mb::enumDataOrderKeyList();
    for (int i = 1; i < ls.count(); i++)  // i = 1 (i != 0) => pass 'DefaultOrder' for byte order
        cmb->addItem(ls.at(i));
    cmb->setCurrentIndex(0);

    // Register Order
    cmb = ui->cmbRegisterOrder;
    ls = mb::enumDataOrderKeyList();
    for (int i = 1; i < ls.count(); i++)  // i = 1 (i != 0) => pass 'DefaultOrder' for byte order
        cmb->addItem(ls.at(i));
    cmb->setCurrentIndex(0);

    //===================================================

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mbServerDialogAction::~mbServerDialogAction()
{
    delete ui;
}

MBSETTINGS mbServerDialogAction::cachedSettings() const
{
    const mbServerAction::Strings &vs = mbServerAction::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS m = mbCoreDialogSettings::cachedSettings();
    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbAdrType->currentText());
    adr.offset = static_cast<quint16>(ui->spOffset->value()-1);

    mb::Address adrCopy;
    adrCopy.type = mb::toModbusMemoryType(ui->cmbCopySourceAdrType->currentText());
    adrCopy.offset = static_cast<quint16>(ui->spCopySourceOffset->value()-1);

    m[prefix+vs.device           ] = ui->cmbDevice->currentText();
    m[prefix+vs.address          ] = mb::toInt(adr);
    m[prefix+vs.dataType         ] = ui->cmbDataType->currentText();
    m[prefix+vs.period           ] = ui->spPeriod->value();
    m[prefix+vs.incrementValue   ] = ui->lnActionIncrement->text();
    m[prefix+vs.incrementMin     ] = ui->lnActionIncrementMin->text();
    m[prefix+vs.incrementMax     ] = ui->lnActionIncrementMax->text();
    m[prefix+vs.sinePeriod       ] = ui->lnActionSinePeriod->text();
    m[prefix+vs.sinePhaseShift   ] = ui->lnActionSinePhaseShift->text();
    m[prefix+vs.sineAmplitude    ] = ui->lnActionSineAmplitude->text();
    m[prefix+vs.sineVerticalShift] = ui->lnActionSineVerticalShift->text();
    m[prefix+vs.randomMin        ] = ui->lnActionRandomMin->text();
    m[prefix+vs.randomMax        ] = ui->lnActionRandomMax->text();
    m[prefix+vs.copySourceAddress] = mb::toInt(adrCopy);
    m[prefix+vs.copySize         ] = ui->spCopySize->value();
    m[prefix+vs.actionType       ] = ui->cmbActionType->currentText();
    m[prefix+vs.byteOrder        ] = ui->cmbByteOrder->currentText();
    m[prefix+vs.registerOrder    ] = ui->cmbRegisterOrder->currentText();
    m[prefix+ds.count            ] = ui->spCount->value();

    return m;
}

void mbServerDialogAction::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogSettings::setCachedSettings(m);

    const mbServerAction::Strings &vs = mbServerAction::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    it = m.find(prefix+vs.address);
    if (it != end)
    {
        mb::Address adr = mb::toAddress(it.value().toInt());
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
        ui->spOffset->setValue(adr.offset+1);
    }

    it = m.find(prefix+vs.copySourceAddress);
    if (it != end)
    {
        mb::Address adr = mb::toAddress(it.value().toInt());
        ui->cmbCopySourceAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
        ui->spCopySourceOffset->setValue(adr.offset+1);
    }

    it = m.find(prefix+vs.device           ); if (it != end) ui->cmbDevice  ->setCurrentText(it.value().toString());
    it = m.find(prefix+vs.dataType         ); if (it != end) ui->cmbDataType->setCurrentText(it.value().toString());
    it = m.find(prefix+vs.period           ); if (it != end) ui->spPeriod   ->setValue      (it.value().toInt()   );
    it = m.find(prefix+vs.incrementValue   ); if (it != end) ui->lnActionIncrement->setText(it.value().toString());
    it = m.find(prefix+vs.incrementMin     ); if (it != end) ui->lnActionIncrementMin->setText(it.value().toString());
    it = m.find(prefix+vs.incrementMax     ); if (it != end) ui->lnActionIncrementMax->setText(it.value().toString());
    it = m.find(prefix+vs.sinePeriod       ); if (it != end) ui->lnActionSinePeriod->setText(it.value().toString());
    it = m.find(prefix+vs.sinePhaseShift   ); if (it != end) ui->lnActionSinePhaseShift->setText(it.value().toString());
    it = m.find(prefix+vs.sineAmplitude    ); if (it != end) ui->lnActionSineAmplitude->setText(it.value().toString());
    it = m.find(prefix+vs.sineVerticalShift); if (it != end) ui->lnActionSineVerticalShift->setText(it.value().toString());
    it = m.find(prefix+vs.randomMin        ); if (it != end) ui->lnActionRandomMin->setText(it.value().toString());
    it = m.find(prefix+vs.randomMax        ); if (it != end) ui->lnActionRandomMax->setText(it.value().toString());
    it = m.find(prefix+vs.copySize         ); if (it != end) ui->spCopySize->setValue(it.value().toInt());
    it = m.find(prefix+vs.actionType       ); if (it != end) ui->cmbActionType->setCurrentText(mb::enumKey(mb::enumValue<mbServerAction::ActionType>(it.value())));
    it = m.find(prefix+vs.byteOrder        ); if (it != end) fillFormByteOrder(mb::enumDataOrderValue(it.value()));
    it = m.find(prefix+vs.registerOrder    ); if (it != end) fillFormRegisterOrder(mb::enumDataOrderValue(it.value()));

    it = m.find(prefix+ds.count            ); if (it != end) ui->spCount->setValue    (it.value().toInt());
}

MBSETTINGS mbServerDialogAction::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;

    mbServerProject *project = mbServer::global()->project();
    if (!project)
        return r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(settings);
    // show main tab
    ui->tabWidget->setCurrentIndex(0);

    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbServerDialogAction::fillForm(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    mbServerProject *project = mbServer::global()->project();

    // fill 'Device'-combobox
    QComboBox* cmb = ui->cmbDevice;
    cmb->clear();
    Q_FOREACH (mbServerDevice* d, project->devices())
        cmb->addItem(d->name());

    int count = settings.value(s.count, 0).toInt();
    if (count > 0) // edit data
    {
        MBSETTINGS::const_iterator it;
        MBSETTINGS::const_iterator end = settings.end();
        const mbServerAction::Strings &sItem = mbServerAction::Strings::instance();

        ui->spCount->setValue(count);
        ui->spCount->setDisabled(true);

        it = settings.find(sItem.device);
        if (it != end)
        {
            mbServerDevice *dev = reinterpret_cast<mbServerDevice*>(it.value().value<void*>());
            if (dev)
                cmb->setCurrentText(dev->name());
        }

        it = settings.find(sItem.address);
        if (it != end)
        {
            mb::Address adr = mb::toAddress(settings.value(sItem.address).toInt());
            ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
            ui->spOffset->setValue(adr.offset+1);
        }

        it = settings.find(sItem.dataType);
        if (it != end)
        {
            mb::DataType dataType = mb::enumDataTypeValue(it.value());
            ui->cmbDataType->setCurrentText(mb::enumDataTypeKey(dataType));
        }

        it = settings.find(sItem.period);
        if (it != end)
            ui->spPeriod->setValue(it.value().toInt());

        fillFormActionType(settings);

        it = settings.find(sItem.byteOrder);
        if (it != end)
        {
            mb::DataOrder byteOrder = mb::enumDataOrderValue(it.value());
            fillFormByteOrder(byteOrder);
        }

        it = settings.find(sItem.registerOrder);
        if (it != end)
        {
            mb::DataOrder registerOrder = mb::enumDataOrderValue(it.value());
            fillFormRegisterOrder(registerOrder);
        }
    }
    else // new data
    {
        ui->spCount->setDisabled(false);
    }
}

void mbServerDialogAction::fillFormActionType(const MBSETTINGS &settings)
{
    const mbServerAction::Strings &sItem = mbServerAction::Strings::instance();
    mbServerAction::ActionType t = mb::enumValue<mbServerAction::ActionType>(settings.value(sItem.actionType), mbServerAction::Increment);

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    switch (t)
    {
    case mbServerAction::Increment:
        ui->lnActionIncrement->setText(settings.value(sItem.incrementValue).toString());
        ui->lnActionIncrementMin->setText(settings.value(sItem.incrementMin).toString());
        ui->lnActionIncrementMax->setText(settings.value(sItem.incrementMax).toString());
        break;
    case mbServerAction::Sine:
        it = settings.find(sItem.sinePeriod       ); if (it != end) ui->lnActionSinePeriod       ->setText(it.value().toString());
        it = settings.find(sItem.sinePhaseShift   ); if (it != end) ui->lnActionSinePhaseShift   ->setText(it.value().toString());
        it = settings.find(sItem.sineAmplitude    ); if (it != end) ui->lnActionSineAmplitude    ->setText(it.value().toString());
        it = settings.find(sItem.sineVerticalShift); if (it != end) ui->lnActionSineVerticalShift->setText(it.value().toString());
        break;
    case mbServerAction::Random:
        it = settings.find(sItem.randomMin); if (it != end) ui->lnActionRandomMin->setText(it.value().toString());
        it = settings.find(sItem.randomMax); if (it != end) ui->lnActionRandomMax->setText(it.value().toString());
        break;
    case mbServerAction::Copy:
    {
        it = settings.find(sItem.copySourceAddress);
        if (it != end)
        {
            mb::Address adr = mb::toAddress(it.value().toInt());
            ui->cmbCopySourceAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
            ui->spCopySourceOffset->setValue(adr.offset+1);
        }

        it = settings.find(sItem.copySize);
        if (it != end)
            ui->spCopySize->setValue(it.value().toInt());
    }
        break;
    }
    ui->cmbActionType->setCurrentText(mb::enumKey<mbServerAction::ActionType>(t));
}

void mbServerDialogAction::fillFormByteOrder(mb::DataOrder e)
{
    QComboBox* cmb = ui->cmbByteOrder;
    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumDataOrderKey(e));
}

void mbServerDialogAction::fillFormRegisterOrder(mb::DataOrder e)
{
    QComboBox* cmb = ui->cmbRegisterOrder;
    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumDataOrderKey(e));
}


void mbServerDialogAction::fillData(MBSETTINGS &settings)
{
    const mbServerAction::Strings &sItem = mbServerAction::Strings::instance();
    mbServerProject* project = mbServer::global()->project();
    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbAdrType->currentText());
    adr.offset = static_cast<quint16>(ui->spOffset->value()-1);
    mb::DataType dataType = mb::enumDataTypeValueByIndex(ui->cmbDataType->currentIndex());
    settings[sItem.device      ] = QVariant::fromValue<void*>(project->device(ui->cmbDevice->currentText()));
    settings[sItem.address     ] = mb::toInt(adr);
    settings[sItem.dataType    ] = dataType;
    settings[sItem.period      ] = ui->spPeriod->value();
    settings[sItem.actionType  ] = ui->cmbActionType->currentIndex();

    settings[Strings::instance().count] = ui->spCount->value();

    fillDataActionType   (settings);
    fillDataByteOrder    (settings);
    fillDataRegisterOrder(settings);
}

void mbServerDialogAction::fillDataActionType(MBSETTINGS &settings)
{
    const mbServerAction::Strings &sItem = mbServerAction::Strings::instance();
    mbServerAction::ActionType t = static_cast<mbServerAction::ActionType>(ui->cmbActionType->currentIndex());
    switch (t)
    {
    case mbServerAction::Increment:
        settings[sItem.incrementValue] = ui->lnActionIncrement->text();
        settings[sItem.incrementMin  ] = ui->lnActionIncrementMin->text();
        settings[sItem.incrementMax  ] = ui->lnActionIncrementMax->text();
        break;
    case mbServerAction::Sine:
        settings[sItem.sinePeriod       ] = ui->lnActionSinePeriod->text();
        settings[sItem.sinePhaseShift   ] = ui->lnActionSinePhaseShift->text();
        settings[sItem.sineAmplitude    ] = ui->lnActionSineAmplitude->text();
        settings[sItem.sineVerticalShift] = ui->lnActionSineVerticalShift->text();
        break;
    case mbServerAction::Random:
        settings[sItem.randomMin] = ui->lnActionRandomMin->text();
        settings[sItem.randomMax] = ui->lnActionRandomMax->text();
        break;
    case mbServerAction::Copy:
    {
        mb::Address adr;
        adr.type = mb::toModbusMemoryType(ui->cmbCopySourceAdrType->currentText());
        adr.offset = static_cast<quint16>(ui->spCopySourceOffset->value()-1);
        settings[sItem.copySourceAddress] = mb::toInt(adr);
        settings[sItem.copySize         ] = ui->spCopySize->value();
    }
        break;
    }
    settings[sItem.actionType] = t;
}

void mbServerDialogAction::fillDataByteOrder(MBSETTINGS &settings)
{
    settings[mbServerAction::Strings::instance().byteOrder] = mb::enumDataOrderValue(ui->cmbByteOrder->currentText());
}

void mbServerDialogAction::fillDataRegisterOrder(MBSETTINGS &settings)
{
    settings[mbServerAction::Strings::instance().registerOrder] = mb::enumDataOrderValue(ui->cmbRegisterOrder->currentIndex());
}

void mbServerDialogAction::setActionType(int i)
{
    ui->swActionType->setCurrentIndex(i);
}

