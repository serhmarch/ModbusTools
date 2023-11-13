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
#include "core_dom.h"

#include "core_dataview.h"

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------- WATCH LIST ITEM ---------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomDataViewItem::Strings::Strings() :
    tagName(QStringLiteral("item")),
    device (QStringLiteral("device"))
{
}

const mbCoreDomDataViewItem::Strings &mbCoreDomDataViewItem::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomDataViewItem::mbCoreDomDataViewItem()
{
}

mbCoreDomDataViewItem::~mbCoreDomDataViewItem()
{
}

void mbCoreDomDataViewItem::read(QXmlStreamReader &reader)
{
    const Strings &s = Strings::instance();

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
            if (tag == s.device)
            {
                setDevice(reader.readElementText());
                continue;
            }
            m_settings.insert(tag, reader.readElementText());
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

void mbCoreDomDataViewItem::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);

    writer.writeTextElement(s.device, device());

    QList<QString> tags = m_settings.keys();
    std::sort(tags.begin(), tags.end());
    Q_FOREACH (const QString &tag, tags)
        writer.writeTextElement(tag, m_settings.value(tag).toString());

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();

}


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ WATCH LIST -----------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomDataView::Strings::Strings() :
    tagName(QStringLiteral("dataview")),
    name   (QStringLiteral("name")),
    period (QStringLiteral("period"))
{
}

const mbCoreDomDataView::Strings &mbCoreDomDataView::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomDataView::mbCoreDomDataView()
{
    m_attr_period = mbCoreDataView::Defaults::instance().period;
}

mbCoreDomDataView::~mbCoreDomDataView()
{
    qDeleteAll(m_items);
}

