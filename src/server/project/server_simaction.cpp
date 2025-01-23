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
#include "server_simaction.h"

#include "server_device.h"

mbServerSimAction::Strings::Strings() :
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

const mbServerSimAction::Strings &mbServerSimAction::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerSimAction::Defaults::Defaults() :
    address          (400001),
    dataType         (mb::Int16),
    period           (1000),
    comment          (QString()),
    actionType       (Increment),
    byteOrder        (mb::LessSignifiedFirst),
    registerOrder    (mb::R0R1R2R3),
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

const mbServerSimAction::Defaults &mbServerSimAction::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbServerSimAction::mbServerSimAction(QObject *parent) : QObject(parent)
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

mbServerSimAction::~mbServerSimAction()
{
}

void mbServerSimAction::setActionType(ActionType actionType)
{
    if (m_actionType != actionType)
    {
        delete m_actionExtended;
        setNewActionExtended(actionType);
    }
}

QString mbServerSimAction::actionTypeStr() const
{
    QMetaEnum me = QMetaEnum::fromType<ActionType>();
    return QString(me.valueToKey(m_actionType));
}

void mbServerSimAction::setActionTypeStr(const QString &actionTypeStr)
{
    QMetaEnum me = QMetaEnum::fromType<ActionType>();
    bool ok;
    int k = me.keyToValue(actionTypeStr.toLatin1(), &ok);
    if (ok)
        setActionType(static_cast<ActionType>(k));
}

QString mbServerSimAction::dataTypeStr() const
{
    return mb::enumDataTypeKey(m_dataType);
}

void mbServerSimAction::setDataTypeStr(const QString &dataTypeStr)
{
    bool ok;
    mb::DataType dataType = mb::enumDataTypeValue(dataTypeStr, &ok);
    if (ok)
        setDataType(dataType);
}

MBSETTINGS mbServerSimAction::commonSettings() const
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
    p[s.registerOrder] = mb::toString(registerOrder());
    p[s.extended     ] = extendedSettingsStr();
    return p;
}

void mbServerSimAction::setCommonSettings(const MBSETTINGS &settings)
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
        mb::RegisterOrder v = mb::toRegisterOrder(it.value(), &ok);
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

MBSETTINGS mbServerSimAction::extendedSettings() const
{
    return m_actionExtended->extendedSettings();
}

void mbServerSimAction::setExtendedSettings(const MBSETTINGS &settings)
{
    m_actionExtended->setExtendedSettings(settings);
}

QString mbServerSimAction::extendedSettingsStr() const
{
    return m_actionExtended->extendedSettingsStr();
}

void mbServerSimAction::setExtendedSettingsStr(const QString &settings)
{
    m_actionExtended->setExtendedSettingsStr(settings);
}

MBSETTINGS mbServerSimAction::settings() const
{
    MBSETTINGS p = commonSettings();
    mb::unite(p, extendedSettings());
    return p;
}

void mbServerSimAction::setSettings(const MBSETTINGS &settings)
{
    setCommonSettings(settings);
    setExtendedSettings(settings);
}

int mbServerSimAction::bitLength() const
{
    switch (m_dataType)
    {
    case mb::Bit:
        return 0;
    default:
        return MB_BYTE_SZ_BITES * mb::sizeOfDataType(m_dataType);
    }
}

int mbServerSimAction::byteLength() const
{
    return mb::sizeOfDataType(m_dataType);
}

int mbServerSimAction::length() const
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

QString mbServerSimAction::byteOrderStr() const
{
    return mb::enumDataOrderKey(m_byteOrder);
}

void mbServerSimAction::setByteOrderStr(const QString &order)
{
    bool ok;
    mb::DataOrder k = mb::enumDataOrderValue(order, &ok);
    if (ok)
        m_byteOrder = k;
}

QString mbServerSimAction::registerOrderStr() const
{
    return mb::toString(m_registerOrder);
}

void mbServerSimAction::setRegisterOrderStr(const QString &registerOrderStr)
{
    bool ok;
    mb::RegisterOrder k = mb::toRegisterOrder(registerOrderStr, &ok);
    if (ok)
        m_registerOrder = k;
}

mb::RegisterOrder mbServerSimAction::getRegisterOrder() const
{
    if (m_registerOrder == mb::DefaultRegisterOrder)
    {
        if (m_device && (m_device->registerOrder() != mb::DefaultRegisterOrder))
            return m_device->registerOrder();
        return mb::R0R1R2R3;
    }
    return m_registerOrder;
}

void mbServerSimAction::setNewActionExtended(ActionType actionType)
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

mbServerSimAction::ActionExtended::~ActionExtended()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ INCREMENT ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerSimAction::ActionIncrement::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.incrementValue] = value;
    p[s.incrementMin  ] = min  ;
    p[s.incrementMax  ] = max  ;
    return p;
}

void mbServerSimAction::ActionIncrement::setExtendedSettings(const MBSETTINGS &settings)
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

QString mbServerSimAction::ActionIncrement::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4;%5=%6").arg(s.incrementValue, value.toString(),
                                            s.incrementMin, min.toString(),
                                            s.incrementMax, max.toString());
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- SINE --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerSimAction::ActionSine::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.sinePeriod       ] = sinePeriod   ;
    p[s.sinePhaseShift   ] = phaseShift   ;
    p[s.sineAmplitude    ] = amplitude    ;
    p[s.sineVerticalShift] = verticalShift;
    return p;
}

void mbServerSimAction::ActionSine::setExtendedSettings(const MBSETTINGS &settings)
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

QString mbServerSimAction::ActionSine::extendedSettingsStr() const
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

MBSETTINGS mbServerSimAction::ActionRandom::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.randomMin] = min;
    p[s.randomMax] = max;
    return p;
}

void mbServerSimAction::ActionRandom::setExtendedSettings(const MBSETTINGS &settings)
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

QString mbServerSimAction::ActionRandom::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4").arg(s.randomMin, min.toString(),
                                      s.randomMax, max.toString());
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- COPY --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

MBSETTINGS mbServerSimAction::ActionCopy::extendedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS p;
    p[s.copySourceAddress] = mb::toInt(sourceAddress);
    p[s.copySize         ] = size;
    return p;
}

void mbServerSimAction::ActionCopy::setExtendedSettings(const MBSETTINGS &settings)
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

QString mbServerSimAction::ActionCopy::extendedSettingsStr() const
{
    const Strings &s = Strings::instance();
    return QString("%1=%2;%3=%4").arg(s.copySourceAddress, mb::toString(sourceAddress),
                                      s.copySize         , QString::number(size));
}

