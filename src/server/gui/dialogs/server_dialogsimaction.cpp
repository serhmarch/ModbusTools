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
#include "server_dialogsimaction.h"
#include "ui_server_dialogsimaction.h"

#include <QMetaEnum>

#include <server.h>
#include <project/server_project.h>
#include <project/server_simaction.h>

#include <gui/widgets/core_addresswidget.h>

mbServerDialogSimAction::Strings::Strings() :
    title      (QStringLiteral("Action")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Action.")),
    count      (QStringLiteral("actionCount"))
{
}

const mbServerDialogSimAction::Strings &mbServerDialogSimAction::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogSimAction::mbServerDialogSimAction(QWidget *parent) :
    mbCoreDialogEdit(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbServerDialogSimAction)
{
    ui->setupUi(this);

    m_address = new mbCoreAddressWidget();
    ui->formLayout->setWidget(1, QFormLayout::FieldRole, m_address);

    m_addressCopy = new mbCoreAddressWidget();
    qobject_cast<QFormLayout*>(ui->pgCopy->layout())->setWidget(0, QFormLayout::FieldRole, m_addressCopy);

    connect(mbCore::globalCore(), &mbCore::addressNotationChanged, this, &mbServerDialogSimAction::setModbusAddresNotation);
    setModbusAddresNotation(mbCore::globalCore()->addressNotation());

    mbServerSimAction::Defaults d = mbServerSimAction::Defaults::instance();

    QStringList ls;
    QMetaEnum e;
    QSpinBox *sp;
    QComboBox *cmb;
    //QLineEdit *ln;

    // Device
    cmb = ui->cmbDevice;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)));

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
    e = mb::metaEnum<mbServerSimAction::ActionType>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setActionType(int)));
    cmb->setCurrentIndex(mbServerSimAction::Defaults::instance().actionType);
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

    // Action Copy address type + Offset
    sp = ui->spCopySize;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);

    //--------------------- ADVANCED ---------------------
    // Byte Order
    cmb = ui->cmbByteOrder;
    ls = mb::enumDataOrderKeyList();
    Q_FOREACH (const QString &s, ls)
        cmb->addItem(s);
    cmb->setCurrentIndex(0);

    // Register Order
    cmb = ui->cmbRegisterOrder;
    ls = mb::enumRegisterOrderKeyList();
    for (int i = 0; i < ls.count(); i++)
        cmb->addItem(ls.at(i));
    cmb->setCurrentIndex(0);

    //===================================================

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mbServerDialogSimAction::~mbServerDialogSimAction()
{
    delete ui;
}

MBSETTINGS mbServerDialogSimAction::cachedSettings() const
{
    const mbServerSimAction::Strings &vs = mbServerSimAction::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS m = mbCoreDialogEdit::cachedSettings();
    mb::Address adr = modbusAddress();
    mb::Address adrCopy = modbusAddressCopy();

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

void mbServerDialogSimAction::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogEdit::setCachedSettings(m);

    const mbServerSimAction::Strings &vs = mbServerSimAction::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    it = m.find(prefix+vs.address);
    if (it != end)
    {
        setModbusAddress(it.value());
    }

    it = m.find(prefix+vs.copySourceAddress);
    if (it != end)
    {
        setModbusAddressCopy(it.value());
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
    it = m.find(prefix+vs.actionType       ); if (it != end) ui->cmbActionType->setCurrentText(mb::enumKey(mb::enumValue<mbServerSimAction::ActionType>(it.value())));
    it = m.find(prefix+vs.byteOrder        ); if (it != end) fillFormByteOrder(mb::enumDataOrderValue(it.value()));
    it = m.find(prefix+vs.registerOrder    ); if (it != end) fillFormRegisterOrder(mb::toRegisterOrder(it.value()));

    it = m.find(prefix+ds.count            ); if (it != end) ui->spCount->setValue    (it.value().toInt());
}

MBSETTINGS mbServerDialogSimAction::getSettings(const MBSETTINGS &settings, const QString &title)
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

void mbServerDialogSimAction::fillForm(const MBSETTINGS &settings)
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
        const mbServerSimAction::Strings &sItem = mbServerSimAction::Strings::instance();

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
            setModbusAddress(it.value());
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
            mb::RegisterOrder registerOrder = mb::toRegisterOrder(it.value());
            fillFormRegisterOrder(registerOrder);
        }
    }
    else // new data
    {
        ui->spCount->setDisabled(false);
    }
}

void mbServerDialogSimAction::fillFormActionType(const MBSETTINGS &settings)
{
    const mbServerSimAction::Strings &sItem = mbServerSimAction::Strings::instance();
    mbServerSimAction::ActionType t = mb::enumValue<mbServerSimAction::ActionType>(settings.value(sItem.actionType), mbServerSimAction::Increment);

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    switch (t)
    {
    case mbServerSimAction::Increment:
        ui->lnActionIncrement->setText(settings.value(sItem.incrementValue).toString());
        ui->lnActionIncrementMin->setText(settings.value(sItem.incrementMin).toString());
        ui->lnActionIncrementMax->setText(settings.value(sItem.incrementMax).toString());
        break;
    case mbServerSimAction::Sine:
        it = settings.find(sItem.sinePeriod       ); if (it != end) ui->lnActionSinePeriod       ->setText(it.value().toString());
        it = settings.find(sItem.sinePhaseShift   ); if (it != end) ui->lnActionSinePhaseShift   ->setText(it.value().toString());
        it = settings.find(sItem.sineAmplitude    ); if (it != end) ui->lnActionSineAmplitude    ->setText(it.value().toString());
        it = settings.find(sItem.sineVerticalShift); if (it != end) ui->lnActionSineVerticalShift->setText(it.value().toString());
        break;
    case mbServerSimAction::Random:
        it = settings.find(sItem.randomMin); if (it != end) ui->lnActionRandomMin->setText(it.value().toString());
        it = settings.find(sItem.randomMax); if (it != end) ui->lnActionRandomMax->setText(it.value().toString());
        break;
    case mbServerSimAction::Copy:
    {
        it = settings.find(sItem.copySourceAddress);
        if (it != end)
        {
            setModbusAddressCopy(it.value());
        }

        it = settings.find(sItem.copySize);
        if (it != end)
            ui->spCopySize->setValue(it.value().toInt());
    }
        break;
    }
    ui->cmbActionType->setCurrentText(mb::enumKey<mbServerSimAction::ActionType>(t));
}

