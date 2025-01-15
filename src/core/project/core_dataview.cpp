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
#include "core_dataview.h"

#include <QByteArray>

#include <core.h>
#include "core_project.h"

mbCoreDataViewItem::Strings::Strings() :
    device            (QStringLiteral("device")),
    address           (QStringLiteral("address")),
    format            (QStringLiteral("format")),
    comment           (QStringLiteral("comment")),
    variableLength    (QStringLiteral("variableLength")),
    byteOrder         (QStringLiteral("byteOrder")),
    registerOrder     (QStringLiteral("registerOrder")),
    byteArrayFormat   (QStringLiteral("byteArrayFormat")),
    byteArraySeparator(QStringLiteral("byteArraySeparator")),
    stringLengthType  (QStringLiteral("stringLengthType")),
    stringEncoding    (QStringLiteral("stringEncoding")),
    value             (QStringLiteral("value"))
{
}

const mbCoreDataViewItem::Strings &mbCoreDataViewItem::Strings::instance()
{
    static const Strings d;
    return d;
}

mbCoreDataViewItem::Defaults::Defaults() :
    address                     (400001),
    format                      (mb::Dec16),
    comment                     (QString()),
    variableLength              (20),
    byteOrder                   (mb::LessSignifiedFirst),
    registerOrder               (mb::LessSignifiedFirst),
    byteArrayFormat             (mb::Hex),
    byteArraySeparator          (QStringLiteral(" ")),
    isDefaultByteArraySeparator (true),
    stringLengthType            (mb::ZerroEnded),
    stringEncoding              (mb::Defaults::instance().stringEncoding),
    value                       (QVariant())
{
}

const mbCoreDataViewItem::Defaults &mbCoreDataViewItem::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCoreDataViewItem::mbCoreDataViewItem(QObject *parent) : QObject(parent)
{
    Defaults d = Defaults::instance();

    m_dataView = nullptr;

    m_device                      = nullptr;
    m_address                     = mb::toAddress(d.address);
    m_format                      = d.format;
    m_variableLength              = d.variableLength;
    m_byteOrder                   = d.byteOrder;
    m_registerOrder               = d.registerOrder;
    m_byteArrayFormat             = d.byteArrayFormat;
    m_byteArraySeparator          = d.byteArraySeparator;
    m_isDefaultByteArraySeparator = d.isDefaultByteArraySeparator;
    m_stringLengthType            = d.stringLengthType;
    m_stringEncoding              = d.stringEncoding;

    m_isDefaultStringEncoding = true;
}

int mbCoreDataViewItem::bitLength() const
{
    switch (m_address.type)
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        if (m_format == mb::Bool)
            return 1;
        break;
    default:
        break;
    }
    return MB_BYTE_SZ_BITES * byteLength();
}

int mbCoreDataViewItem::byteLength() const
{
    switch (m_format)
    {
    case mb::Bool:
        if ((m_address.type == Modbus::Memory_0x) || (m_address.type == Modbus::Memory_1x))
            return 0;
        else
            return 2;
    case mb::ByteArray:
    case mb::String:
        return m_variableLength;
    default:
        return static_cast<int>(sizeofFormat(m_format));
    }
}

int mbCoreDataViewItem::length() const
{
    switch (m_address.type)
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
        return bitLength();
    default:
        return registerLength();
    }
}

QString mbCoreDataViewItem::addressStr() const
{
    if (m_dataView)
        return mb::toString(m_address, m_dataView->getAddressNotation());
    return mb::toString(m_address);
}

void mbCoreDataViewItem::setAddress(const mb::Address &address)
{
    m_address = address;
    Q_EMIT changed();
}

void mbCoreDataViewItem::setAddress(Modbus::MemoryType type, quint16 offset)
{
    mb::Address adr;
    adr.type = type;
    adr.offset = offset;
    setAddress(adr);
}

void mbCoreDataViewItem::setFormat(mb::Format format)
{
    m_format = format;
    Q_EMIT changed();
}

QString mbCoreDataViewItem::formatStr() const
{
    return mb::enumFormatKey(m_format);
}

