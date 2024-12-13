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
#include "server_action.h"

mbServerAction::Strings::Strings() :
    device           (QStringLiteral("device")),
    address          (QStringLiteral("address")),
    dataType         (QStringLiteral("dataType")),
    period           (QStringLiteral("period")),
    comment          (QStringLiteral("comment")),
    actionType       (QStringLiteral("actionType")),
    byteOrder        (QStringLiteral("byteOrder")),
    registerOrder    (QStringLiteral("registerOrder")),
    extended         (QStringLiteral("extended")),
    incrementValue   (QStringLiteral("incrementValue")),
    incrementMin     (QStringLiteral("min")),
    incrementMax     (QStringLiteral("max")),
    sinePeriod       (QStringLiteral("sinePeriod")),
    sinePhaseShift   (QStringLiteral("sinePhaseShift")),
    sineAmplitude    (QStringLiteral("sineAmplitude")),
    sineVerticalShift(QStringLiteral("sineVerticalShift")),
    randomMin        (QStringLiteral("randomMin")),
    randomMax        (QStringLiteral("randomMax")),
    copySourceAddress(QStringLiteral("sourceAddress")),
    copySize         (QStringLiteral("size"))
{
}

const mbServerAction::Strings &mbServerAction::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerAction::Defaults::Defaults() :
    address          (400001),
    dataType         (mb::Int16),
    period           (1000),
    comment          (QString()),
    actionType       (Increment),
    byteOrder        (mb::LessSignifiedFirst),
    registerOrder    (mb::LessSignifiedFirst),
    incrementValue   (1),
    incrementMin     (0),
    incrementMax     (65535),
    sinePeriod       (10000),
    sinePhaseShift   (0),
    sineAmplitude    (100),
    sineVerticalShift(0),
    randomMin        (0),
    randomMax        (100),
    copySourceAddress(300001),
    copySize         (1)
{
}

const mbServerAction::Defaults &mbServerAction::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbServerAction::mbServerAction(QObject *parent) : QObject(parent)
{
    Defaults d = Defaults::instance();

    m_device = nullptr;
    m_address = mb::toAddress(d.address);
    m_period = d.period;
    m_dataType = d.dataType;
    setNewActionExtended(d.actionType);
    m_byteOrder = d.byteOrder;
    m_registerOrder = d.registerOrder;

}

mbServerAction::~mbServerAction()
{
}

void mbServerAction::setActionType(ActionType actionType)
{
    if (m_actionType != actionType)
    {
        delete m_actionExtended;
        setNewActionExtended(actionType);
    }
}

QString mbServerAction::actionTypeStr() const
{
    QMetaEnum me = QMetaEnum::fromType<ActionType>();
    return QString(me.valueToKey(m_actionType));
}

void mbServerAction::setActionTypeStr(const QString &actionTypeStr)
{
    QMetaEnum me = QMetaEnum::fromType<ActionType>();
    bool ok;
    int k = me.keyToValue(actionTypeStr.toLatin1(), &ok);
    if (ok)
        setActionType(static_cast<ActionType>(k));
}

QString mbServerAction::dataTypeStr() const
{
    return mb::enumDataTypeKey(m_dataType);
}

void mbServerAction::setDataTypeStr(const QString &dataTypeStr)
{
    bool ok;
    mb::DataType dataType = mb::enumDataTypeValue(dataTypeStr, &ok);
    if (ok)
        setDataType(dataType);
}

MBSETTINGS mbServerAction::commonSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.device       ] = QVariant::fromValue<void*>(device());
    p[s.address      ] = addressInt();
    p[s.dataType     ] = mb::enumDataTypeKey(dataType());
    p[s.period       ] = period();
    p[s.comment      ] = comment();
    p[s.actionType   ] = mb::enumKey(actionType());
    p[s.byteOrder    ] = mb::enumDataOrderKey(byteOrder());
    p[s.registerOrder] = mb::enumDataOrderKey(registerOrder());
    p[s.extended     ] = extendedSettingsStr();
    return p;
}

void mbServerAction::setCommonSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    bool ok;

    it = settings.find(s.device);
    if (it != end)
    {
        mbServerDevice *v = reinterpret_cast<mbServerDevice*>(it.value().value<void*>());
        setDevice(v);
    }

    it = settings.find(s.address);
    if (it != end)
    {
        QVariant var = it.value();
        setAddress(var.toInt());
    }

    it = settings.find(s.dataType);
    if (it != end)
    {
        mb::DataType v = mb::enumDataTypeValue(it.value(), &ok);
        if (ok)
            setDataType(v);
    }

    it = settings.find(s.period);
    if (it != end)
    {
        int v = it.value().toInt(&ok);
        if (ok)
            setPeriod(v);
    }

    it = settings.find(s.comment);
    if (it != end)
    {
        QString v = it.value().toString();
        setComment(v);
    }

    it = settings.find(s.actionType);
    if (it != end)
    {
        ActionType v = mb::enumValue<ActionType>(it.value(), &ok);
        if (ok)
            setActionType(v);
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

    it = settings.find(s.extended);
    if (it != end)
    {
        QString v = it.value().toString();
        setExtendedSettingsStr(v);
    }


}

MBSETTINGS mbServerAction::extendedSettings() const
{
    return m_actionExtended->extendedSettings();
}

void mbServerAction::setExtendedSettings(const MBSETTINGS &settings)
{
    m_actionExtended->setExtendedSettings(settings);
}

