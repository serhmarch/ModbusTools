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
#ifndef SERVER_DOM_H
#define SERVER_DOM_H

#include <project/core_dom.h>

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- ACTION -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomSimAction : public mbCoreDom
{
public:
    struct Strings
    {
        const QString tagName           ;
        const QString device            ;
        const QString address           ;
        const QString dataType          ;
        const QString period            ;
        const QString comment           ;
        const QString actionType        ;
        const QString extended          ;
        const QString byteOrder         ;
        const QString registerOrder     ;

        Strings();
        static const Strings &instance();
    };

public:
    mbServerDomSimAction();
    ~mbServerDomSimAction();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // elements
    inline QString device() const { return m_device; }
    inline void setDevice(const QString& device) { m_device = device; }

    inline QString address() const { return m_address; }
    inline void setAddress(const QString& address) { m_address = address; }

    inline QString dataType() const { return m_data_type; }
    inline void setDataType(const QString& dataType) { m_data_type = dataType; }

    inline int period() const { return m_period; }
    inline void setPeriod(int period) { m_period = period; }

    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& device) { m_comment = device; }

    inline QString actionType() const { return m_actionType; }
    inline void setActionType(const QString& actionType) { m_actionType = actionType; }

    inline QString extended() const { return m_extended; }
    inline void setExtended(const QString& extended) { m_extended = extended; }

    inline QString byteOrder() const { return m_byte_order; }
    inline void setByteOrder(const QString& v) { m_byte_order = v; }

    inline QString registerOrder() const { return m_register_order; }
    inline void setRegisterOrder(const QString& v) { m_register_order = v; }

private: // elements
    QString m_device;
    QString m_address;
    QString m_data_type;
    int m_period;
    QString m_comment;
    QString m_actionType;
    QString m_extended;
    QString m_byte_order;
    QString m_register_order;
};


class mbServerDomSimActions : public mbCoreDomItems<mbServerDomSimAction>
{
public:
    mbServerDomSimActions() : mbCoreDomItems<mbServerDomSimAction>("simactions", mbServerDomSimAction::Strings::instance().tagName) {}
};

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DATA VIEW ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomDataViewItem : public mbCoreDomDataViewItem
{
public:
    mbServerDomDataViewItem();
};

class mbServerDomDataView: public mbCoreDomDataView
{
public:
    mbServerDomDataView();
    mbCoreDomDataViewItem *newItem() const override { return new mbServerDomDataViewItem; }
};

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomDeviceData : public mbCoreDom
{
public:
    struct Strings
    {
        const QString tagName;
        const QString offset ;
        const QString count  ;

        Strings();
        static const Strings &instance();
    };

public:
    mbServerDomDeviceData();
    ~mbServerDomDeviceData();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // attributes
    inline int count() const { return m_attr_count; }
    inline void setCount(int count) { m_attr_count = count; }
    inline bool hasCount() const { return m_has_attr_count; }
    inline void clearCount() { m_has_attr_count = false; }


    inline quint16 offset() const { return m_attr_offset; }
    inline void setOffset(quint16 offset) { m_attr_offset = offset; m_has_attr_offset = true; }
    inline bool hasOffset() const { return m_has_attr_offset; }
    inline void clearOffset() { m_has_attr_offset = false; }

    // elements
    inline QString data() const { return m_text; }
    inline void setData(const QString& e) { m_text = e; m_has_data = true; }
    inline bool hasData() const { return m_has_data; }
    inline void clearData() { m_has_data = false; }

private:
    // attributes
    int m_attr_count;
    bool m_has_attr_count;

    quint16 m_attr_offset;
    bool m_has_attr_offset;

    // child element data
    bool m_has_data;

    mbServerDomDeviceData(const mbServerDomDeviceData& other);
    void operator = (const mbServerDomDeviceData& other);
};

class mbServerDomDevice : public mbCoreDomDevice
{
public:
    struct Strings : public mbCoreDomDevice::Strings
    {
        const QString data0x;
        const QString data1x;
        const QString data3x;
        const QString data4x;

        Strings();
        static const Strings &instance();
    };

public:
    mbServerDomDevice();

    inline const mbServerDomDeviceData &data0x() const { return m_data0x; }
    inline mbServerDomDeviceData &data0x() { return m_data0x; }
    inline QString value0x() const { return m_data0x.data(); }
    inline void setValue0x(const QString &e) { m_data0x.setData(e); }

