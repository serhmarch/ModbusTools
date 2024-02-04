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
#ifndef CORE_DOM_H
#define CORE_DOM_H

#include <QtAlgorithms>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <Modbus.h>
#include <mbcore.h>

class MB_EXPORT mbCoreDom
{
public:
    virtual ~mbCoreDom();

public:
    virtual QString tagName() const = 0;
    virtual void read(QXmlStreamReader &reader) = 0;
    virtual void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const = 0;
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

protected:
    QString m_text;
};

template <typename T>
class mbCoreDomItems : public mbCoreDom
{
public:
    mbCoreDomItems(const QString &tagItems, const QString &tagItem) : c_tagItems(tagItems), c_tagItem(tagItem){}

    virtual ~mbCoreDomItems()
    {
        qDeleteAll(m_items);
    }

public:
    QString tagName() const override { return c_tagItems; }
    void read(QXmlStreamReader &reader) override
    {
        Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
        {
            QStringRef name = attribute.name();
            reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
        }

        for (bool finished = false; !finished && !reader.hasError();)
        {
            switch (reader.readNext())
            {
            case QXmlStreamReader::StartElement :
            {
                const QString tag = reader.name().toString();
                if (tag == c_tagItem)
                {
                    T *item = newItem();
                    item->read(reader);
                    m_items.append(item);
                    continue;
                }
                reader.raiseError(QStringLiteral("Unexpected element ") + tag);
            }
            break;
            case QXmlStreamReader::EndElement :
                finished = true;
                break;
            case QXmlStreamReader::Characters :
                if (!reader.isWhitespace())
                    m_text.append(reader.text().toString());
                break;
            default :
                break;
            }
        }
    }
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override
    {
        writer.writeStartElement(tagName.isEmpty() ? c_tagItems : tagName);

        Q_FOREACH (T *v, m_items)
            v->write(writer);

        if (!m_text.isEmpty())
            writer.writeCharacters(m_text);

        writer.writeEndElement();
    }

    inline QList<T*> items() const { return m_items; };
    inline void setItems(const QList<T*> &items) { m_items = items; }
    inline int itemCount() const { return m_items.count(); }

    inline const QString &tagItems() const { return c_tagItems; }
    inline const QString &tagItem() const { return c_tagItem; }

    virtual T *newItem() const { return new T; }

private:
    QList<T*> m_items;

private:
    const QString c_tagItems;
    const QString c_tagItem;
};


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ WATCH LIST -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class MB_EXPORT mbCoreDomDataViewItem : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;
        const QString device ;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomDataViewItem();
    ~mbCoreDomDataViewItem();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

    // attributes
    inline QString device() const { return m_device; }
    inline void setDevice(const QString& device) { m_device = device; }

    // elements
    inline MBSETTINGS settings() const { return m_settings; }
    inline void setSettings(const MBSETTINGS& settings) { m_settings = settings; }

private:
    // attributes
    QString m_device;
    MBSETTINGS m_settings;
};

class MB_EXPORT mbCoreDomDataView : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;
        const QString name;
        const QString period;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomDataView();
    ~mbCoreDomDataView();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    inline QString name() const { return m_attr_name; }
    inline void setName(const QString & v) { m_attr_name = v; }

    inline int period() const { return m_attr_period; }
    inline void setPeriod(int v) { m_attr_period = v; }

    inline QList<mbCoreDomDataViewItem*> items() const { return m_items; }
    inline void setItems(const  QList<mbCoreDomDataViewItem*> &items) { m_items = items; }

    virtual mbCoreDomDataViewItem *newItem() const { return new mbCoreDomDataViewItem; }

private:
    QString m_attr_name;
    int m_attr_period;

    QList<mbCoreDomDataViewItem*> m_items;

};


// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class MB_EXPORT mbCoreDomDevice : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomDevice();
    virtual ~mbCoreDomDevice();
    
    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // elements
    inline MBSETTINGS settings() const { return m_settings; }
    inline void setSettings(const MBSETTINGS &settings) { m_settings = settings; }

