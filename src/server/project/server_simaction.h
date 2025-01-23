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
#ifndef SERVER_SIMACTION_H
#define SERVER_SIMACTION_H

#include <QPointer>

#include <server_global.h>
#include "server_device.h"

class mbServerSimAction : public QObject
{
    Q_OBJECT
public:
    enum ActionType
    {
        Increment,
        Sine,
        Random,
        Copy
    };
    Q_ENUM(ActionType)

    struct Strings
    {
        const QString device           ;
        const QString address          ;
        const QString dataType         ;
        const QString period           ;
        const QString comment          ;
        const QString actionType       ;
        const QString byteOrder        ;
        const QString registerOrder    ;
        const QString extended         ;
        const QString incrementValue   ;
        const QString incrementMin     ;
        const QString incrementMax     ;
        const QString sinePeriod       ;
        const QString sinePhaseShift   ;
        const QString sineAmplitude    ;
        const QString sineVerticalShift;
        const QString randomMin        ;
        const QString randomMax        ;
        const QString copySourceAddress;
        const QString copySize         ;

        Strings();
        static const Strings &instance();
    };

    struct Defaults
    {
        const int               address          ;
        const mb::DataType      dataType         ;
        const int               period           ;
        const QString           comment          ;
        const ActionType        actionType       ;
        const mb::DataOrder     byteOrder        ;
        const mb::RegisterOrder registerOrder    ;
        const int               incrementValue   ;
        const int               incrementMin     ;
        const int               incrementMax     ;
        const int               sinePeriod       ;
        const int               sinePhaseShift   ;
        const int               sineAmplitude    ;
        const int               sineVerticalShift;
        const int               randomMin        ;
        const int               randomMax        ;
        const int               copySourceAddress;
        const quint16           copySize         ;

        Defaults();
        static const Defaults &instance();
    };

public:
    mbServerSimAction(QObject *parent = nullptr);
    virtual ~mbServerSimAction();

public:
    inline ActionType actionType() const { return m_actionType; }
    void setActionType(ActionType actionType);
    QString actionTypeStr() const;
    void setActionTypeStr(const QString& actionTypeStr);

    inline mbServerDevice *device() const { return m_device.data(); }
    inline void setDevice(mbServerDevice *device) { m_device = device; }
    inline mb::Address address() const { return m_address; }
    inline int addressInt() const { return mb::toInt(m_address); }
    inline QString addressStr() const { return mb::toString(m_address); }
    inline void setAddress(const mb::Address& address) { m_address = address; }
    inline void setAddressInt(int address) { setAddress(mb::toAddress(address)); }
    inline void setAddressStr(const QString& address) { setAddress(mb::toAddress(address)); }
    inline void setAddress(int address) { setAddressInt(address); }
    inline void setAddress(const QString& address) { setAddressStr(address); }
    void setAddress(Modbus::MemoryType type, quint16 offset);
    inline Modbus::MemoryType addressType() const { return m_address.type; }
    inline quint16 addressOffset() const { return m_address.offset; }
    inline mb::DataType dataType() const { return m_dataType; }
    inline void setDataType(mb::DataType dataType) { m_dataType = dataType; }
    QString dataTypeStr() const;
    void setDataTypeStr(const QString& dataTypeStr);
    inline int period() const { return m_period; }
    inline void setPeriod(int period) { m_period = period; }
    inline QString comment() const { return m_comment; }
    inline void setComment(const QString &comment) { m_comment = comment; }
    int bitLength() const;
    int byteLength() const;
    inline int registerLength() const { return (byteLength()+1)/2; }
    int length() const;
    inline int count() const { return length(); }

    inline mb::DataOrder byteOrder() const { return m_byteOrder; }
    inline void setByteOrder(mb::DataOrder order) { m_byteOrder = order; }
    QString byteOrderStr() const;
    void setByteOrderStr(const QString& order);

    inline mb::RegisterOrder registerOrder() const { return m_registerOrder; }
    inline void setRegisterOrder(mb::RegisterOrder registerOrder) { m_registerOrder = registerOrder; }
    QString registerOrderStr() const;
    void setRegisterOrderStr(const QString& registerOrderStr);

    mb::RegisterOrder getRegisterOrder() const;

    MBSETTINGS commonSettings() const;
    void setCommonSettings(const MBSETTINGS &settings);
    MBSETTINGS extendedSettings() const;
    void setExtendedSettings(const MBSETTINGS &settings);
    QString extendedSettingsStr() const;
    void setExtendedSettingsStr(const QString &settings);
    MBSETTINGS settings() const;
    void setSettings(const MBSETTINGS &settings);

Q_SIGNALS:
    void changed();

private:
    struct ActionExtended
    {
        virtual ~ActionExtended();
        virtual MBSETTINGS extendedSettings() const = 0;
        virtual void setExtendedSettings(const MBSETTINGS &settings) = 0;
        virtual QString extendedSettingsStr() const = 0;
        inline void setExtendedSettingsStr(const QString &settings) { setExtendedSettings(mb::parseExtendedAttributesStr(settings)); }
    };

    struct ActionIncrement : public ActionExtended
    {
        QVariant value;
        QVariant min;
        QVariant max;
        MBSETTINGS extendedSettings() const override;
        void setExtendedSettings(const MBSETTINGS &settings) override;
        QString extendedSettingsStr() const override;
        ActionIncrement()
        {
            Defaults d = Defaults::instance();
            value = d.incrementValue;
            min = d.incrementMin;
            max = d.incrementMax;
        }
    };

    struct ActionSine : public ActionExtended
    {
        qint64 sinePeriod;
        qint64 phaseShift;
        QVariant amplitude;
        QVariant verticalShift;

        MBSETTINGS extendedSettings() const override;
        void setExtendedSettings(const MBSETTINGS &settings) override;
        QString extendedSettingsStr() const override;

        ActionSine()
        {
            Defaults d = Defaults::instance();
            sinePeriod    = d.sinePeriod       ;
            phaseShift    = d.sinePhaseShift   ;
            amplitude     = d.sineAmplitude    ;
            verticalShift = d.sineVerticalShift;
        }
    };

    struct ActionRandom : public ActionExtended
    {
        QVariant min;
        QVariant max;

        MBSETTINGS extendedSettings() const override;
        void setExtendedSettings(const MBSETTINGS &settings) override;
        QString extendedSettingsStr() const override;

        ActionRandom()
        {
            Defaults d = Defaults::instance();
            min = d.randomMin;
            max = d.randomMax;
        }
    };

    struct ActionCopy : public ActionExtended
    {
        mb::Address sourceAddress;
        quint16 size;

        MBSETTINGS extendedSettings() const override;
        void setExtendedSettings(const MBSETTINGS &settings) override;
        QString extendedSettingsStr() const override;

        ActionCopy()
        {
            Defaults d = Defaults::instance();
            sourceAddress = mb::toAddress(d.copySourceAddress);
            size = d.copySize;
        }
    };

private:
    void setNewActionExtended(ActionType actionType);

private:
    ActionType m_actionType;
    ActionExtended *m_actionExtended;

    QPointer<mbServerDevice> m_device;
    mb::Address m_address;
    mb::DataType m_dataType;
    int m_period;
    QString m_comment;
    mb::DataOrder m_byteOrder;
    mb::RegisterOrder m_registerOrder;
};

#endif // SERVER_SIMACTION_H
