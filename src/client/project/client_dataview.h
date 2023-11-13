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
#ifndef CLIENT_DATAVIEW_H
#define CLIENT_DATAVIEW_H

#include <QReadWriteLock>

#include <client_global.h>

#include <project/core_dataview.h>

class mbClientProject;
class mbClientDevice;

class mbClientDataViewItem : public mbCoreDataViewItem
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDataViewItem::Strings
    {
        const QString period;

        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbCoreDataViewItem::Defaults
    {
        const uint32_t period;

        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbClientDataViewItem(QObject *parent = nullptr);

public:
    inline mb::Client::ItemHandle_t handle() const { return const_cast<mb::Client::ItemHandle_t>(this); }
    inline bool isReadOnly() const { return m_address.type == Modbus::Memory_1x || m_address.type == Modbus::Memory_3x; }

public: // settings
    inline mbClientDevice *device() const { return reinterpret_cast<mbClientDevice*>(deviceCore()); }
    inline void setDevice(mbClientDevice *device) { setDeviceCore(reinterpret_cast<mbCoreDevice*>(device)); }

    inline int period() const { return m_period; }
    inline void setPeriod(int period) { m_period = period; }

    void setFormat(mb::Format format) override;
    bool isFormatEqualSize(mb::Format format) const;

    MBSETTINGS settings() const override;
    bool setSettings(const MBSETTINGS &settings) override;

public:
    QVariant value() const override;
    void setValue(const QVariant& value) override;

public:
    inline mb::StatusCode status() const { return m_status; }        // TODO: make thread safe
    inline mb::Timestamp_t timestamp() const { return m_timestamp; } // TODO: make thread safe
    void update(const QByteArray &data, mb::StatusCode status, mb::Timestamp_t timestamp);
    inline void update(mb::StatusCode status, mb::Timestamp_t timestamp) { update(QByteArray(), status, timestamp); }

private:
    uint32_t m_period;
    mb::StatusCode m_status;
    mb::Timestamp_t m_timestamp;
    QByteArray m_value;
    QVariant m_cache;

private:
    mutable QReadWriteLock m_lock;
};

class mbClientDataView : public mbCoreDataView
{
    Q_OBJECT

public:
    explicit mbClientDataView(QObject *parent = nullptr);

public:
    inline mbClientProject* project() const { return reinterpret_cast<mbClientProject*>(mbCoreDataView::projectCore()); }
    inline void setProject(mbClientProject* project) { mbCoreDataView::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // items
    inline bool hasItem(mbClientDataViewItem* item) const { return mbCoreDataView::hasItem(static_cast<mbCoreDataViewItem*>(item)); }
    inline QList<mbClientDataViewItem*> items() const { return QList<mbClientDataViewItem*>(*(reinterpret_cast<const QList<mbClientDataViewItem*>*>(&m_items))); }
    inline int itemIndex(mbClientDataViewItem* item) const { return mbCoreDataView::itemIndex(reinterpret_cast<mbClientDataViewItem*>(item)); }
    inline mbClientDataViewItem *item(int i) const { return reinterpret_cast<mbClientDataViewItem*>(mbCoreDataView::itemCore(i)); }
    inline mbClientDataViewItem *itemAt(int i) const { return reinterpret_cast<mbClientDataViewItem*>(mbCoreDataView::itemCoreAt(i)); }
    inline int itemInsert(mbClientDataViewItem* item, int index = -1) { return mbCoreDataView::itemInsert(reinterpret_cast<mbCoreDataViewItem*>(item), index); }
    inline int itemAdd(mbClientDataViewItem* item) { return mbCoreDataView::itemAdd(reinterpret_cast<mbCoreDataViewItem*>(item)); }
    inline int itemRemove(mbClientDataViewItem* item) { return mbCoreDataView::itemRemove(reinterpret_cast<mbCoreDataViewItem*>(item)); }
};

#endif // CLIENT_DATAVIEW_H
