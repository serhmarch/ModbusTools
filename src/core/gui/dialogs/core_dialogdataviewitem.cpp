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
#include "core_dialogdataviewitem.h"

#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QToolButton>
#include <QTabWidget>
#include <QDialogButtonBox>

#include <QMetaEnum>

#include <core.h>
#include <project/core_project.h>
#include <project/core_device.h>
#include <project/core_dataview.h>

mbCoreDialogDataViewItem::Strings::Strings() :
    title(QStringLiteral("Item(s)")),
    count(QStringLiteral("count"))
{
}

const mbCoreDialogDataViewItem::Strings &mbCoreDialogDataViewItem::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogDataViewItem::mbCoreDialogDataViewItem(QWidget *parent) :
    mbCoreDialogSettings(parent)
{
    const mbCoreDataViewItem::Defaults &d = mbCoreDataViewItem::Defaults::instance();

    m_variableLength = d.variableLength;
    setNonDefaultByteArraySeparator(d.byteArraySeparator);
    m_formatLast = static_cast<mb::Format>(-1);

    memset(&m_ui, 0, sizeof(m_ui));
}

void mbCoreDialogDataViewItem::initializeBaseUi()
{
    QMetaEnum e;
    QSpinBox *sp;
    QComboBox *cmb;
    QLineEdit *ln;

    // Device
    cmb = m_ui.cmbDevice;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(deviceChanged(int)));

    // Address type
    cmb = m_ui.cmbAdrType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(3);

    // Offset
    sp = m_ui.spOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    // Count
    sp = m_ui.spCount;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);

    // Format
    cmb = m_ui.cmbFormat;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(formatChanged(int)));
    e = mb::metaEnum<mb::Format>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentText(mb::enumKeyTypeStr<mb::Format>(mb::Dec16));

    // Length
    sp = m_ui.spLength;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);

    //--------------------- ADVANCED ---------------------
    // Byte Order
    cmb = m_ui.cmbByteOrder;
    e = mb::metaEnum<mb::DataOrder>();
    for (int i = 1; i < e.keyCount(); i++)  // i = 1 (i != 0) => pass 'DefaultOrder' for byte order
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // Register Order
    cmb = m_ui.cmbRegisterOrder;
    e = mb::metaEnum<mb::DataOrder>();
    for (int i = 0; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // ByteArray format
    cmb = m_ui.cmbByteArrayFormat;
    e = mb::metaEnum<mb::DigitalFormat>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));

    // ByteArray separator
    ln = m_ui.lnByteArraySeparator;
    ln->setText(nonDefaultByteArraySeparator());
    connect(m_ui.btnTogleDefaultByteArraySeparator, SIGNAL(clicked()), this, SLOT(togleDefaultByteArraySeparator()));


    // String Length Type
    cmb = m_ui.cmbStringLengthType;
    e = mb::metaEnum<mb::StringLengthType>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    // String Encoding
    cmb = m_ui.cmbStringEncoding;
    e = mb::metaEnum<mb::StringEncoding>();
    for (int i = 0 ; i < e.keyCount(); i++)
        cmb->addItem(QString(e.key(i)));
    cmb->setCurrentIndex(0);

    //===================================================

    connect(m_ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

MBSETTINGS mbCoreDialogDataViewItem::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(settings);
    // show main tab
    m_ui.tabWidget->setCurrentIndex(0);

    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbCoreDialogDataViewItem::fillFormInner(const MBSETTINGS &settings)
{
    // base implementation do nothing
}

void mbCoreDialogDataViewItem::fillDataInner(MBSETTINGS &settings)
{
    // base implementation do nothing
}

void mbCoreDialogDataViewItem::fillForm(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    mbCoreProject* project = mbCore::globalCore()->projectCore();

    // fill 'Device'-combobox
    QComboBox* cmb = m_ui.cmbDevice;
    cmb->clear();
    Q_FOREACH(mbCoreDevice* d, project->devicesCore())
        cmb->addItem(d->name());

    int count = settings.value(s.count, 0).toInt();
    if (count > 0) // edit data
    {
        const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();

        mbCoreDevice *dev = reinterpret_cast<mbCoreDevice*>(settings.value(sItem.device).value<void*>());
        if (dev)
            cmb->setCurrentText(dev->name());
        if (settings.contains(sItem.variableLength))
            setVariableLength(settings.value(sItem.variableLength).toInt());

        bool ok;
        mb::Address adr = mb::toAddress(settings.value(sItem.address).toInt());
        m_ui.cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(adr.type));
        m_ui.spOffset->setValue(adr.offset+1);

        mb::Format format = mb::enumValue<mb::Format>(settings.value(sItem.format), &ok);
        if (ok)
            m_ui.cmbFormat->setCurrentText(mb::enumKeyTypeStr<mb::Format>(format));

        m_ui.spCount->setValue(count);

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

        fillFormInner(settings);
        m_ui.spCount->setDisabled(true);
    }
    else // new data
    {
        deviceChanged(m_ui.cmbDevice->currentIndex());
        m_ui.spCount->setDisabled(false);
    }
}

