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

#include "core_project.h"

static inline mb::DataOrder getRegisterOrder(mb::DataOrder order, mbCoreDevice *device)
{
    if (order == mb::DefaultOrder)
    {
        if (device && (device->registerOrder() != mb::DefaultOrder))
            return device->registerOrder();
        return mb::LessSignifiedFirst;
    }
    return order;
}

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
    stringEncoding    (QStringLiteral("stringEncoding"))
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
    stringEncoding              (mb::Utf8)
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
    QMetaEnum me = QMetaEnum::fromType<mb::Format>();
    return QString(me.valueToKey(m_format));
}

void mbCoreDataViewItem::setFormatStr(const QString &formatStr)
{
    QMetaEnum me = QMetaEnum::fromType<mb::Format>();
    bool ok;
    int k = me.keyToValue(formatStr.toLatin1(), &ok);
    if (ok)
        setFormat(static_cast<mb::Format>(k));
}

QString mbCoreDataViewItem::byteOrderStr() const
{
    return mb::enumKeyTypeStr<mb::DataOrder>(m_byteOrder);
}

void mbCoreDataViewItem::setByteOrderStr(const QString &order)
{
    bool ok;
    mb::DataOrder k = mb::enumValueTypeStr<mb::DataOrder>(order, &ok);
    if (ok)
        m_byteOrder = k;
}

QString mbCoreDataViewItem::registerOrderStr() const
{
    return mb::enumKeyTypeStr<mb::DataOrder>(m_registerOrder);
}

void mbCoreDataViewItem::setRegisterOrderStr(const QString &registerOrderStr)
{
    bool ok;
    mb::DataOrder k = mb::enumValueTypeStr<mb::DataOrder>(registerOrderStr, &ok);
    if (ok)
        m_registerOrder = k;
}

QString mbCoreDataViewItem::byteArrayFormatStr() const
{
    return mb::enumKeyTypeStr<mb::DigitalFormat>(m_byteArrayFormat);
}

void mbCoreDataViewItem::setByteArrayFormatStr(const QString &byteArrayFormatStr)
{
    bool ok;
    mb::DigitalFormat k = mb::enumValueTypeStr<mb::DigitalFormat>(byteArrayFormatStr, &ok);
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
    return mb::enumKeyTypeStr<mb::StringLengthType>(m_stringLengthType);
}

void mbCoreDataViewItem::setStringLengthTypeStr(const QString &stringLengthTypeStr)
{
    bool ok;
    mb::StringLengthType k = mb::enumValueTypeStr<mb::StringLengthType>(stringLengthTypeStr, &ok);
    if (ok)
        m_stringLengthType = k;
}

QString mbCoreDataViewItem::stringEncodingStr() const
{
    return mb::enumKeyTypeStr<mb::StringEncoding>(m_stringEncoding);
}

void mbCoreDataViewItem::setStringEncodingStr(const QString &stringEncodingStr)
{
    bool ok;
    mb::StringEncoding k = mb::enumValueTypeStr<mb::StringEncoding>(stringEncodingStr, &ok);
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
    p[s.stringEncoding    ] = mb::enumKey(stringEncoding());
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
        mb::DataOrder v = mb::enumValue<mb::DataOrder>(it.value(), &ok);
        if (ok)
            setByteOrder(v);
    }

    it = settings.find(s.registerOrder);
    if (it != end)
    {
        mb::DataOrder v = mb::enumValue<mb::DataOrder>(it.value(), &ok);
        if (ok)
            setRegisterOrder(v);
    }

    it = settings.find(s.byteArrayFormat);
    if (it != end)
    {
        mb::DigitalFormat v = mb::enumValue<mb::DigitalFormat>(it.value(), &ok);
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
        mb::StringLengthType v = mb::enumValue<mb::StringLengthType>(it.value(), &ok);
        if (ok)
            setStringLengthType(v);
    }

    it = settings.find(s.stringEncoding);
    if (it != end)
    {
        mb::StringEncoding v = mb::enumValue<mb::StringEncoding>(it.value(), &ok);
        if (ok)
            setStringEncoding(v);
    }

    it = settings.find(s.format);
    if (it != end)
    {
        mb::Format v = mb::enumValue<mb::Format>(it.value(), &ok);
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
                           getRegisterOrder(m_registerOrder, m_device),
                           m_byteArrayFormat,
                           m_stringEncoding,
                           m_stringLengthType,
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
                         getRegisterOrder(m_registerOrder, m_device),
                         m_byteArrayFormat,
                         m_stringEncoding,
                         m_stringLengthType,
                         byteArraySeparator(),
                         m_variableLength);
}

mbCoreDataView::Strings::Strings() :
    name  (QStringLiteral("name")),
    period(QStringLiteral("period"))
{
}

const mbCoreDataView::Strings &mbCoreDataView::Strings::instance()
{
    static const Strings d;
    return d;
}

mbCoreDataView::Defaults::Defaults() :
    name(QStringLiteral("dataView")),
    period(1000)
{
}

const mbCoreDataView::Defaults &mbCoreDataView::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCoreDataView::mbCoreDataView(QObject *parent) : QObject(parent)
{
    m_project = nullptr;
    m_period = Defaults::instance().period;
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

MBSETTINGS mbCoreDataView::settings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS p;
    p[s.name  ] = name();
    p[s.period] = period();
    return p;
}

bool mbCoreDataView::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::ConstIterator it;
    MBSETTINGS::ConstIterator end = settings.constEnd();

    it = settings.find(s.name);
    if (it != end)
        setName(it.value().toString());

    it = settings.find(s.period);
    if (it != end)
        setPeriod(it.value().toInt());
    return true;
}

int mbCoreDataView::itemInsert(mbCoreDataViewItem *item, int index)
{
    if (!hasItem(item))
    {
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
        return index;
    }
    return -1;
}

void mbCoreDataView::changed()
{
    mbCoreDataViewItem *item = static_cast<mbCoreDataViewItem*>(sender());
    Q_EMIT itemChanged(item);
}