void mbCoreDataViewItem::setFormatStr(const QString &formatStr)
{
    bool ok;
    mb::Format v = mb::enumFormatValue(formatStr, &ok);
    if (ok)
        setFormat(v);
}

QString mbCoreDataViewItem::byteOrderStr() const
{
    return mb::enumDataOrderKey(m_byteOrder);
}

void mbCoreDataViewItem::setByteOrderStr(const QString &order)
{
    bool ok;
    mb::DataOrder v = mb::enumDataOrderValue(order, &ok);
    if (ok)
        m_byteOrder = v;
}

QString mbCoreDataViewItem::registerOrderStr() const
{
    return mb::enumDataOrderKey(m_registerOrder);
}

void mbCoreDataViewItem::setRegisterOrderStr(const QString &registerOrderStr)
{
    bool ok;
    mb::DataOrder v = mb::enumDataOrderValue(registerOrderStr, &ok);
    if (ok)
        m_registerOrder = v;
}

QString mbCoreDataViewItem::byteArrayFormatStr() const
{
    return mb::enumDigitalFormatKey(m_byteArrayFormat);
}

void mbCoreDataViewItem::setByteArrayFormatStr(const QString &byteArrayFormatStr)
{
    bool ok;
    mb::DigitalFormat k = mb::enumDigitalFormatValue(byteArrayFormatStr, &ok);
    if (ok)
        m_byteArrayFormat = k;
}

QString mbCoreDataViewItem::byteArraySeparator() const
{
    if (m_isDefaultByteArraySeparator && m_device)
        return m_device->byteArraySeparator();
    return m_byteArraySeparator;
}

void mbCoreDataViewItem::setByteArraySeparator(const QString &byteArraySeparator)
{
    m_byteArraySeparator = byteArraySeparator;
}

QString mbCoreDataViewItem::byteArraySeparatorStr() const
{
    if (m_isDefaultByteArraySeparator)
        return mb::Defaults::instance().default_string_value;
    return mb::makeEscapeSequnces(m_byteArraySeparator);
}

void mbCoreDataViewItem::setByteArraySeparatorStr(const QString &byteArraySeparatorStr)
{
    if (mb::isDefaultStringValue(byteArraySeparatorStr))
        m_isDefaultByteArraySeparator = true;
    else
    {
        m_isDefaultByteArraySeparator = false;
        m_byteArraySeparator = mb::resolveEscapeSequnces(byteArraySeparatorStr);
    }
}

QString mbCoreDataViewItem::stringLengthTypeStr() const
{
    return mb::enumStringLengthTypeKey(m_stringLengthType);
}

void mbCoreDataViewItem::setStringLengthTypeStr(const QString &stringLengthTypeStr)
{
    bool ok;
    mb::StringLengthType k = mb::enumStringLengthTypeValue(stringLengthTypeStr, &ok);
    if (ok)
        m_stringLengthType = k;
}

QString mbCoreDataViewItem::stringEncodingStr() const
{
    if (isDefaultStringEncoding())
        return mb::Defaults::instance().stringEncodingSpecial;
    return mb::fromStringEncoding(m_stringEncoding);
}

void mbCoreDataViewItem::setStringEncodingStr(const QString &stringEncodingStr)
{
    m_isDefaultStringEncoding = (stringEncodingStr == mb::Defaults::instance().stringEncodingSpecial);
    if (m_isDefaultStringEncoding)
        return;
    bool ok;
    mb::StringEncoding k = mb::toStringEncoding(stringEncodingStr, &ok);
    if (ok)
        m_stringEncoding = k;
}

MBSETTINGS mbCoreDataViewItem::settings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS p;
    p[s.device            ] = QVariant::fromValue<void*>(deviceCore());
    p[s.address           ] = addressInt();
    p[s.format            ] = mb::enumKey(format());
    p[s.comment           ] = comment();
    p[s.variableLength    ] = variableLength();
    p[s.byteOrder         ] = mb::enumKey(byteOrder());
    p[s.registerOrder     ] = mb::enumKey(registerOrder());
    p[s.byteArrayFormat   ] = mb::enumKey(byteArrayFormat());
    p[s.byteArraySeparator] = byteArraySeparatorStr();
    p[s.stringLengthType  ] = mb::enumKey(stringLengthType());
    p[s.stringEncoding    ] = stringEncodingStr();
    return p;
}

