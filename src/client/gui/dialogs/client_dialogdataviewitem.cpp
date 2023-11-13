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
#include "client_dialogdataviewitem.h"
#include "ui_client_dialogdataviewitem.h"

#include <QMetaEnum>

#include <client.h>
#include <project/client_project.h>
#include <project/client_device.h>
#include <project/client_dataview.h>

mbClientDialogDataViewItem::mbClientDialogDataViewItem(QWidget *parent) :
    mbCoreDialogDataViewItem(parent),
    ui(new Ui::mbClientDialogDataViewItem)
{
    ui->setupUi(this);

    const mbClientDataViewItem::Defaults &d = mbClientDataViewItem::Defaults::instance();
    m_variableLength = d.variableLength;
    setNonDefaultByteArraySeparator(d.byteArraySeparator);

    QMetaEnum e;
    QSpinBox *sp;
    QComboBox *cmb;
    QLineEdit *ln;

    // Device
    cmb = ui->cmbDevice;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)));

    // Address type
    cmb = ui->cmbAdrType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(3);
    //e = QMetaEnum::fromType<Modbus::MemoryType>();
    //for (int i = 0; i < e.keyCount(); i++)
    //    cmb->addItem(QString(e.key(i)));
    //connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(adrTypeChanged(int)));

    // Offset
    sp = ui->spOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    // Count
    sp = ui->spCount;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);

    // Format
    m_formatLast = static_cast<mb::Format>(-1);
    cmb = ui->cmbFormat;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
    e = mb::metaEnum<mb::Format>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));

    // Length
    sp = ui->spCount;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);

    // Period
    sp = ui->spPeriod;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);

    //--------------------- ADVANCED ---------------------
    // Byte Order
    cmb = ui->cmbByteOrder;
    e = mb::metaEnum<mb::DataOrder>();
    for (int i = 1; i < e.keyCount(); i++)  // i = 1 (i != 0) => pass 'DefaultOrder' for byte order
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // Register Order
    cmb = ui->cmbRegisterOrder;
    e = mb::metaEnum<mb::DataOrder>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // ByteArray format
    cmb = ui->cmbByteArrayFormat;
    e = mb::metaEnum<mb::DigitalFormat>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));

    // ByteArray separator
    ln = ui->lnByteArraySeparator;
    ln->setText(nonDefaultByteArraySeparator());
    connect(ui->btnTogleDefaultByteArraySeparator, SIGNAL(clicked()), this, SLOT(togleDefaultByteArraySeparator()));


    // String Length Type
    cmb = ui->cmbStringLengthType;
    e = mb::metaEnum<mb::StringLengthType>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // String Encoding
    cmb = ui->cmbStringEncoding;
    e = mb::metaEnum<mb::StringEncoding>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    //===================================================
    ui->cmbAdrType->setCurrentText(mb::enumKeyTypeStr<Modbus::MemoryType>(Modbus::Memory_4x));
    ui->cmbFormat->setCurrentText(mb::enumKeyTypeStr<mb::Format>(mb::Dec16));
}

mbClientDialogDataViewItem::~mbClientDialogDataViewItem()
{
    delete ui;
}

MBSETTINGS mbClientDialogDataViewItem::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;

    mbClient *core = mbClient::global();
    mbClientProject* project = core->project();
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