protected:
    virtual bool readAttribute(QXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(QXmlStreamWriter &writer) const;
    virtual bool readElement(QXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(QXmlStreamWriter &writer) const;

protected:
    // elements
    MBSETTINGS m_settings;

private:
    mbCoreDomDevice(const mbCoreDomDevice& other);
    void operator = (const mbCoreDomDevice& other);
};


// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class MB_EXPORT mbCoreDomPort : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomPort();
    virtual ~mbCoreDomPort();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // settings
    inline MBSETTINGS settings() const { return m_settings; }
    inline void setSettings(const MBSETTINGS& settings) { m_settings = settings; }

protected:
    virtual bool readAttribute(QXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(QXmlStreamWriter &writer) const;
    virtual bool readElement(QXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(QXmlStreamWriter &writer) const;

protected:
    // settings
    MBSETTINGS m_settings;

private:
    mbCoreDomPort(const mbCoreDomPort& other);
    void operator = (const mbCoreDomPort& other);
};


// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- TASK --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class MB_EXPORT mbCoreDomTaskInfo : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;
        const QString name;
        const QString type;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomTaskInfo();
    virtual ~mbCoreDomTaskInfo();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // attributes
    inline QString name() const { return m_attr_name; }
    inline void setName(const QString& name) { m_attr_name = name; }

    inline QString type() const { return m_attr_type; }
    inline void setType(const QString& type) { m_attr_type = type; }

    inline MBSETTINGS settings() const { return m_settings; }
    inline void setSettings(const MBSETTINGS &settings) { m_settings = settings; }

protected:
    // attributes
    QString m_attr_name;
    QString m_attr_type;

    MBSETTINGS m_settings;

    mbCoreDomTaskInfo(const mbCoreDomTaskInfo &other);
    void operator = (const mbCoreDomTaskInfo &other);
};


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbCoreDomPorts : public mbCoreDomItems<mbCoreDomPort>
{
public:
    mbCoreDomPorts() : mbCoreDomItems<mbCoreDomPort>("ports", mbCoreDomPort::Strings::instance().tagName) {}
};

class mbCoreDomDevices : public mbCoreDomItems<mbCoreDomDevice>
{
public:
    mbCoreDomDevices() : mbCoreDomItems<mbCoreDomDevice>("devices", mbCoreDomDevice::Strings::instance().tagName) {}
};

class mbCoreDomDataViews : public mbCoreDomItems<mbCoreDomDataView>
{
public:
    mbCoreDomDataViews() : mbCoreDomItems<mbCoreDomDataView>("dataviews", mbCoreDomDataView::Strings::instance().tagName) {}
};

class mbCoreDomTasks : public mbCoreDomItems<mbCoreDomTaskInfo>
{
public:
    mbCoreDomTasks() : mbCoreDomItems<mbCoreDomTaskInfo>(QStringLiteral("tasks"), QStringLiteral("task")) {}
};

class MB_EXPORT mbCoreDomProject : public mbCoreDom
{
public:
    struct MB_EXPORT Strings
    {
        const QString tagName;
        const QString name;
        const QString author;
        const QString comment;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomProject(mbCoreDomPorts      *ports,
                     mbCoreDomDevices    *devices,
                     mbCoreDomDataViews *dataViews);

    virtual ~mbCoreDomProject();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(QXmlStreamReader &reader) override;
    void write(QXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // elements
    inline QString name() const { return m_name; }
    inline void setName(const QString& name) { m_name = name; }

    inline QString author() const { return m_author; }
    inline void setAuthor(const QString& e) { m_author = e; }

    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& e) { m_comment = e; }

    inline QList<mbCoreDomPort*> ports() const { return m_ports->items(); }
    inline void setPorts(const QList<mbCoreDomPort*> &ls) { m_ports->setItems(ls); }

    inline QList<mbCoreDomDevice*> devices() const { return m_devices->items(); }
    inline void setDevices(const QList<mbCoreDomDevice*> &ls) { m_devices->setItems(ls); }

    inline QList<mbCoreDomDataView*> dataViews() const { return m_dataViews->items(); }
    inline void setDataViews(const QList<mbCoreDomDataView*> &ls) { m_dataViews->setItems(ls); }

    inline QList<mbCoreDomTaskInfo*> tasks() const { return m_tasks->items(); }
    inline void setTasks(const QList<mbCoreDomTaskInfo*> &tasks) { m_tasks->setItems(tasks); }

protected:
    virtual bool readAttribute(QXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(QXmlStreamWriter &writer) const;
    virtual bool readElement(QXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(QXmlStreamWriter &writer) const;

protected:
    // attributes
    QString m_name;
    QString m_author;
    QString m_comment;
    // elements
    mbCoreDomPorts      *m_ports;
    mbCoreDomDevices    *m_devices;
    mbCoreDomDataViews  *m_dataViews;
    mbCoreDomTasks      *m_tasks;

    mbCoreDomProject(const mbCoreDomProject& other);
    void operator = (const mbCoreDomProject& other);
};

#endif // CORE_DOM_H