bool mbCoreDataViewItem::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    bool ok;

    blockSignals(true);

    it = settings.find(s.device);
    if (it != end)
    {
        QVariant var = it.value();
        mbCoreDevice* d = reinterpret_cast<mbCoreDevice*>(var.value<void*>());
        setDeviceCore(d);
    }

    it = settings.find(s.address);
    if (it != end)
    {
        QVariant var = it.value();
        setAddress(var.toInt());
    }

    it = settings.find(s.comment);
    if (it != end)
    {
        QVariant var = it.value();
        setComment(var.toString());
    }

    it = settings.find(s.variableLength);
    if (it != end)
    {
        QVariant var = it.value();
        setVariableLength(var.toInt());
    }

    it = settings.find(s.byteOrder);
    if (it != end)
    {
        mb::DataOrder v = mb::enumDataOrderValue(it.value(), &ok);
        if (ok)
            setByteOrder(v);
    }

    it = settings.find(s.registerOrder);
    if (it != end)
    {
        mb::DataOrder v = mb::enumDataOrderValue(it.value(), &ok);
        if (ok)
            setRegisterOrder(v);
    }

    it = settings.find(s.byteArrayFormat);
    if (it != end)
    {
        mb::DigitalFormat v = mb::enumDigitalFormatValue(it.value(), &ok);
        if (ok)
            setByteArrayFormat(v);
    }

    it = settings.find(s.byteArraySeparator);
    if (it != end)
    {
        QVariant var = it.value();
        setByteArraySeparatorStr(var.toString());
    }

    it = settings.find(s.stringLengthType);
    if (it != end)
    {
        mb::StringLengthType v = mb::enumStringLengthTypeValue(it.value(), &ok);
        if (ok)
            setStringLengthType(v);
    }

    it = settings.find(s.stringEncoding);
    if (it != end)
    {
        QVariant var = it.value();
        setStringEncodingStr(var.toString());
    }

    it = settings.find(s.format);
    if (it != end)
    {
        mb::Format v = mb::enumFormatValue(it.value(), &ok);
        if (ok)
            setFormat(v);
    }

    blockSignals(false);
    Q_EMIT changed();
    return true;
}

QByteArray mbCoreDataViewItem::toByteArray(const QVariant &value) const
{
    return mb::toByteArray(value,
                           m_format,
                           m_address.type,
                           m_byteOrder,
                           getRegisterOrder(),
                           m_byteArrayFormat,
                           getStringEncoding(),
                           getStringLengthType(),
                           byteArraySeparator(),
                           m_variableLength);
}

QVariant mbCoreDataViewItem::toVariant(const QByteArray &v) const
{
    QByteArray data = v;
    if (data.isEmpty())
        data = QByteArray(sizeOf(), '\0');

    return mb::toVariant(data,
                         m_format,
                         m_address.type,
                         m_byteOrder,
                         getRegisterOrder(),
                         m_byteArrayFormat,
                         getStringEncoding(),
                         getStringLengthType(),
                         byteArraySeparator(),
                         m_variableLength);
}

mb::DataOrder mbCoreDataViewItem::getRegisterOrder() const
{
    if (m_registerOrder == mb::DefaultOrder)
    {
        if (m_device && (m_device->registerOrder() != mb::DefaultOrder))
            return m_device->registerOrder();
        return mb::LessSignifiedFirst;
    }
    return m_registerOrder;
}

mb::StringEncoding mbCoreDataViewItem::getStringEncoding() const
{
    if (isDefaultStringEncoding() && m_device)
        return m_device->stringEncoding();
    return m_stringEncoding;
}

mb::StringLengthType mbCoreDataViewItem::getStringLengthType() const
{
    if (m_stringLengthType == mb::DefaultStringLengthType)
    {
        if (m_device && (m_device->stringLengthType() != mb::DefaultStringLengthType))
            return m_device->stringLengthType();
        return mb::ZerroEnded;
    }
    return m_stringLengthType;
}