QString mbServerAction::extendedSettingsStr() const
{
    return m_actionExtended->extendedSettingsStr();
}

void mbServerAction::setExtendedSettingsStr(const QString &settings)
{
    m_actionExtended->setExtendedSettingsStr(settings);
}

MBSETTINGS mbServerAction::settings() const
{
    MBSETTINGS p = commonSettings();
    mb::unite(p, extendedSettings());
    return p;
}

void mbServerAction::setSettings(const MBSETTINGS &settings)
{
    setCommonSettings(settings);
    setExtendedSettings(settings);
}

int mbServerAction::bitLength() const
{
    switch (m_dataType)
    {
    case mb::Bit:
        return 0;
    default:
        return MB_BYTE_SZ_BITES * mb::sizeOfDataType(m_dataType);
    }
}

int mbServerAction::byteLength() const
{
    return mb::sizeOfDataType(m_dataType);
}

int mbServerAction::length() const
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

QString mbServerAction::byteOrderStr() const
{
    return mb::enumDataOrderKey(m_byteOrder);
}

void mbServerAction::setByteOrderStr(const QString &order)
{
    bool ok;
    mb::DataOrder k = mb::enumDataOrderValue(order, &ok);
    if (ok)
        m_byteOrder = k;
}

QString mbServerAction::registerOrderStr() const
{
    return mb::enumDataOrderKey(m_registerOrder);
}

void mbServerAction::setRegisterOrderStr(const QString &registerOrderStr)
{
    bool ok;
    mb::DataOrder k = mb::enumDataOrderValue(registerOrderStr, &ok);
    if (ok)
        m_registerOrder = k;
}

void mbServerAction::setNewActionExtended(ActionType actionType)
{
    switch (actionType)
    {
    case Increment:
        m_actionExtended = new ActionIncrement;
        break;
    case Sine:
        m_actionExtended = new ActionSine;
        break;
    case Random:
        m_actionExtended = new ActionRandom;
        break;
    case Copy:
        m_actionExtended = new ActionCopy;
        break;
    default:
        return;
    }
    m_actionType = actionType;
}

mbServerAction::ActionExtended::~ActionExtended()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ INCREMENT ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerAction::ActionIncrement::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.incrementValue] = value;
    p[s.incrementMin  ] = min  ;
    p[s.incrementMax  ] = max  ;
    return p;
}

void mbServerAction::ActionIncrement::setExtendedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    auto end = settings.end();

    auto it = settings.find(s.incrementValue);
    if (it != end)
        value = it.value();

    it = settings.find(s.incrementMin);
    if (it != end)
        min = it.value();

    it = settings.find(s.incrementMax);
    if (it != end)
        max = it.value();
}

QString mbServerAction::ActionIncrement::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4;%5=%6").arg(s.incrementValue, value.toString(),
                                            s.incrementMin, min.toString(),
                                            s.incrementMax, max.toString());
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- SINE --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerAction::ActionSine::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.sinePeriod       ] = sinePeriod   ;
    p[s.sinePhaseShift   ] = phaseShift   ;
    p[s.sineAmplitude    ] = amplitude    ;
    p[s.sineVerticalShift] = verticalShift;
    return p;
}

void mbServerAction::ActionSine::setExtendedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    auto end = settings.end();
    bool ok;

    it = settings.find(s.sinePeriod);
    if (it != end)
    {
        qint64 v = it.value().toLongLong(&ok);
        if (ok)
            sinePeriod = v;
    }

    it = settings.find(s.sinePhaseShift);
    if (it != end)
    {
        qint64 v = it.value().toLongLong(&ok);
        if (ok)
            phaseShift = v;
    }

    it = settings.find(s.sineAmplitude);
    if (it != end)
    {
        amplitude = it.value();
    }

    it = settings.find(s.sineVerticalShift);
    if (it != end)
    {
        verticalShift = it.value();
    }
}

QString mbServerAction::ActionSine::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4;%5=%6;%7=%8")
        .arg(s.sinePeriod       , QString::number(sinePeriod),
             s.sinePhaseShift   , QString::number(phaseShift),
             s.sineAmplitude    , amplitude.toString(),
             s.sineVerticalShift, verticalShift.toString());
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- RANDOM -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerAction::ActionRandom::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.randomMin] = min;
    p[s.randomMax] = max;
    return p;
}

void mbServerAction::ActionRandom::setExtendedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    auto end = settings.end();

    it = settings.find(s.randomMin);
    if (it != end)
        min = it.value();

    it = settings.find(s.randomMax);
    if (it != end)
        max = it.value();
}

QString mbServerAction::ActionRandom::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4").arg(s.randomMin, min.toString(),
                                      s.randomMax, max.toString());
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- COPY --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerAction::ActionCopy::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.copySourceAddress] = mb::toInt(sourceAddress);
    p[s.copySize         ] = size;
    return p;
}

void mbServerAction::ActionCopy::setExtendedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    auto end = settings.end();

    it = settings.find(s.copySourceAddress);
    if (it != end)
        sourceAddress = mb::toAddress(it.value().toInt());

    it = settings.find(s.copySize);
    if (it != end)
        size = static_cast<quint16>(it.value().toUInt());
}

QString mbServerAction::ActionCopy::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4").arg(s.copySourceAddress, mb::toString(sourceAddress),
                                      s.copySize         , QString::number(size));
}

