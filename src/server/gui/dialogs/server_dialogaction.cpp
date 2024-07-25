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
    title(QStringLiteral("Action")),
    count(QStringLiteral("Server.Ui.Dialogs.Action.Count"))
{
}

const mbServerDialogAction::Strings &mbServerDialogAction::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogAction::mbServerDialogAction(QWidget *parent) :
    QDialog(parent),
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
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);

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

    // Simulation type
    cmb = ui->cmbActionType;
    e = mb::metaEnum<mbServerAction::ActionType>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setActionType(int)));
    cmb->setCurrentIndex(mbServerAction::Defaults::instance().actionType);
    setActionType(cmb->currentIndex());

    // Simulation Increment
    ui->lnActionIncrement->setText(QString::number(d.incrementValue));

    // Simulation Sine
    ui->lnActionSinePeriod->setText(QString::number(d.sinePeriod));
    ui->lnActionSinePhaseShift->setText(QString::number(d.sinePhaseShift));
    ui->lnActionSineAmplitude->setText(QString::number(d.sineAmplitude));
    ui->lnActionSineVerticalShift->setText(QString::number(d.sineVerticalShift));

    // Simulation Random
    ui->lnActionRandomMin->setText(QString::number(d.randomMin));
    ui->lnActionRandomMax->setText(QString::number(d.randomMax));

    //--------------------- ADVANCED ---------------------
    // Byte Order
    cmb = ui->cmbByteOrder;
    ls = mb::enumDataTypeKeyList();
    for (int i = 1; i < ls.count(); i++)  // i = 1 (i != 0) => pass 'DefaultOrder' for byte order
        cmb->addItem(ls.at(i));
    cmb->setCurrentIndex(0);

    // Register Order
    cmb = ui->cmbRegisterOrder;
    ls = mb::enumDataTypeKeyList();
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
        const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
        mbServerDevice *dev = reinterpret_cast<mbServerDevice*>(settings.value(sAction.device).value<void*>());
        if (dev)
            cmb->setCurrentText(dev->name());

        mb::Address adr = mb::toAddress(settings.value(sAction.address).toInt());
        mb::DataType dataType = static_cast<mb::DataType>(settings.value(sAction.dataType).toInt());
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
        ui->spOffset->setValue(adr.offset+1);
        ui->cmbDataType->setCurrentText(mb::enumDataTypeKey(dataType));
        ui->spPeriod->setValue(settings.value(sAction.period).toInt());
        ui->spCount->setValue(count);
        fillFormActionType(settings);
        mb::DataOrder byteOrder = static_cast<mb::DataOrder>(settings.value(sAction.byteOrder).toInt());
        fillFormByteOrder(byteOrder);

        mb::DataOrder registerOrder = static_cast<mb::DataOrder>(settings.value(sAction.registerOrder).toInt());
        fillFormRegisterOrder(registerOrder);

        ui->spCount->setDisabled(true);
    }
    else // new data
    {
        ui->spCount->setDisabled(false);
    }
}

void mbServerDialogAction::fillFormActionType(const MBSETTINGS &settings)
{
    const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
    mbServerAction::ActionType t = mb::enumValue<mbServerAction::ActionType>(settings.value(sAction.actionType), mbServerAction::Increment);
    switch (t)
    {
    case mbServerAction::Increment:
        ui->lnActionIncrement->setText(settings.value(sAction.incrementValue).toString());
        break;
    case mbServerAction::Sine:
        ui->lnActionSinePeriod->setText(settings.value(sAction.sinePeriod).toString());
        ui->lnActionSinePhaseShift->setText(settings.value(sAction.sinePhaseShift).toString());
        ui->lnActionSineAmplitude->setText(settings.value(sAction.sineAmplitude).toString());
        ui->lnActionSineVerticalShift->setText(settings.value(sAction.sineVerticalShift).toString());
        break;
    case mbServerAction::Random:
        ui->lnActionRandomMin->setText(settings.value(sAction.randomMin).toString());
        ui->lnActionRandomMax->setText(settings.value(sAction.randomMax).toString());
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
    const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
    mbServerProject* project = mbServer::global()->project();
    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbAdrType->currentText());
    adr.offset = static_cast<quint16>(ui->spOffset->value()-1);
    mb::DataType dataType = mb::enumDataTypeValueByIndex(ui->cmbDataType->currentIndex());
    settings[sAction.device      ] = QVariant::fromValue<void*>(project->device(ui->cmbDevice->currentText()));
    settings[sAction.address     ] = mb::toInt(adr);
    settings[sAction.dataType    ] = dataType;
    settings[sAction.period      ] = ui->spPeriod->value();
    settings[sAction.actionType  ] = ui->cmbActionType->currentIndex();

    settings[Strings::instance().count] = ui->spCount->value();

    fillDataActionType   (settings);
    fillDataByteOrder    (settings);
    fillDataRegisterOrder(settings);
}

void mbServerDialogAction::fillDataActionType(MBSETTINGS &settings)
{
    const mbServerAction::Strings &sAction = mbServerAction::Strings::instance();
    mbServerAction::ActionType t = static_cast<mbServerAction::ActionType>(ui->cmbActionType->currentIndex());
    switch (t)
    {
    case mbServerAction::Increment:
        settings[sAction.incrementValue] = ui->lnActionIncrement->text();
        break;
    case mbServerAction::Sine:
        settings[sAction.sinePeriod       ] = ui->lnActionSinePeriod->text();
        settings[sAction.sinePhaseShift   ] = ui->lnActionSinePhaseShift->text();
        settings[sAction.sineAmplitude    ] = ui->lnActionSineAmplitude->text();
        settings[sAction.sineVerticalShift] = ui->lnActionSineVerticalShift->text();
        break;
    case mbServerAction::Random:
        settings[sAction.randomMin] = ui->lnActionRandomMin->text();
        settings[sAction.randomMax] = ui->lnActionRandomMax->text();
        break;
    }
    settings[sAction.actionType] = t;
}

void mbServerDialogAction::fillDataByteOrder(MBSETTINGS &settings)
{
    settings[mbServerAction::Strings::instance().byteOrder] = mb::enumDataOrderValue(ui->cmbByteOrder->currentText());
}

void mbServerDialogAction::fillDataRegisterOrder(MBSETTINGS &settings)
{
    QComboBox* cmb = ui->cmbRegisterOrder;
    settings[mbServerAction::Strings::instance().registerOrder] = mb::enumDataOrderValueByIndex(cmb->currentIndex());
}

void mbServerDialogAction::setActionType(int i)
{
    ui->swActionType->setCurrentIndex(i);
}