mbCoreDataView::Strings::Strings() :
    name             (QStringLiteral("name")),
    period           (QStringLiteral("period")),
    addressNotation  (QStringLiteral("addressNotation")),
    useDefaultColumns(QStringLiteral("useDefaultColumns")),
    columns          (QStringLiteral("columns"))
{
}

const mbCoreDataView::Strings &mbCoreDataView::Strings::instance()
{
    static const Strings d;
    return d;
}

mbCoreDataView::Defaults::Defaults() :
    name(QStringLiteral("dataView")),
    period(1000),
    addressNotation(mb::Address_Default),
    useDefaultColumns(true)
{
}

const mbCoreDataView::Defaults &mbCoreDataView::Defaults::instance()
{
    static const Defaults d;
    return d;
}

QStringList mbCoreDataView::availableColumnNames()
{
    QStringList res;
    QMetaEnum me = QMetaEnum::fromType<CoreColumns>();
    for (int i = 0; i < ColumnCount; i++)
        res.append(me.key(i));
    return res;
}

mbCoreDataView::mbCoreDataView(QObject *parent) : QObject(parent)
{
    const Defaults &d = Defaults::instance();
    m_project = nullptr;
    m_period            = d.period;
    m_addressNotation   = d.addressNotation;
    m_useDefaultColumns = false;

    setUseDefaultColumns(d.useDefaultColumns);
    setColumnNames(mbCore::globalCore()->availableDataViewColumns());
}

mbCoreDataView::~mbCoreDataView()
{
    qDeleteAll(m_items);
}

void mbCoreDataView::setName(const QString &name)
{
    QString tn = name;
    if (tn.isEmpty())
        tn = Defaults::instance().name;
    if (m_project && m_project->dataViewCore(tn) != this)
    {
        if (m_project->hasDataView(tn))
            tn = m_project->freeDataViewName(tn);
        m_project->dataViewRename(this, tn);
    }
    setObjectName(name);
    Q_EMIT nameChanged(name);
}

void mbCoreDataView::setPeriod(int period)
{
    if (m_period != period)
    {
        m_period = period;
        Q_EMIT periodChanged(m_period);
    }
}

mb::AddressNotation mbCoreDataView::getAddressNotation() const
{
    if (m_addressNotation == mb::Address_Default)
        return mbCore::globalCore()->addressNotation();
    return m_addressNotation;
}

void mbCoreDataView::setAddressNotation(mb::AddressNotation notation)
{
    if (m_addressNotation != notation)
    {
        m_addressNotation = notation;
        Q_EMIT addressNotationChanged(m_addressNotation);
    }
}

void mbCoreDataView::setUseDefaultColumns(bool use)
{
    if (m_useDefaultColumns != use)
    {
        if (use)
            connect(mbCore::globalCore(), &mbCore::columnsChanged, this, &mbCoreDataView::columnsChanged);
        else
            disconnect(mbCore::globalCore());
        m_useDefaultColumns = use;
        Q_EMIT columnsChanged();
    }
}

int mbCoreDataView::getColumnCount() const
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columnCount();
    return columnCount();
}

QList<int> mbCoreDataView::getColumns() const
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columns();
    return columns();
}

void mbCoreDataView::setColumns(const QList<int> columns)
{
    m_columns = columns;
    if (!m_useDefaultColumns)
        Q_EMIT columnsChanged();
}

QStringList mbCoreDataView::columnNames() const
{
    QStringList res;
    for (int i = 0; i < m_columns.count(); i++)
        res.append(columnNameByIndex(i));
    return res;
}

void mbCoreDataView::setColumnNames(const QStringList &columns)
{
    QList<int> cols;
    Q_FOREACH (const QString &col, columns)
    {
        int type = columnTypeByName(col);
        if (col >= 0)
            cols.append(type);
    }
    setColumns(cols);
}

int mbCoreDataView::columnTypeByIndex(int i) const
{
    return m_columns.value(i, -1);
}

int mbCoreDataView::getColumnTypeByIndex(int i) const
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columnTypeByIndex(i);
    return columnTypeByIndex(i);
}