void mbClientDialogDataViewItem::fillForm(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    mbClientProject* project = mbClient::global()->project();

    // fill 'Device'-combobox
    QComboBox* cmb = ui->cmbDevice;
    cmb->clear();
    Q_FOREACH (mbClientDevice* d, project->devices())
        cmb->addItem(d->name());

    int count = settings.value(s.count, 0).toInt();
    if (count > 0) // alter data
    {
        const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();

        mbClientDevice *dev = reinterpret_cast<mbClientDevice*>(settings.value(sItem.device).value<void*>());
        if (dev)
            cmb->setCurrentText(dev->name());
        if (settings.contains(sItem.variableLength))
            setVariableLength(settings.value(sItem.variableLength).toInt());

        bool ok;
        mb::Address adr = mb::toAddress(settings.value(sItem.address).toInt());
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
        ui->spOffset->setValue(adr.offset+1);

        mb::Format format = mb::enumValue<mb::Format>(settings.value(sItem.format), &ok);
        if (ok)
            ui->cmbFormat->setCurrentText(mb::enumKeyTypeStr<mb::Format>(format));

        int period = settings.value(sItem.period).toInt();
        ui->spPeriod->setValue(period);
        ui->spCount->setValue(count);

        mb::DataOrder byteOrder = mb::enumValue<mb::DataOrder>(settings.value(sItem.byteOrder), &ok);
        if (ok)
            fillFormByteOrder(byteOrder);

        mb::DataOrder registerOrder = mb::enumValue<mb::DataOrder>(settings.value(sItem.registerOrder), &ok);
        if (ok)
            fillFormRegisterOrder(registerOrder, dev);

        mb::DigitalFormat byteArrayFormat = mb::enumValue<mb::DigitalFormat>(settings.value(sItem.byteArrayFormat), &ok);
        if (ok)
            fillFormByteArrayFormat(byteArrayFormat, dev);

        QString byteArraySeparator = settings.value(sItem.byteArraySeparator).toString();
        fillFormByteArraySeparator(byteArraySeparator, dev);

        mb::StringLengthType stringLengthType = mb::enumValue<mb::StringLengthType>(settings.value(sItem.stringLengthType), &ok);
        if (ok)
            fillFormStringLengthType(stringLengthType, dev);

        mb::StringEncoding stringEncoding = mb::enumValue<mb::StringEncoding>(settings.value(sItem.stringEncoding), &ok);
        if (ok)
            fillFormStringEncoding(stringEncoding, dev);

        ui->spCount->setDisabled(true);
    }
    else // new data
    {
        deviceChanged(ui->cmbDevice->currentIndex());
        ui->spCount->setDisabled(false);
    }
}

void mbClientDialogDataViewItem::fillFormByteOrder(mb::DataOrder e)
{
    QComboBox* cmb = ui->cmbByteOrder;
    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::DataOrder>(e));
}

void mbClientDialogDataViewItem::fillFormRegisterOrder(mb::DataOrder e, mbClientDevice *dev)
{
    mbClient *core = mbClient::global();
    QComboBox* cmb = ui->cmbRegisterOrder;
    if (!dev)
    {
        mbClientProject *project = core->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::enumKey<mb::DataOrder>(dev->registerOrder()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumKey<mb::DataOrder>(mb::DefaultOrder));

    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::DataOrder>(e));
}

void mbClientDialogDataViewItem::fillFormByteArrayFormat(mb::DigitalFormat e, mbClientDevice *dev)
{
    mbClient *core = mbClient::global();
    QComboBox* cmb = ui->cmbByteArrayFormat;
    if (!dev)
    {
        mbClientProject *project = core->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::enumKey<mb::DigitalFormat>(dev->byteArrayFormat()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumKey<mb::DigitalFormat>(mb::DefaultDigitalFormat));

    if (e == mb::DefaultDigitalFormat)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::DigitalFormat>(e));
}

void mbClientDialogDataViewItem::fillFormByteArraySeparator(const QString &s, mbClientDevice *dev)
{
    mbClient *core = mbClient::global();
    QLineEdit *ln = ui->lnByteArraySeparator;

    if (mb::isDefaultStringValue(s))
    {
        if (!dev)
        {
            mbClientProject *project = core->project();
            if (project)
                dev = project->device(ui->cmbDevice->currentIndex());
        }
        QString deviceByteArraySeparator;
        if (dev)
            deviceByteArraySeparator = QString("Default(%1)").arg(dev->byteArraySeparatorStr());
        else
            deviceByteArraySeparator = QStringLiteral("Default");
        ln->setText(deviceByteArraySeparator);
        ln->setEnabled(false);
    }
    else
    {
        setNonDefaultByteArraySeparator(s);
        ln->setText(nonDefaultByteArraySeparator());
        ln->setEnabled(true);
    }
}

