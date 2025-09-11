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

// Note: tagName values are supposed to be lowercase

class MB_EXPORT mbCoreXmlStreamReader : public QXmlStreamReader
{
public:
    using QXmlStreamReader::QXmlStreamReader;
    inline bool hasWarning() const { return m_warnings.count(); }
    inline int warningCount() const { return m_warnings.count(); }
    inline QStringList warnings() const { return m_warnings; }
    void raiseWarning(const QString &text);
    void processUnexpectedElement(const QString &name);

protected:
    QStringList m_warnings;
};

class MB_EXPORT mbCoreXmlStreamWriter : public QXmlStreamWriter
{
public:
    using QXmlStreamWriter::QXmlStreamWriter;
};

class MB_EXPORT mbCoreDom
{
public:
    virtual ~mbCoreDom();

public:
    virtual QString tagName() const = 0;
    virtual void read(mbCoreXmlStreamReader &reader) = 0;
    virtual void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const = 0;
    inline QString text() const { return m_text; }
    inline void setText(const QString &s) { m_text = s; }

protected:
    QString m_text;
    QStringList m_warnings;
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
    void read(mbCoreXmlStreamReader &reader) override
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
            case mbCoreXmlStreamReader::StartElement :
            {
                const QString tag = reader.name().toString().toLower();
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
            case mbCoreXmlStreamReader::EndElement :
                finished = true;
                break;
            case mbCoreXmlStreamReader::Characters :
                if (!reader.isWhitespace())
                    m_text.append(reader.text().toString());
                break;
            default :
                break;
            }
        }
    }
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override
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
// ------------------------------------------------------ DATA VIEW ------------------------------------------------------
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
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;
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
        const QString addressNotation;
        const QString useDefaultColumns;
        const QString columns;
        const QChar   sepColumns;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomDataView();
    ~mbCoreDomDataView();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    inline QString name() const { return m_attr_name; }
    inline void setName(const QString & v) { m_attr_name = v; }

    inline int period() const { return m_attr_period; }
    inline void setPeriod(int v) { m_attr_period = v; }

    inline QString addressNotation() const { return m_addressNotation; }
    inline void setAddressNotation(const QString & v) { m_addressNotation = v; }

    inline bool useDefaultColumns() const { return m_useDefaultColumns; }
    inline void setUseDefaultColumns(bool v) { m_useDefaultColumns = v; }

    inline QStringList columns() const { return m_columns; }
    inline void setColumns(const QStringList &columns) { m_columns = columns; }

    inline QList<mbCoreDomDataViewItem*> items() const { return m_items; }
    inline void setItems(const  QList<mbCoreDomDataViewItem*> &items) { m_items = items; }

    virtual mbCoreDomDataViewItem *newItem() const { return new mbCoreDomDataViewItem; }

private:
    QString m_attr_name;
    int m_attr_period;
    QString m_addressNotation;
    bool m_useDefaultColumns;
    QStringList m_columns;

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
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // elements
    QString name() const { return m_name; }
    inline MBSETTINGS settings() const { return m_settings; }
    void setSettings(const MBSETTINGS &settings);

protected:
    virtual bool readAttribute(mbCoreXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(mbCoreXmlStreamWriter &writer) const;
    virtual bool readElement(mbCoreXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(mbCoreXmlStreamWriter &writer) const;

protected:
    // elements
    QString m_name;
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
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // settings
    QString name() const { return m_name; }
    inline MBSETTINGS settings() const { return m_settings; }
    void setSettings(const MBSETTINGS &settings);

protected:
    virtual bool readAttribute(mbCoreXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(mbCoreXmlStreamWriter &writer) const;
    virtual bool readElement(mbCoreXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(mbCoreXmlStreamWriter &writer) const;

protected:
    // settings
    QString m_name;
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
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

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
        const QString version;
        const QString editnum;
        const QString name;
        const QString author;
        const QString comment;
        const QString windows;

        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDomProject(mbCoreDomPorts      *ports,
                     mbCoreDomDevices    *devices,
                     mbCoreDomDataViews *dataViews);

    virtual ~mbCoreDomProject();

    QString tagName() const override { return Strings::instance().tagName; }
    void read(mbCoreXmlStreamReader &reader) override;
    void write(mbCoreXmlStreamWriter &writer, const QString &tagName = QString()) const override;

    // attributes
    inline quint32 version() const { return m_version.full; }
    inline quint16 versionMajor() const { return m_version.major; }
    inline quint8  versionMinor() const { return m_version.minor; }
    inline quint8  versionPatch() const { return m_version.patch; }
    QString versionStr() const;
    inline void setVersion(quint32 version) { m_version.full = version; }
    inline void setVersion(quint16 major, quint8 minor, quint8 patch) { m_version.major = major; m_version.minor = minor; m_version.patch = patch; }
    void setVersionStr(const QString &versionStr);
    inline int editNumber() const { return m_editnum; }
    inline void setEditNumber(int v) { m_editnum = v; }

    // elements
    inline QString name() const { return m_name; }
    inline void setName(const QString& name) { m_name = name; }

    inline QString author() const { return m_author; }
    inline void setAuthor(const QString& e) { m_author = e; }

    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& e) { m_comment = e; }

    inline QByteArray windowsData() const { return m_windowsData; }
    inline void setWindowsData(const QByteArray& v) { m_windowsData = v; }

    inline QList<mbCoreDomPort*> ports() const { return m_ports->items(); }
    inline void setPorts(const QList<mbCoreDomPort*> &ls) { m_ports->setItems(ls); }

    inline QList<mbCoreDomDevice*> devices() const { return m_devices->items(); }
    inline void setDevices(const QList<mbCoreDomDevice*> &ls) { m_devices->setItems(ls); }

    inline QList<mbCoreDomDataView*> dataViews() const { return m_dataViews->items(); }
    inline void setDataViews(const QList<mbCoreDomDataView*> &ls) { m_dataViews->setItems(ls); }

    inline QList<mbCoreDomTaskInfo*> tasks() const { return m_tasks->items(); }
    inline void setTasks(const QList<mbCoreDomTaskInfo*> &tasks) { m_tasks->setItems(tasks); }

protected:
    virtual bool readAttribute(mbCoreXmlStreamReader &reader, const QXmlStreamAttribute &attribute);
    virtual void writeAttributes(mbCoreXmlStreamWriter &writer) const;
    virtual bool readElement(mbCoreXmlStreamReader &reader, const QString &tag);
    virtual void writeElements(mbCoreXmlStreamWriter &writer) const;

protected:
    // attributes
    union
    {
        quint32 full;
        struct
        {
            quint8  patch;
            quint8  minor;
            quint16 major;
        };
    } m_version;
    int m_editnum;

    // elements
    QString m_name;
    QString m_author;
    QString m_comment;
    QByteArray m_windowsData;
    mbCoreDomPorts      *m_ports;
    mbCoreDomDevices    *m_devices;
    mbCoreDomDataViews  *m_dataViews;
    mbCoreDomTasks      *m_tasks;

    mbCoreDomProject(const mbCoreDomProject& other);
    void operator = (const mbCoreDomProject& other);
};

#endif // CORE_DOM_H