int mbCoreDataView::columnTypeByName(const QString &name) const
{
    QMetaEnum me = QMetaEnum::fromType<CoreColumns>();
    return me.keyToValue(name.toUtf8().constData());
}

int mbCoreDataView::getColumnTypeByName(const QString &name) const
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columnTypeByName(name);
    return columnTypeByName(name);
}

QString mbCoreDataView::columnNameByIndex(int i) const
{
    int c = m_columns.value(i, -1);
    if (c >= 0)
        return QMetaEnum::fromType<CoreColumns>().key(c);
    return QString();
}

QString mbCoreDataView::getColumnNameByIndex(int i) const
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columnNameByIndex(i);
    return columnNameByIndex(i);
}

int mbCoreDataView::columnIndexByType(int type)
{
    for (int i = 0; i < columnCount(); i++)
    {
        if (m_columns.at(i) == type)
            return i;
    }
    return -1;
}

int mbCoreDataView::getColumnIndexByType(int type)
{
    if (m_useDefaultColumns)
        return mbCore::globalCore()->columnIndexByType(type);
    return columnIndexByType(type);
}

MBSETTINGS mbCoreDataView::settings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS p;
    p[s.name             ] = name();
    p[s.period           ] = period();
    p[s.addressNotation  ] = addressNotation();
    p[s.useDefaultColumns] = useDefaultColumns();
    p[s.columns          ] = columnNames();
    return p;
}

bool mbCoreDataView::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::ConstIterator it;
    MBSETTINGS::ConstIterator end = settings.constEnd();

    bool ok;
    it = settings.find(s.name);
    if (it != end)
        setName(it.value().toString());

    it = settings.find(s.period);
    if (it != end)
        setPeriod(it.value().toInt());

    it = settings.find(s.addressNotation);
    if (it != end)
    {
        mb::AddressNotation v = mb::toAddressNotation(it.value(), &ok);
        if (ok)
            setAddressNotation(v);
    }

    it = settings.find(s.useDefaultColumns);
    if (it != end)
    {
        bool v = it.value().toBool();
        setUseDefaultColumns(v);
    }

    it = settings.find(s.columns);
    if (it != end)
    {
        QStringList v = it.value().toStringList();
        setColumnNames(v);
    }

    return true;
}

int mbCoreDataView::itemInsert(mbCoreDataViewItem *item, int index)
{
    if (!hasItem(item))
    {
        item->setDataViewCore(this);
        if ((index >= 0) && (index < m_items.count()))
            m_items.insert(index, item);
        else
        {
            index = m_items.count();
            m_items.append(item);
        }
        Q_EMIT itemAdded(item);
        connect(item, &mbCoreDataViewItem::changed     , this, &mbCoreDataView::changed);
        connect(item, &mbCoreDataViewItem::valueChanged, this, &mbCoreDataView::changed);
        return index;
    }
    return -1;
}

void mbCoreDataView::itemsInsert(const QList<mbCoreDataViewItem *> &items, int index)
{
    if (index < 0 || index >= itemCount())
    {
        Q_FOREACH (mbCoreDataViewItem *item, items)
            itemAdd(item);
    }
    else
    {
        Q_FOREACH (mbCoreDataViewItem *item, items)
            itemInsert(item, index++);
    }
}

void mbCoreDataView::itemsRemove(const QList<mbCoreDataViewItem *> &items)
{
    Q_FOREACH (mbCoreDataViewItem *item, items)
        itemRemove(item);
}

int mbCoreDataView::itemRemove(int index)
{
    if ((index >= 0) && (index < itemCount()))
    {
        mbCoreDataViewItem *item = itemCoreAt(index);
        item->disconnect(this);
        Q_EMIT itemRemoving(item);
        m_items.removeAt(index);
        Q_EMIT itemRemoved(item);
        item->setDataViewCore(nullptr);
        return index;
    }
    return -1;
}

void mbCoreDataView::changed()
{
    mbCoreDataViewItem *item = static_cast<mbCoreDataViewItem*>(sender());
    Q_EMIT itemChanged(item);
}