    inline const mbServerDomDeviceData &data1x() const { return m_data1x; }
    inline mbServerDomDeviceData &data1x() { return m_data1x; }
    inline QString value1x() const { return m_data1x.data(); }
    inline void setValue1x(const QString &e) { m_data1x.setData(e); }

    inline const mbServerDomDeviceData &data3x() const { return m_data3x; }
    inline mbServerDomDeviceData &data3x() { return m_data3x; }
    inline QString value3x() const { return m_data3x.data(); }
    inline void setValue3x(const QString &e) { m_data3x.setData(e); }

    inline const mbServerDomDeviceData &data4x() const { return m_data4x; }
    inline mbServerDomDeviceData &data4x() { return m_data4x; }
    inline QString value4x() const { return m_data4x.data(); }
    inline void setValue4x(const QString &e) { m_data4x.setData(e); }

protected:
    bool readElement(mbCoreXmlStreamReader &reader, const QString &tag) override;
    void writeElements(mbCoreXmlStreamWriter &writer) const override;

private:
    mbServerDomDeviceData m_data0x;
    mbServerDomDeviceData m_data1x;
    mbServerDomDeviceData m_data3x;
    mbServerDomDeviceData m_data4x;

private:
    mbServerDomDevice(const mbServerDomDevice &other);
    void operator =  (const mbServerDomDevice &other);
};

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DEVICEREF ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomDeviceRef : public mbCoreDom
{
public:
    struct Strings
    {
        const QString tagName;
        const QString name   ;

        Strings();
        static const Strings &instance();
    };

public:
    mbServerDomDeviceRef();
    ~mbServerDomDeviceRef();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // attributes
    inline QString name() const { return m_attr_name; }
    inline void setName(const QString &s) { m_attr_name = s; }

    // elements
    inline QString units() const { return m_text; }
    inline void setUnits(const QString &s) { m_text = s; }

private:
    QString m_attr_name;
};

class mbServerDomDeviceRefs : public mbCoreDomItems<mbServerDomDeviceRef>
{
public:
    mbServerDomDeviceRefs() : mbCoreDomItems<mbServerDomDeviceRef>("devices", mbServerDomDeviceRef::Strings::instance().tagName) {}
};

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomPort : public mbCoreDomPort
{
public:
    struct Strings : mbCoreDomPort::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    mbServerDomPort();
    virtual ~mbServerDomPort();

    // devices
    inline QList<mbServerDomDeviceRef*> devices() const { return m_devices->items(); }
    inline void setDevices(const QList<mbServerDomDeviceRef*> &ls) { m_devices->setItems(ls); }

protected:
    bool readElement(mbCoreXmlStreamReader &reader, const QString &tag) override;
    void writeElements(mbCoreXmlStreamWriter &writer) const override;

protected:
    mbServerDomDeviceRefs *m_devices;

private:
    mbServerDomPort(const mbServerDomPort& other);
    void operator = (const mbServerDomPort& other);
};


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbServerDomPorts : public mbCoreDomPorts
{
public:
    mbServerDomPorts() : mbCoreDomPorts() {}
    mbCoreDomPort *newItem() const override { return new mbServerDomPort; }
};

class mbServerDomDevices : public mbCoreDomDevices
{
public:
    mbServerDomDevices() : mbCoreDomDevices() {}
    mbCoreDomDevice *newItem() const override { return new mbServerDomDevice; }
};

class mbServerDomDataViews : public mbCoreDomDataViews
{
public:
    mbServerDomDataViews() : mbCoreDomDataViews() {}
    mbCoreDomDataView *newItem() const override { return new mbServerDomDataView; }
};

class mbServerDomProject : public mbCoreDomProject
{
public:
    mbServerDomProject();
    ~mbServerDomProject();

public:
    inline QList<mbServerDomSimAction*> simActions() const { return m_simActions->items(); }
    inline void setSimActions(const QList<mbServerDomSimAction*> &ls) { m_simActions->setItems(ls); }

protected:
    bool readElement(mbCoreXmlStreamReader &reader, const QString &tag) override;
    void writeElements(mbCoreXmlStreamWriter &writer) const override;

private:
    mbServerDomSimActions *m_simActions;

private:
    mbServerDomProject(const mbServerDomProject& other);
    void operator =   (const mbServerDomProject& other);
};

#endif // SERVER_DOM_H