void mbCoreDomDataView::read(QXmlStreamReader &reader)
{
    const Strings &s = Strings::instance();
    const mbCoreDomDataViewItem::Strings &sItem = mbCoreDomDataViewItem::Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (name == s.name)
        {
            setName(attribute.value().toString());
            continue;
        }
        if (name == s.period)
        {
            setPeriod(attribute.value().toInt());
            continue;
        }
        reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            if (tag == sItem.tagName)
            {
                mbCoreDomDataViewItem *item = newItem();
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

void mbCoreDomDataView::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writer.writeAttribute(s.name  , name());
    writer.writeAttribute(s.period, QString::number(period()));

    Q_FOREACH (mbCoreDomDataViewItem *v, m_items)
        v->write(writer);

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}


// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomDevice::Strings::Strings() :
    tagName(QStringLiteral("device"))
{

}

const mbCoreDomDevice::Strings &mbCoreDomDevice::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomDevice::mbCoreDomDevice()
{
}

mbCoreDomDevice::~mbCoreDomDevice()
{
}

void mbCoreDomDevice::read(QXmlStreamReader &reader)
{
    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (readAttribute(reader, attribute))
            continue;
        reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            if (readElement(reader, tag))
                continue;
            m_settings.insert(tag, reader.readElementText());
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

void mbCoreDomDevice::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writeAttributes(writer);

    for (Modbus::Settings::const_iterator i = m_settings.constBegin(); i != m_settings.constEnd(); i++)
        writer.writeTextElement(i.key(), i.value().toString());
    writeElements(writer);
    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}

bool mbCoreDomDevice::readAttribute(QXmlStreamReader &/*reader*/, const QXmlStreamAttribute &/*attribute*/)
{
    return false;
}

void mbCoreDomDevice::writeAttributes(QXmlStreamWriter &/*writer*/) const
{
}

bool mbCoreDomDevice::readElement(QXmlStreamReader &/*reader*/, const QString &/*tag*/)
{
    return false;
}

void mbCoreDomDevice::writeElements(QXmlStreamWriter &/*writer*/) const
{
}


// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomPort::Strings::Strings() :
    tagName(QStringLiteral("port"))
{

}

const mbCoreDomPort::Strings &mbCoreDomPort::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomPort::mbCoreDomPort()
{
}

mbCoreDomPort::~mbCoreDomPort()
{
}

void mbCoreDomPort::read(QXmlStreamReader &reader)
{
    mbCoreDomPort::Strings s = mbCoreDomPort::Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (readAttribute(reader, attribute))
            continue;
        reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            if (readElement(reader, tag))
                continue;
            m_settings.insert(tag, reader.readElementText());
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

void mbCoreDomPort::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writeAttributes(writer);

    for (Modbus::Settings::const_iterator i = m_settings.constBegin(); i != m_settings.constEnd(); i++)
        writer.writeTextElement(i.key(), i.value().toString());
    writeElements(writer);
    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}

bool mbCoreDomPort::readAttribute(QXmlStreamReader &/*reader*/, const QXmlStreamAttribute &/*attribute*/)
{
    return false;
}

void mbCoreDomPort::writeAttributes(QXmlStreamWriter &/*writer*/) const
{
}

bool mbCoreDomPort::readElement(QXmlStreamReader &/*reader*/, const QString &/*tag*/)
{
    return false;
}

void mbCoreDomPort::writeElements(QXmlStreamWriter &/*writer*/) const
{
}


// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- TASK --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomTaskInfo::Strings::Strings() :
    tagName(QStringLiteral("task")),
    name(QStringLiteral("name")),
    type(QStringLiteral("type"))
{

}

const mbCoreDomTaskInfo::Strings &mbCoreDomTaskInfo::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomTaskInfo::mbCoreDomTaskInfo()
{
}

mbCoreDomTaskInfo::~mbCoreDomTaskInfo()
{
}

void mbCoreDomTaskInfo::read(QXmlStreamReader &reader)
{
    mbCoreDomTaskInfo::Strings s = mbCoreDomTaskInfo::Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (name == s.name)
        {
            setName(attribute.value().toString());
            continue;
        }
        if (name == s.type)
        {
            setType(attribute.value().toString());
            continue;
        }
        reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            m_settings.insert(tag, reader.readElementText());
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

void mbCoreDomTaskInfo::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    mbCoreDomTaskInfo::Strings s = mbCoreDomTaskInfo::Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName  : tagName);
    writer.writeAttribute(s.name, name());
    writer.writeAttribute(s.type, type());

    for (MBSETTINGS::const_iterator i = m_settings.constBegin(); i != m_settings.constEnd(); i++)
        writer.writeTextElement(i.key(), i.value().toString());
    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbCoreDomProject::Strings::Strings() :
    tagName(QStringLiteral("project")),
    name   (QStringLiteral("name")),
    author (QStringLiteral("author")),
    comment(QStringLiteral("comment"))
{

}

const mbCoreDomProject::Strings &mbCoreDomProject::Strings::instance()
{
    static Strings s;
    return s;
}

mbCoreDomProject::mbCoreDomProject(mbCoreDomPorts      *ports,
                                   mbCoreDomDevices    *devices,
                                   mbCoreDomDataViews *dataViews)
{
    m_ports         = ports     ;
    m_devices       = devices   ;
    m_dataViews    = dataViews;
    m_tasks = new mbCoreDomTasks;
}

mbCoreDomProject::~mbCoreDomProject()
{
    delete m_ports     ;
    delete m_devices   ;
    delete m_dataViews;
    delete m_tasks;
}

void mbCoreDomProject::read(QXmlStreamReader &reader)
{
    mbCoreDomProject::Strings s = mbCoreDomProject::Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (readAttribute(reader, attribute))
            continue;
        reader.raiseError(QStringLiteral("Unexpected attribute ") + name.toString());
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case QXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            if (tag == s.name)
            {
                setName(reader.readElementText());
                continue;
            }
            if (tag == s.author)
            {
                setAuthor(reader.readElementText());
                continue;
            }
            if (tag == s.comment)
            {
                setComment(reader.readElementText());
                continue;
            }
            if (tag == m_ports->tagItems())
            {
                m_ports->read(reader);
                continue;
            }
            if (tag == m_devices->tagItems())
            {
                m_devices->read(reader);
                continue;
            }
            if (tag == m_dataViews->tagItems())
            {
                m_dataViews->read(reader);
                continue;
            }
            if (tag == m_tasks->tagItems())
            {
                m_tasks->read(reader);
                continue;
            }
            if (readElement(reader, tag))
                continue;
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

void mbCoreDomProject::write(QXmlStreamWriter &writer, const QString &tagName) const
{
    mbCoreDomProject::Strings s = mbCoreDomProject::Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writeAttributes(writer);

    writer.writeTextElement(s.name, name());
    writer.writeTextElement(s.author, author());
    writer.writeTextElement(s.comment, comment());
    if (m_ports->itemCount())
        m_ports->write(writer);
    if (m_devices->itemCount())
        m_devices->write(writer);
    if (m_dataViews->itemCount())
        m_dataViews->write(writer);
    if (m_tasks->itemCount())
        m_tasks->write(writer);
    writeElements(writer);

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}

bool mbCoreDomProject::readAttribute(QXmlStreamReader &/*reader*/, const QXmlStreamAttribute &/*attribute*/)
{
    return false;
}

void mbCoreDomProject::writeAttributes(QXmlStreamWriter &/*writer*/) const
{
}

bool mbCoreDomProject::readElement(QXmlStreamReader &/*reader*/, const QString &/*tag*/)
{
    return false;
}

void mbCoreDomProject::writeElements(QXmlStreamWriter &/*writer*/) const
{
}