void mbCoreDialogDataViewItem::fillFormByteOrder(mb::DataOrder e)
{
    QComboBox* cmb = m_ui.cmbByteOrder;
    if (e == mb::DefaultOrder)
        cmb->setCurrentIndex(0);
    else
        cmb->setCurrentText(mb::enumKey<mb::DataOrder>(e));
}

void mbCoreDialogDataViewItem::fillFormRegisterOrder(mb::DataOrder e, mbCoreDevice *dev)
{
    QComboBox* cmb = m_ui.cmbRegisterOrder;
    if (!dev)
    {
        mbCoreProject* project = mbCore::globalCore()->projectCore();
        if (project)
            dev = project->deviceCore(m_ui.cmbDevice->currentIndex());
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

void mbCoreDialogDataViewItem::fillFormByteArrayFormat(mb::DigitalFormat e, mbCoreDevice *dev)
{
    QComboBox* cmb = m_ui.cmbByteArrayFormat;
    if (!dev)
    {
        mbCoreProject* project = mbCore::globalCore()->projectCore();
        if (project)
            dev = project->deviceCore(m_ui.cmbDevice->currentIndex());
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

void mbCoreDialogDataViewItem::fillFormByteArraySeparator(const QString &s, mbCoreDevice *dev)
{
    QLineEdit *ln = m_ui.lnByteArraySeparator;

    if (mb::isDefaultStringValue(s))
    {
        if (!dev)
        {
            mbCoreProject* project = mbCore::globalCore()->projectCore();
            if (project)
                dev = project->deviceCore(m_ui.cmbDevice->currentIndex());
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

void mbCoreDialogDataViewItem::fillFormStringLengthType(mb::StringLengthType e, mbCoreDevice *dev)
{
    QComboBox* cmb = m_ui.cmbStringLengthType;
    if (!dev)
    {
        mbCoreProject* project = mbCore::globalCore()->projectCore();
        if (project)
            dev = project->deviceCore(m_ui.cmbDevice->currentIndex());
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

void mbCoreDialogDataViewItem::fillFormStringEncoding(mb::StringEncoding e, mbCoreDevice *dev)
{
    QComboBox* cmb = m_ui.cmbStringEncoding;
    if (!dev)
    {
        mbCoreProject* project = mbCore::globalCore()->projectCore();
        if (project)
            dev = project->deviceCore(m_ui.cmbDevice->currentIndex());
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

void mbCoreDialogDataViewItem::fillData(MBSETTINGS &settings)
{
    mbCoreProject *project = mbCore::globalCore()->projectCore();
    if (!project)
        return;

    const Strings &s = Strings::instance();
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();

    mb::Address adr;
    adr.type = mb::toModbusMemoryType(m_ui.cmbAdrType->currentText());
    adr.offset = static_cast<quint16>(m_ui.spOffset->value()-1);
    mb::Format format = mb::enumValueByIndex<mb::Format>(m_ui.cmbFormat->currentIndex());
    settings[sItem.device] =  QVariant::fromValue<void*>(project->deviceCore(m_ui.cmbDevice->currentIndex()));
    settings[sItem.address] = mb::toInt(adr);
    settings[sItem.format] = format;
    switch (format)
    {
    case mb::ByteArray:
    case mb::String:
    {
        int len = m_ui.spLength->value();
        settings[sItem.variableLength] = len;
    }
    break;
    default:
        break;
    }
    settings[s.count] = m_ui.spCount->value();
    fillDataByteOrder(settings);
    fillDataRegisterOrder(settings);
    fillDataByteArrayFormat(settings);
    fillDataByteArraySeparator(settings);
    fillDataStringLengthType(settings);
    fillDataStringEncoding(settings);
    fillDataInner(settings);
}

void mbCoreDialogDataViewItem::fillDataByteOrder(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    settings[sItem.byteOrder] = mb::enumValueType<mb::DataOrder>(m_ui.cmbByteOrder->currentText());
}

void mbCoreDialogDataViewItem::fillDataRegisterOrder(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::DataOrder>();
    QComboBox* cmb = m_ui.cmbRegisterOrder;
    settings[sItem.registerOrder] = static_cast<mb::DataOrder>(me.value(cmb->currentIndex()));
}

void mbCoreDialogDataViewItem::fillDataByteArrayFormat(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::DigitalFormat>();
    QComboBox* cmb = m_ui.cmbByteArrayFormat;
    settings[sItem.byteArrayFormat] = static_cast<mb::DigitalFormat>(me.value(cmb->currentIndex()));
}

void mbCoreDialogDataViewItem::fillDataByteArraySeparator(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    QString s;
    if (isDefaultByteArraySeparator())
        s = mb::Defaults::instance().default_string_value;
    else
    {
        setNonDefaultByteArraySeparator(m_ui.lnByteArraySeparator->text());
        s = nonDefaultByteArraySeparator();
    }
    settings[sItem.byteArraySeparator] = s;
}

void mbCoreDialogDataViewItem::fillDataStringLengthType(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::StringLengthType>();
    QComboBox* cmb = m_ui.cmbStringLengthType;
    settings[sItem.stringLengthType] = static_cast<mb::StringLengthType>(me.value(cmb->currentIndex()));
}

void mbCoreDialogDataViewItem::fillDataStringEncoding(MBSETTINGS &settings)
{
    const mbCoreDataViewItem::Strings &sItem = mbCoreDataViewItem::Strings::instance();
    const QMetaEnum me = mb::metaEnum<mb::StringEncoding>();
    QComboBox* cmb = m_ui.cmbStringEncoding;
    settings[sItem.stringEncoding] = static_cast<mb::StringEncoding>(me.value(cmb->currentIndex()));
}

void mbCoreDialogDataViewItem::deviceChanged(int i)
{
    mbCoreProject *project = mbCore::globalCore()->projectCore();
    if (!project)
        return;
    mbCoreDevice *dev = project->deviceCore(i);

    mb::DataOrder ro = mb::enumValueByIndex<mb::DataOrder>(m_ui.cmbRegisterOrder->currentIndex());
    fillFormRegisterOrder(ro, dev);

    mb::DigitalFormat byteArrayFormat = mb::enumValueByIndex<mb::DigitalFormat>(m_ui.cmbByteArrayFormat->currentIndex());
    fillFormByteArrayFormat(byteArrayFormat, dev);

    if (isDefaultByteArraySeparator())
        fillFormByteArraySeparator(mb::Defaults::instance().default_string_value, dev);

    mb::StringLengthType slt = mb::enumValueByIndex<mb::StringLengthType>(m_ui.cmbStringLengthType->currentIndex());
    fillFormStringLengthType(slt, dev);

    mb::StringEncoding se = mb::enumValueByIndex<mb::StringEncoding>(m_ui.cmbStringEncoding->currentIndex());
    fillFormStringEncoding(se, dev);
}

void mbCoreDialogDataViewItem::adrTypeChanged(int i)
{
    QMetaEnum e = QMetaEnum::fromType<Modbus::MemoryType>();
    switch (static_cast<Modbus::MemoryType>(e.value(i)))
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        m_ui.cmbFormat->setCurrentText(mb::enumKeyTypeStr<mb::Format>(mb::Bin16));
        m_ui.cmbFormat->setEnabled(false);
        break;
    case Modbus::Memory_3x:
    case Modbus::Memory_4x:
        m_ui.cmbFormat->setEnabled(true);
        break;
    }
}

void mbCoreDialogDataViewItem::formatChanged(int i)
{
    QMetaEnum e = mb::metaEnum<mb::Format>();
    mb::Format f = static_cast<mb::Format>(e.value(i));
    switch (f)
    {
    case mb::ByteArray:
    case mb::String:
        if ((m_formatLast != mb::ByteArray) && (m_formatLast != mb::String))
        {
            this->connect(m_ui.spLength, SIGNAL(valueChanged(int)), SLOT(lengthChanged(int)));
        }
        m_ui.spLength->setEnabled(true);
        m_ui.spLength->setValue(m_variableLength);
        break;
    default:
        if ((m_formatLast == mb::ByteArray) || (m_formatLast == mb::String))
        {
            //m_variableLength = m_ui.spLength->value();
            m_ui.spLength->disconnect(this);
        }
        m_ui.spLength->setValue(static_cast<int>(mb::sizeofFormat(f)));
        m_ui.spLength->setEnabled(false);
        break;
    }
    m_formatLast = f;
}

void mbCoreDialogDataViewItem::lengthChanged(int i)
{
    m_variableLength = i;
}

void mbCoreDialogDataViewItem::togleDefaultByteArraySeparator()
{
    if (isDefaultByteArraySeparator())
        fillFormByteArraySeparator(nonDefaultByteArraySeparator());
    else
        fillFormByteArraySeparator(mb::Defaults::instance().default_string_value);
}

void mbCoreDialogDataViewItem::setVariableLength(int len)
{
    mb::Format format = mb::enumValueByIndex<mb::Format>(m_ui.cmbFormat->currentIndex());
    m_variableLength = len;
    switch (format)
    {
    case mb::ByteArray:
    case mb::String:
        m_ui.spLength->setValue(m_variableLength);
        break;
    default:
        break;
    }
}

bool mbCoreDialogDataViewItem::isDefaultByteArraySeparator()
{
    return !m_ui.lnByteArraySeparator->isEnabled();
}

void mbCoreDialogDataViewItem::setNonDefaultByteArraySeparator(const QString &s)
{
    m_nonDefaultByteArraySeparator = mb::makeEscapeSequnces(s);
}