void mbClientDialogDataViewItem::fillFormStringLengthType(mb::StringLengthType e, mbClientDevice *dev)
{
    mbClient *core = mbClient::global();
    QComboBox* cmb = ui->cmbStringLengthType;
    if (!dev)
    {
        mbClientProject *project = core->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::enumKey<mb::StringLengthType>(dev->stringLengthType()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumKey<mb::StringLengthType>(mb::DefaultStringLengthType));

    if (e == mb::DefaultStringLengthType)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::StringLengthType>(e));
}

void mbClientDialogDataViewItem::fillFormStringEncoding(mb::StringEncoding e, mbClientDevice *dev)
{
    mbClient *core = mbClient::global();
    QComboBox* cmb = ui->cmbStringEncoding;
    if (!dev)
    {
        mbClientProject *project = core->project();
        if (project)
            dev = project->device(ui->cmbDevice->currentIndex());
    }
    if (dev)
    {
        QString s = QString("Default(%1)").arg(mb::enumKey<mb::StringEncoding>(dev->stringEncoding()));
        cmb->setItemText(0, s);
    }
    else
        cmb->setItemText(0, mb::enumKey<mb::StringEncoding>(mb::DefaultStringEncoding));

    if (e == mb::DefaultStringEncoding)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::StringEncoding>(e));
}

void mbClientDialogDataViewItem::fillData(MBSETTINGS &settings)
{
    mbClientProject *project = mbClient::global()->project();
    if (!project)
        return;

    const Strings &s = Strings::instance();
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();

    mb::Address adr;
    adr.type = mb::toModbusMemoryType(ui->cmbAdrType->currentText());
    adr.offset = static_cast<quint16>(ui->spOffset->value()-1);
    mb::Format format = mb::enumValueByIndex<mb::Format>(ui->cmbFormat->currentIndex());
    settings[sItem.device] =  QVariant::fromValue<void*>(project->device(ui->cmbDevice->currentIndex()));
    settings[sItem.address] = mb::toInt(adr);
    settings[sItem.format] = format;
    if (format == mb::String)
    {
        int len = ui->spLength->value();
        settings[sItem.variableLength] = len;
    }
    settings[sItem.period] = ui->spPeriod->value();
    settings[s.count] = ui->spCount->value();
    fillDataByteOrder(settings);
    fillDataRegisterOrder(settings);
    fillDataByteArrayFormat(settings);
    fillDataByteArraySeparator(settings);
    fillDataStringLengthType(settings);
    fillDataStringEncoding(settings);
}

void mbClientDialogDataViewItem::fillDataByteOrder(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();

    settings[sItem.byteOrder] = mb::enumValueType<mb::DataOrder>(ui->cmbByteOrder->currentText());
}

void mbClientDialogDataViewItem::fillDataRegisterOrder(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::DataOrder>();
    QComboBox* cmb = ui->cmbRegisterOrder;
    settings[sItem.registerOrder] = static_cast<mb::DataOrder>(me.value(cmb->currentIndex()));
}

void mbClientDialogDataViewItem::fillDataByteArrayFormat(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::DigitalFormat>();
    QComboBox* cmb = ui->cmbByteArrayFormat;
    settings[sItem.byteArrayFormat] = static_cast<mb::DigitalFormat>(me.value(cmb->currentIndex()));
}

void mbClientDialogDataViewItem::fillDataByteArraySeparator(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    QString s;
    if (isDefaultByteArraySeparator())
        s = mb::Defaults::instance().default_string_value;
    else
    {
        setNonDefaultByteArraySeparator(ui->lnByteArraySeparator->text());
        s = nonDefaultByteArraySeparator();
    }
    settings[sItem.byteArraySeparator] = s;
}

void mbClientDialogDataViewItem::fillDataStringLengthType(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::StringLengthType>();
    QComboBox* cmb = ui->cmbStringLengthType;
    settings[sItem.stringLengthType] = static_cast<mb::StringLengthType>(me.value(cmb->currentIndex()));
}