void mbServerDialogSimAction::fillFormByteOrder(mb::DataOrder e, mbServerDevice *dev)
{
    QComboBox* cmb = ui->cmbByteOrder;
    if (!dev)
    {
        mbServerProject *project = mbServer::global()->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::toString(dev->byteOrder()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumDataOrderKey(mb::DefaultOrder));
    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumDataOrderKey(e));
}

void mbServerDialogSimAction::fillFormRegisterOrder(mb::RegisterOrder e, mbServerDevice *dev)
{
    QComboBox* cmb = ui->cmbRegisterOrder;
    if (!dev)
    {
        mbServerProject *project = mbServer::global()->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::toString(dev->registerOrder()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumRegisterOrderKey(mb::DefaultRegisterOrder));

    if (e == mb::DefaultRegisterOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::toString(e));
}


void mbServerDialogSimAction::fillData(MBSETTINGS &settings)
{
    const mbServerSimAction::Strings &sItem = mbServerSimAction::Strings::instance();
    mbServerProject* project = mbServer::global()->project();
    mb::Address adr = modbusAddress();
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

void mbServerDialogSimAction::fillDataActionType(MBSETTINGS &settings)
{
    const mbServerSimAction::Strings &sItem = mbServerSimAction::Strings::instance();
    mbServerSimAction::ActionType t = static_cast<mbServerSimAction::ActionType>(ui->cmbActionType->currentIndex());
    switch (t)
    {
    case mbServerSimAction::Increment:
        settings[sItem.incrementValue] = ui->lnActionIncrement->text();
        settings[sItem.incrementMin  ] = ui->lnActionIncrementMin->text();
        settings[sItem.incrementMax  ] = ui->lnActionIncrementMax->text();
        break;
    case mbServerSimAction::Sine:
        settings[sItem.sinePeriod       ] = ui->lnActionSinePeriod->text();
        settings[sItem.sinePhaseShift   ] = ui->lnActionSinePhaseShift->text();
        settings[sItem.sineAmplitude    ] = ui->lnActionSineAmplitude->text();
        settings[sItem.sineVerticalShift] = ui->lnActionSineVerticalShift->text();
        break;
    case mbServerSimAction::Random:
        settings[sItem.randomMin] = ui->lnActionRandomMin->text();
        settings[sItem.randomMax] = ui->lnActionRandomMax->text();
        break;
    case mbServerSimAction::Copy:
    {
        mb::Address adr = modbusAddressCopy();
        settings[sItem.copySourceAddress] = mb::toInt(adr);
        settings[sItem.copySize         ] = ui->spCopySize->value();
    }
        break;
    }
    settings[sItem.actionType] = t;
}

void mbServerDialogSimAction::fillDataByteOrder(MBSETTINGS &settings)
{
    settings[mbServerSimAction::Strings::instance().byteOrder] = mb::enumDataOrderValue(ui->cmbByteOrder->currentText());
}

void mbServerDialogSimAction::fillDataRegisterOrder(MBSETTINGS &settings)
{
    settings[mbServerSimAction::Strings::instance().registerOrder] = mb::enumRegisterOrderValue(ui->cmbRegisterOrder->currentText());
}

mb::Address mbServerDialogSimAction::modbusAddress() const
{
    return m_address->getAddress();
}

void mbServerDialogSimAction::setModbusAddress(const QVariant &v)
{
    m_address->setAddress(mb::toAddress(v.toInt()));
}

mb::Address mbServerDialogSimAction::modbusAddressCopy() const
{
    return m_addressCopy->getAddress();
}

void mbServerDialogSimAction::setModbusAddressCopy(const QVariant &v)
{
    m_addressCopy->setAddress(mb::toAddress(v.toInt()));
}

void mbServerDialogSimAction::setModbusAddresNotation(mb::AddressNotation notation)
{
    m_address->setAddressNotation(notation);
    m_addressCopy->setAddressNotation(notation);

}

void mbServerDialogSimAction::deviceChanged(int i)
{
    mbServerProject *project = mbServer::global()->project();
    if (!project)
        return;
    mbServerDevice *dev = project->device(i);

    mb::DataOrder bo = mb::enumDataOrderValueByIndex(ui->cmbRegisterOrder->currentIndex());
    mb::RegisterOrder ro = mb::enumRegisterOrderValueByIndex(ui->cmbRegisterOrder->currentIndex());
    fillFormByteOrder(bo, dev);
    fillFormRegisterOrder(ro, dev);
}

void mbServerDialogSimAction::setActionType(int i)
{
    ui->swActionType->setCurrentIndex(i);
}

