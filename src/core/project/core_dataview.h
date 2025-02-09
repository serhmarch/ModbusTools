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
#ifndef CORE_DATAVIEW_H
#define CORE_DATAVIEW_H

#include <QPointer>

#include <mbcore.h>

#include "core_device.h"

class mbCoreProject;
class mbCoreDataView;

class MB_EXPORT mbCoreDataViewItem : public QObject
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString device            ;
        const QString address           ;
        const QString format            ;
        const QString comment           ;
        const QString variableLength    ;
        const QString byteOrder         ;
        const QString registerOrder     ;
        const QString byteArrayFormat   ;
        const QString byteArraySeparator;
        const QString stringLengthType  ;
        const QString stringEncoding    ;
        const QString value             ;

        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const int                   address                     ;
        const mb::Format            format                      ;
        const QString               comment                     ;
        const int                   variableLength              ;
        const mb::DataOrder         byteOrder                   ;
        const mb::RegisterOrder     registerOrder               ;
        const mb::DigitalFormat     byteArrayFormat             ;
        const QString               byteArraySeparator          ;
        const bool                  isDefaultByteArraySeparator ;
        const mb::StringLengthType  stringLengthType            ;
        const mb::StringEncoding    stringEncoding              ;
        const QVariant              value                       ;

        Defaults();
        static const Defaults &instance();
    };

public:
    mbCoreDataViewItem(QObject *parent = nullptr);

public:
    inline mbCoreDevice *deviceCore() const { return m_device; }
    inline void setDeviceCore(mbCoreDevice *device) { m_device = device; }
    inline mbCoreDataView *dataViewCore() const { return m_dataView; }
    inline void setDataViewCore(mbCoreDataView *view) { m_dataView = view; }

public:
    int bitLength() const;
    int byteLength() const;
    inline int registerLength() const { return (byteLength()+1)/2; }
    int length() const;
    inline int count() const { return length(); }
    inline int sizeOf() const { return (bitLength()+7)/8; }

public:
    inline mb::Address address() const { return m_address; }
    inline int addressInt() const { return mb::toInt(m_address); }
    QString addressStr() const;
    void setAddress(const mb::Address& address);
    inline void setAddressInt(int address) { setAddress(mb::toAddress(address)); }
    inline void setAddressStr(const QString& address) { setAddress(mb::toAddress(address)); }
    inline void setAddress(int address) { setAddressInt(address); }
    inline void setAddress(const QString& address) { setAddressStr(address); }
    void setAddress(Modbus::MemoryType type, quint16 offset);
    inline Modbus::MemoryType addressType() const { return m_address.type; }
    inline quint16 addressOffset() const { return m_address.offset; }

    inline mb::Format format() const { return m_format; }
    virtual void setFormat(mb::Format format);
    QString formatStr() const;
    void setFormatStr(const QString& formatStr);

    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& comment) { m_comment = comment; }

    inline int variableLength() const { return m_variableLength; }
    inline void setVariableLength(int len) { m_variableLength = len; }

    inline mb::DataOrder byteOrder() const { return m_byteOrder; }
    inline void setByteOrder(mb::DataOrder order) { m_byteOrder = order; }
    QString byteOrderStr() const;
    void setByteOrderStr(const QString& order);

    inline mb::RegisterOrder registerOrder() const { return m_registerOrder; }
    inline void setRegisterOrder(mb::RegisterOrder registerOrder) { m_registerOrder = registerOrder; }
    QString registerOrderStr() const;
    void setRegisterOrderStr(const QString& registerOrderStr);

    inline mb::DigitalFormat byteArrayFormat() const { return m_byteArrayFormat; }
    inline void setByteArrayFormat(mb::DigitalFormat byteArrayFormat) { m_byteArrayFormat = byteArrayFormat; }
    QString byteArrayFormatStr() const;
    void setByteArrayFormatStr(const QString& byteArrayFormatStr);

    QString byteArraySeparator() const;
    void setByteArraySeparator(const QString &byteArraySeparator);
    QString byteArraySeparatorStr() const;
    void setByteArraySeparatorStr(const QString &byteArraySeparatorStr);

    inline mb::StringLengthType stringLengthType() const { return m_stringLengthType; }
    inline void setStringLengthType(mb::StringLengthType stringLengthType) { m_stringLengthType = stringLengthType; }
    QString stringLengthTypeStr() const;
    void setStringLengthTypeStr(const QString& stringLengthTypeStr);

    inline bool isDefaultStringEncoding() const { return m_isDefaultStringEncoding; }
    inline mb::StringEncoding stringEncoding() const { return m_stringEncoding; }
    inline void setStringEncoding(const mb::StringEncoding &stringEncoding) { m_stringEncoding = stringEncoding; }
    QString stringEncodingStr() const;
    void setStringEncodingStr(const QString& stringEncodingStr);

    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS &settings);

