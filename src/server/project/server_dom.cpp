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
#include "server_dom.h"

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- ACTION -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbServerDomAction::Strings::Strings() :
    tagName      (QStringLiteral("action")),
    device       (QStringLiteral("device")),
    address      (QStringLiteral("address")),
    dataType     (QStringLiteral("dataType")),
    period       (QStringLiteral("period")),
    comment      (QStringLiteral("comment")),
    actionType   (QStringLiteral("actionType")),
    extended     (QStringLiteral("extended")),
    byteOrder    (QStringLiteral("byteOrder")),
    registerOrder(QStringLiteral("registerOrder"))
{
}

const mbServerDomAction::Strings &mbServerDomAction::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDomAction::mbServerDomAction()
{
    m_period = 0;
}

mbServerDomAction::~mbServerDomAction()
{
}

void mbServerDomAction::read(mbCoreXmlStreamReader &reader)
{
    const Strings &s = Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        reader.raiseWarning(QString("Unexpected attribute '%1'").arg(name.toString()));
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case mbCoreXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            if (tag == s.device)
            {
                setDevice(reader.readElementText());
                continue;
            }
            if (tag == s.address)
            {
                setAddress(reader.readElementText());
                continue;
            }
            if (tag == s.dataType)
            {
                setDataType(reader.readElementText());
                continue;
            }
            if (tag == s.period)
            {
                setPeriod(reader.readElementText().toInt());
                continue;
            }
            if (tag == s.comment)
            {
                setComment(reader.readElementText());
                continue;
            }
            if (tag == s.actionType)
            {
                setActionType(reader.readElementText());
                continue;
            }
            if (tag == s.extended)
            {
                setExtended(reader.readElementText());
                continue;
            }
            if (tag == s.byteOrder)
            {
                setByteOrder(reader.readElementText());
                continue;
            }
            if (tag == s.registerOrder)
            {
                setRegisterOrder(reader.readElementText());
                continue;
            }
            reader.processUnexpectedElement(tag);
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

void mbServerDomAction::write(mbCoreXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);

    writer.writeTextElement(s.device        , device());
    writer.writeTextElement(s.address       , address());
    writer.writeTextElement(s.dataType      , dataType());
    writer.writeTextElement(s.period        , QString::number(period()));
    writer.writeTextElement(s.comment       , comment());
    writer.writeTextElement(s.actionType    , actionType());
    writer.writeTextElement(s.extended      , extended());
    writer.writeTextElement(s.byteOrder     , byteOrder());
    writer.writeTextElement(s.registerOrder , registerOrder());

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DATA VIEW ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbServerDomDataViewItem::mbServerDomDataViewItem() : mbCoreDomDataViewItem()
{
}

mbServerDomDataView::mbServerDomDataView() : mbCoreDomDataView()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbServerDomDeviceData::Strings::Strings() :
    tagName(QStringLiteral("tagName")),
    offset (QStringLiteral("offset")),
    count  (QStringLiteral("count"))
{
}

const mbServerDomDeviceData::Strings &mbServerDomDeviceData::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDomDeviceData::mbServerDomDeviceData()
{
    m_attr_count = 0;
    m_attr_offset = 0;
    m_has_attr_count = false;
    m_has_attr_offset = false;
    m_has_data = false;
}

mbServerDomDeviceData::~mbServerDomDeviceData()
{
}

void mbServerDomDeviceData::read(mbCoreXmlStreamReader &reader)
{
    const Strings &s = Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (name == s.count)
        {
            setCount(attribute.value().toInt());
            continue;
        }
        if (name == s.offset)
        {
            setOffset(static_cast<quint16>(attribute.value().toUInt()));
            continue;
        }
        reader.raiseWarning(QString("Unexpected attribute '%1'").arg(name.toString()));
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case mbCoreXmlStreamReader::StartElement :
            reader.processUnexpectedElement(reader.name().toString());
            break;
        case mbCoreXmlStreamReader::EndElement :
            finished = true;
            break;
        case mbCoreXmlStreamReader::Characters :
            if (!reader.isWhitespace())
                m_text.append(reader.text().toString());
            break;
        default:
            break;
        }
    }
}

void mbServerDomDeviceData::write(mbCoreXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writer.writeAttribute(s.count, QString::number(count()));
    if (m_has_attr_offset)
        writer.writeAttribute(s.offset, QString::number(offset()));

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}

mbServerDomDevice::Strings::Strings() : mbCoreDomDevice::Strings(),
    data0x(QStringLiteral("data0x")),
    data1x(QStringLiteral("data1x")),
    data3x(QStringLiteral("data3x")),
    data4x(QStringLiteral("data4x"))
{
}

