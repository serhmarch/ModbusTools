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
#include "client_dataview.h"

#include "client.h"

mbClientDataViewItem::Strings::Strings() :
    mbCoreDataViewItem::Strings(),
    period(QStringLiteral("period"))
{
}

const mbClientDataViewItem::Strings &mbClientDataViewItem::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDataViewItem::Defaults::Defaults() :
    mbCoreDataViewItem::Defaults(),
    period(1000)
{
}

const mbClientDataViewItem::Defaults &mbClientDataViewItem::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbClientDataViewItem::mbClientDataViewItem(QObject *parent) :
    mbCoreDataViewItem(parent)
{
    m_period = Defaults::instance().period;
    m_status = mb::Status_MbStopped;
    m_timestamp = mb::currentTimestamp();
    m_cache = toVariant(m_value);
}

void mbClientDataViewItem::setFormat(mb::Format format)
{
    QWriteLocker _(&m_lock);
    if (!mbClient::global()->isRunning() || isFormatEqualSize((format)))
    {
        this->blockSignals(true);
        mbCoreDataViewItem::setFormat(format);
        m_cache = toVariant(m_value);
        this->blockSignals(false);
        Q_EMIT changed();
    }
}

bool mbClientDataViewItem::isFormatEqualSize(mb::Format format) const
{
    switch (m_format)
    {
    case mb::Bin16:
    case mb::Oct16:
    case mb::Dec16:
    case mb::UDec16:
    case mb::Hex16:
        switch (format)
        {
        case mb::Bin16:
        case mb::Oct16:
        case mb::Dec16:
        case mb::UDec16:
        case mb::Hex16:
            return true;
        default:
            return false;
        }
    case mb::Bin32:
    case mb::Oct32:
    case mb::Dec32:
    case mb::UDec32:
    case mb::Hex32:
    case mb::Float:
        switch (format)
        {
        case mb::Bin32:
        case mb::Oct32:
        case mb::Dec32:
        case mb::UDec32:
        case mb::Hex32:
        case mb::Float:
            return true;
        default:
            return false;
        }
    case mb::Bin64:
    case mb::Oct64:
    case mb::Dec64:
    case mb::UDec64:
    case mb::Hex64:
    case mb::Double:
        switch (format)
        {
        case mb::Bin64:
        case mb::Oct64:
        case mb::Dec64:
        case mb::UDec64:
        case mb::Hex64:
        case mb::Double:
            return true;
        default:
            return false;
        }
    default:
        return false;
    }
}

MBSETTINGS mbClientDataViewItem::settings() const
{
    Strings s = Strings();

    MBSETTINGS r = mbCoreDataViewItem::settings();
    r.insert(s.period, period());
    return r;
}

bool mbClientDataViewItem::setSettings(const MBSETTINGS &settings)
{
    Strings s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.period);
    if (it != end)
    {
        QVariant var = it.value();
        uint32_t v = static_cast<uint32_t>(var.toUInt(&ok));
        if (ok)
            setPeriod(v);
    }

    mbCoreDataViewItem::setSettings(settings); // Q_EMIT changed() within
    return true;
}

QVariant mbClientDataViewItem::value() const
{
    QReadLocker _(&m_lock);
    return m_cache;
}

void mbClientDataViewItem::setValue(const QVariant &value)
{
    m_lock.lockForRead();
    QByteArray data = toByteArray(value); // Note: m_format may change
    m_lock.unlock();
    if (data.length() > 0)
    {
        if (mbClient::global()->isRunning())
        {
            if (!isReadOnly() && m_device)
                mbClient::global()->writeItemData(handle(), data);
        }
        else
            update(data, mb::Status_MbStopped, mb::currentTimestamp());
    }
}

void mbClientDataViewItem::update(const QByteArray &value, mb::StatusCode status, mb::Timestamp_t timestamp)
{
    QWriteLocker _(&m_lock);
    if (value.count())
    {
        m_value = value;
        QVariant newCacheValue = toVariant(m_value);
        if (m_cache != newCacheValue)
        {
            m_cache = newCacheValue;
            m_status = status;
            m_timestamp = timestamp;
            Q_EMIT valueChanged();
            return;
        }
    }
    if (m_status != status)
    {
        m_status = status;
        m_timestamp = timestamp;
        Q_EMIT valueChanged();
        return;
    }
}

QStringList mbClientDataView::availableColumnNames()
{
    QStringList res = mbCoreDataView::availableColumnNames();
    QMetaEnum me = QMetaEnum::fromType<ClientColumns>();
    for (int i = 0; i < (ColumnCount-mbCoreDataView::ColumnCount); i++)
        res.append(me.key(i));
    return res;
}

mbClientDataView::mbClientDataView(QObject *parent)
    : mbCoreDataView{parent}
{
    for (int i = mbCoreDataView::ColumnCount; i < ColumnCount; i++)
        m_columns.append(i);
}

int mbClientDataView::columnTypeByName(const QString &name) const
{
    int res = mbCoreDataView::columnTypeByName(name);
    if (res < 0)
    {
        QMetaEnum me = QMetaEnum::fromType<ClientColumns>();
        res = me.keyToValue(name.toUtf8().constData());
    }
    return res;
}

QString mbClientDataView::columnNameByIndex(int i) const
{
    int type = m_columns.value(i, -1);
    if (type < mbCoreDataView::ColumnCount)
        return mbCoreDataView::columnNameByIndex(i);
    else
        return QMetaEnum::fromType<ClientColumns>().valueToKey(type);;
}