public:
    QByteArray toByteArray(const QVariant &v) const;
    QVariant toVariant(const QByteArray &v) const;

public:
    virtual QVariant value() const = 0;
    virtual void setValue(const QVariant &value) = 0;

Q_SIGNALS:
    void changed();
    void valueChanged();

protected:
    mb::DataOrder getByteOrder() const;
    mb::RegisterOrder getRegisterOrder() const;
    mb::StringEncoding getStringEncoding() const;
    mb::StringLengthType getStringLengthType() const;

protected:
    QPointer<mbCoreDevice> m_device;
    mbCoreDataView *m_dataView;
    mb::Address m_address;
    mb::Format m_format;
    QString m_comment;
    int m_variableLength;
    mb::DataOrder m_byteOrder;
    mb::RegisterOrder m_registerOrder;
    mb::DigitalFormat m_byteArrayFormat;
    QString m_byteArraySeparator;
    bool m_isDefaultByteArraySeparator;
    mb::StringLengthType m_stringLengthType;
    mb::StringEncoding m_stringEncoding;
    bool m_isDefaultStringEncoding;
};

class MB_EXPORT mbCoreDataView : public QObject
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString name;
        const QString period;
        const QString addressNotation;
        const QString useDefaultColumns;
        const QString columns;

        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const QString name;
        const int period;
        const mb::AddressNotation addressNotation;
        const bool useDefaultColumns;
        Defaults();
        static const Defaults &instance();
    };

public:
    enum CoreColumns
    {
        Device,
        Address,
        Format,
        Comment,
        Value,
        ColumnCount
    };
    Q_ENUM(CoreColumns)

    static QStringList availableColumnNames();

public:
    mbCoreDataView(QObject *parent = nullptr);
    ~mbCoreDataView();

public:
    inline mbCoreProject* projectCore() const { return m_project; }
    inline void setProjectCore(mbCoreProject* project) { m_project = project; }

public:
    inline QString name() const { return objectName(); }
    void setName(const QString &name);
    inline int period() const { return m_period; }
    void setPeriod(int period);
    inline mb::AddressNotation addressNotation() const { return m_addressNotation; }
    mb::AddressNotation getAddressNotation() const;
    void setAddressNotation(mb::AddressNotation notation);
    inline bool useDefaultColumns() const { return m_useDefaultColumns; }
    void setUseDefaultColumns(bool use);
    inline int columnCount() const { return m_columns.count(); }
    int getColumnCount() const;
    inline QList<int> columns() const { return m_columns; }
    QList<int> getColumns() const;
    void setColumns(const QList<int> columns);
    QStringList columnNames() const;
    void setColumnNames(const QStringList &columns);
    int columnTypeByIndex(int i) const;
    int getColumnTypeByIndex(int i) const;
    virtual int columnTypeByName(const QString &name) const;
    int getColumnTypeByName(const QString &name) const;
    virtual QString columnNameByIndex(int i) const;
    QString getColumnNameByIndex(int i) const;
    int columnIndexByType(int type);
    int getColumnIndexByType(int type);

    virtual MBSETTINGS settings() const;
    virtual bool setSettings(const MBSETTINGS& settings);

public: // item
    inline bool hasItem(mbCoreDataViewItem* item) const { return m_items.contains(item); }
    inline QList<mbCoreDataViewItem*> itemsCore() const { return m_items; }
    inline int itemIndex(mbCoreDataViewItem* item) const { return m_items.indexOf(item); }
    inline mbCoreDataViewItem* itemCore(int i) const { return m_items.value(i); }
    inline mbCoreDataViewItem* itemCoreAt(int i) const { return m_items.at(i); }
    inline int itemCount() const { return m_items.count(); }
    int itemInsert(mbCoreDataViewItem* item, int index = -1);
    inline int itemAdd(mbCoreDataViewItem* item) { return itemInsert(item); }
    void itemsInsert(const QList<mbCoreDataViewItem*> &items, int index = -1);
    void itemsRemove(const QList<mbCoreDataViewItem*> &items);
    int itemRemove(int index);
    inline int itemRemove(mbCoreDataViewItem* item) { return itemRemove(itemIndex(item)); }

Q_SIGNALS:
    void nameChanged(const QString &name);
    void itemAdded(mbCoreDataViewItem* item);
    void itemRemoving(mbCoreDataViewItem* item);
    void itemRemoved(mbCoreDataViewItem* item);
    void itemChanged(mbCoreDataViewItem* item);
    void periodChanged(int period);
    void addressNotationChanged(mb::AddressNotation addressNotation);
    void columnsChanged();

protected Q_SLOTS:
    void changed();

protected:
    mbCoreProject* m_project;
    QList<mbCoreDataViewItem*> m_items;

protected:
    int m_period;
    mb::AddressNotation m_addressNotation;
    bool m_useDefaultColumns;
    QList<int> m_columns;
};

#endif // CORE_DATAVIEW_H