const mbServerDomDevice::Strings &mbServerDomDevice::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDomDevice::mbServerDomDevice()
{
}

bool mbServerDomDevice::readElement(mbCoreXmlStreamReader &reader, const QString &tag)
{
    const Strings &s = Strings::instance();

    if      (tag == s.data0x)
        m_data0x.read(reader);
    else if (tag == s.data1x)
        m_data1x.read(reader);
    else if (tag == s.data3x)
        m_data3x.read(reader);
    else if (tag == s.data4x)
        m_data4x.read(reader);
    else
        return mbCoreDomDevice::readElement(reader, tag);
    return true;
}

void mbServerDomDevice::writeElements(mbCoreXmlStreamWriter &writer) const
{
    const Strings &s = Strings::instance();

    mbCoreDomDevice::writeElements(writer);
    if (m_data0x.hasData())
        m_data0x.write(writer, s.data0x);
    if (m_data1x.hasData())
        m_data1x.write(writer, s.data1x);
    if (m_data3x.hasData())
        m_data3x.write(writer, s.data3x);
    if (m_data4x.hasData())
        m_data4x.write(writer, s.data4x);
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DEVICEREF ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------
mbServerDomDeviceRef::Strings::Strings() :
    tagName(QStringLiteral("deviceref")),
    name   (QStringLiteral("name"))
{
}

const mbServerDomDeviceRef::Strings &mbServerDomDeviceRef::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDomDeviceRef::mbServerDomDeviceRef()
{
}

mbServerDomDeviceRef::~mbServerDomDeviceRef()
{
}

void mbServerDomDeviceRef::read(mbCoreXmlStreamReader &reader)
{
    const Strings &s = Strings::instance();

    Q_FOREACH (const QXmlStreamAttribute &attribute, reader.attributes())
    {
        QStringRef name = attribute.name();
        if (name == s.name)
        {
            setName(attribute.value().toString());
            continue;
        }
        reader.raiseWarning(QString("Unexpected attribute '%1'").arg(name.toString()));
    }

    for (bool finished = false; !finished && !reader.hasError();)
    {
        switch (reader.readNext())
        {
        case mbCoreXmlStreamReader::StartElement :
        {
            const QString tag = reader.name().toString();
            reader.processUnexpectedElement(tag);
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

void mbServerDomDeviceRef::write(mbCoreXmlStreamWriter &writer, const QString &tagName) const
{
    const Strings &s = Strings::instance();

    writer.writeStartElement(tagName.isEmpty() ? s.tagName : tagName);
    writer.writeAttribute(s.name, name());

    if (!m_text.isEmpty())
        writer.writeCharacters(m_text);

    writer.writeEndElement();
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbServerDomPort::Strings::Strings() : mbCoreDomPort::Strings()
{
}

const mbServerDomPort::Strings &mbServerDomPort::Strings::instance()
{
    static Strings s;
    return s;
}

mbServerDomPort::mbServerDomPort() : mbCoreDomPort()
{
    m_devices = new mbServerDomDeviceRefs();
}

mbServerDomPort::~mbServerDomPort()
{
    delete m_devices;
}

bool mbServerDomPort::readElement(mbCoreXmlStreamReader &reader, const QString &tag)
{
    const mbServerDomDeviceRef::Strings &sDeviceRef = mbServerDomDeviceRef::Strings::instance();

    if (tag == sDeviceRef.tagName)
        m_devices->read(reader);
    else
        return mbCoreDomPort::readElement(reader, tag);
    return true;
}

void mbServerDomPort::writeElements(mbCoreXmlStreamWriter &writer) const
{
    const mbServerDomDeviceRef::Strings &sDeviceRef = mbServerDomDeviceRef::Strings::instance();

    mbCoreDomPort::writeElements(writer);
    m_devices->write(writer, sDeviceRef.tagName);
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbServerDomProject::mbServerDomProject() : mbCoreDomProject(new mbServerDomPorts,
                                                            new mbServerDomDevices,
                                                            new mbServerDomDataViews)
{
    m_actions = new mbServerDomActions;
}

mbServerDomProject::~mbServerDomProject()
{
    delete m_actions;
}

bool mbServerDomProject::readElement(mbCoreXmlStreamReader &reader, const QString &tag)
{
    if (tag == m_actions->tagItems())
        m_actions->read(reader);
    else
        return mbCoreDomProject::readElement(reader, tag);
    return true;
}

void mbServerDomProject::writeElements(mbCoreXmlStreamWriter &writer) const
{
    mbCoreDomProject::writeElements(writer);
    if (m_actions->itemCount())
        m_actions->write(writer);
}