void mbClientDialogDataViewItem::fillDataStringEncoding(MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::StringEncoding>();
    QComboBox* cmb = ui->cmbStringEncoding;
    settings[sItem.stringEncoding] = static_cast<mb::StringEncoding>(me.value(cmb->currentIndex()));
}

void mbClientDialogDataViewItem::deviceChanged(int i)
{
    mbClient *core = mbClient::global();
    mbClientProject *project = core->project();
    if (!project)
        return;
    mbClientDevice *dev = project->device(i);

    mb::DataOrder ro = mb::enumValueByIndex<mb::DataOrder>(ui->cmbRegisterOrder->currentIndex());
    fillFormRegisterOrder(ro, dev);

    mb::DigitalFormat byteArrayFormat = mb::enumValueByIndex<mb::DigitalFormat>(ui->cmbByteArrayFormat->currentIndex());
    fillFormByteArrayFormat(byteArrayFormat, dev);

    if (isDefaultByteArraySeparator())
        fillFormByteArraySeparator(mb::Defaults::instance().default_string_value, dev);

    mb::StringLengthType slt = mb::enumValueByIndex<mb::StringLengthType>(ui->cmbStringLengthType->currentIndex());
    fillFormStringLengthType(slt, dev);

    mb::StringEncoding se = mb::enumValueByIndex<mb::StringEncoding>(ui->cmbStringEncoding->currentIndex());
    fillFormStringEncoding(se, dev);
}

void mbClientDialogDataViewItem::adrTypeChanged(int i)
{
    QMetaEnum e = QMetaEnum::fromType<Modbus::MemoryType>();
    switch (static_cast<Modbus::MemoryType>(e.value(i)))
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        ui->cmbFormat->setCurrentText(mb::enumKeyTypeStr<mb::Format>(mb::Bin16));
        ui->cmbFormat->setEnabled(false);
        break;
    case Modbus::Memory_3x:
    case Modbus::Memory_4x:
        ui->cmbFormat->setEnabled(true);
        break;
    }
}

void mbClientDialogDataViewItem::formatChanged(int i)
{
    QMetaEnum e = mb::metaEnum<mb::Format>();
    mb::Format f = static_cast<mb::Format>(e.value(i));
    switch (f)
    {
    case mb::ByteArray:
    case mb::String:
        if ((m_formatLast != mb::ByteArray) && (m_formatLast != mb::String))
        {
            this->connect(ui->spLength, SIGNAL(valueChanged(int)), SLOT(lengthChanged(int)));
        }
        ui->spLength->setEnabled(true);
        ui->spLength->setValue(m_variableLength);
        break;
    default:
        if ((m_formatLast == mb::ByteArray) || (m_formatLast == mb::String))
        {
            //m_variableLength = ui->spLength->value();
            ui->spLength->disconnect(this);
        }
        ui->spLength->setValue(static_cast<int>(mb::sizeofFormat(f)));
        ui->spLength->setEnabled(false);
        break;
    }
    m_formatLast = f;
}

void mbClientDialogDataViewItem::lengthChanged(int i)
{
    m_variableLength = i;
}

void mbClientDialogDataViewItem::togleDefaultByteArraySeparator()
{
    if (isDefaultByteArraySeparator())
        fillFormByteArraySeparator(nonDefaultByteArraySeparator());
    else
        fillFormByteArraySeparator(mb::Defaults::instance().default_string_value);
}

void mbClientDialogDataViewItem::setVariableLength(int len)
{
    mb::Format format = mb::enumValueByIndex<mb::Format>(ui->cmbFormat->currentIndex());
    m_variableLength = len;
    if (format == mb::String)
        ui->spLength->setValue(m_variableLength);
}

bool mbClientDialogDataViewItem::isDefaultByteArraySeparator()
{
    return !ui->lnByteArraySeparator->isEnabled();
}

void mbClientDialogDataViewItem::setNonDefaultByteArraySeparator(const QString &s)
{
    m_nonDefaultByteArraySeparator = mb::makeEscapeSequnces(s);
}
