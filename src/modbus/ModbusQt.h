/*!
 * \file   ModbusQt.h
 * \brief  Qt support file for ModbusLib.
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSQT_H
#define MODBUSQT_H

#include "Modbus.h"

#include <QMetaEnum>
#include <QHash>
#include <QVariant>

namespace Modbus {

/// \brief Map for settings of Modbus protocol where key has type `QString` and value is `QVariant`.
typedef QHash<QString, QVariant> Settings;

/*! \brief Sets constant key values for the map of settings.
 */
class MODBUS_EXPORT Strings
{
public:
    const QString unit            ; ///< Setting key for the unit number of remote device
    const QString type            ; ///< Setting key for the type of Modbus protocol
    const QString host            ; ///< Setting key for the IP address or DNS name of the remote device
    const QString port            ; ///< Setting key for the TCP port number of the remote device
    const QString timeout         ; ///< Setting key for connection timeout (milliseconds)
    const QString serialPortName  ; ///< Setting key for the serial port name
    const QString baudRate        ; ///< Setting key for the serial port's baud rate
    const QString dataBits        ; ///< Setting key for the serial port's data bits
    const QString parity          ; ///< Setting key for the serial port's patiry
    const QString stopBits        ; ///< Setting key for the serial port's stop bits
    const QString flowControl     ; ///< Setting key for the serial port's flow control
    const QString timeoutFirstByte; ///< Setting key for the serial port's timeout waiting first byte of packet
    const QString timeoutInterByte; ///< Setting key for the serial port's timeout waiting next byte of packet

    /// \details Constructor ot the class.
    Strings();

    /// \details Returns a reference to the global `Modbus::Strings` object.
    static const Strings &instance();
};

/*! \brief Holds the default values of the settings.
*/
class MODBUS_EXPORT Defaults
{
public:
    const uint8_t      unit            ; ///< Default value for the unit number of remote device
    const ProtocolType type            ; ///< Default value for the type of Modbus protocol
    const QString      host            ; ///< Default value for the IP address or DNS name of the remote device
    const uint16_t     port            ; ///< Default value for the TCP port number of the remote device
    const uint32_t     timeout         ; ///< Default value for connection timeout (milliseconds)
    const QString      serialPortName  ; ///< Default value for the serial port name
    const int32_t      baudRate        ; ///< Default value for the serial port's baud rate
    const int8_t       dataBits        ; ///< Default value for the serial port's data bits
    const Parity       parity          ; ///< Default value for the serial port's patiry
    const StopBits     stopBits        ; ///< Default value for the serial port's stop bits
    const FlowControl  flowControl     ; ///< Default value for the serial port's flow control
    const uint32_t     timeoutFirstByte; ///< Default value for the serial port's timeout waiting first byte of packet
    const uint32_t     timeoutInterByte; ///< Default value for the serial port's timeout waiting next byte of packet

    /// \details Constructor ot the class.
    Defaults();

    /// \details Returns a reference to the global `Modbus::Defaults` object.
    static const Defaults &instance();
};

/*! \brief Class for convinient manipulation with Modbus Data Address.
*/
class MODBUS_EXPORT Address
{
public:
    /// \details Defauilt constructor ot the class. Creates invalid Modbus Data Address
    Address();

    /// \details Constructor ot the class. E.g. `Address(Modbus::Memory_4x, 0)` creates `400001` standard address. 
    Address(Modbus::MemoryType, quint16 offset);

    /// \details Constructor ot the class. E.g. `Address(400001)` creates `Address` with type `Modbus::Memory_4x`
    /// and offset `0`, and `Address(1)` creates `Address` with type `Modbus::Memory_0x` and offset `0`. 
    Address(quint32 adr);

public:
    /// \details Returns `true` if memory type is `Modbus::Memory_Unknown`, `false` otherwise
    inline bool isValid() const { return m_type != Memory_Unknown; }

    /// \details Returns memory type of Modbus Data Address
    inline MemoryType type() const { return static_cast<MemoryType>(m_type); }
 
    /// \details Returns memory offset of Modbus Data Address
    inline quint16 offset() const { return m_offset; }
 
    /// \details Returns memory number (offset+1) of Modbus Data Address
    inline quint32 number() const { return m_offset+1; }
 
    /// \details Returns string repr of Modbus Data Address
    /// e.g. `Address(Modbus::Memory_4x, 0)` will be converted to `QString("400001")`.
    QString toString() const;

    /// \details Converts current Modbus Data Address to `quint32`,
    /// e.g. `Address(Modbus::Memory_4x, 0)` will be converted to `400001`.
    inline operator quint32 () const { return number() | (m_type<<16);  }

    /// \details Assigment operator definition.
    Address &operator= (quint32 v);

private:
    quint16 m_type;
    quint16 m_offset;
};

/// \details Convert String repr to Modbus::Address
inline Address addressFromString(const QString &s) { return Address(s.toUInt()); }

/// \details Convert value to QString key for type
template <class EnumType>
inline QString enumKey(int value)
{
    const QMetaEnum me = QMetaEnum::fromType<EnumType>();
    return QString(me.valueToKey(value));
}

/// \details Convert value to QString key for type
template <class EnumType>
inline QString enumKey(EnumType value, const QString &byDef = QString())
{
    const QMetaEnum me = QMetaEnum::fromType<EnumType>();
    const char *key = me.valueToKey(value);
    if (key)
        return QString(me.valueToKey(value));
    else
        return byDef;
}

/// \details Convert key to value for enumeration by QString key
template <class EnumType>
inline EnumType enumValue(const QString& key, bool* ok = nullptr)
{
    const QMetaEnum me = QMetaEnum::fromType<EnumType>();
    return static_cast<EnumType>(me.keyToValue(key.toLatin1().constData(), ok));

}

/// \details Convert `QVariant` value to enumeration value (int - value, string - key).
/// Stores result of convertion in output parameter `ok`
template <class EnumType>
inline EnumType enumValue(const QVariant& value, bool *ok)
{
    bool okInner;
    int v = value.toInt(&okInner);
    if (okInner)
    {
        const QMetaEnum me = QMetaEnum::fromType<EnumType>();
        if (me.valueToKey(v)) // check value exists
        {
            if (ok)
                *ok = true;
            return static_cast<EnumType>(v);
        }
        if (ok)
            *ok = false;
        return static_cast<EnumType>(-1);
    }
    return enumValue<EnumType>(value.toString(), ok);
}

/// \details Convert `QVariant` value to enumeration value (int - value, string - key).
/// If `value` can't be converted, `defaultValue` is returned.
template <class EnumType>
inline EnumType enumValue(const QVariant& value, EnumType defaultValue)
{
    bool okInner;
    EnumType v = enumValue<EnumType>(value, &okInner);
    if (okInner)
        return v;
    return defaultValue;
}

/// \details Convert `QVariant` value to enumeration value (int - value, string - key).
template <class EnumType>
inline EnumType enumValue(const QVariant& value)
{
    return enumValue<EnumType>(value, nullptr);
}

/// \details Converts string representation to `ProtocolType` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT ProtocolType toProtocolType(const QString &s, bool *ok = nullptr);

/// \details Converts QVariant value to `ProtocolType` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT ProtocolType toProtocolType(const QVariant &v, bool *ok = nullptr);

/// \details Converts string representation to `DataBits` value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT int8_t toDataBits(const QString &s, bool *ok = nullptr);

/// \details Converts QVariant value to `DataBits` value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT int8_t toDataBits(const QVariant &v, bool *ok = nullptr);

/// \details Converts string representation to `Parity` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT Parity toParity(const QString &s, bool *ok = nullptr);

/// \details Converts QVariant value to `Parity` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT Parity toParity(const QVariant &v, bool *ok = nullptr);

/// \details Converts string representation to `StopBits` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT StopBits toStopBits(const QString &s, bool *ok = nullptr);

/// \details Converts QVariant value to `StopBits` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT StopBits toStopBits(const QVariant &v, bool *ok = nullptr);

/// \details Converts string representation to `FlowControl` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT FlowControl toFlowControl(const QString &s, bool *ok = nullptr);

/// \details Converts QVariant value to `FlowControl` enum value.
/// If ok is not nullptr, failure is reported by setting *ok to false, and success by setting *ok to true.
MODBUS_EXPORT FlowControl toFlowControl(const QVariant &v, bool *ok = nullptr);

/// \details Returns string representation of `StatusCode` enum value
MODBUS_EXPORT QString toString(StatusCode v);

/// \details Returns string representation of `ProtocolType` enum value
MODBUS_EXPORT QString toString(ProtocolType v);

/// \details Returns string representation of `Parity` enum value
MODBUS_EXPORT QString toString(Parity v);

/// \details Returns string representation of `StopBits` enum value
MODBUS_EXPORT QString toString(StopBits v);

/// \details Returns string representation of `FlowControl` enum value
MODBUS_EXPORT QString toString(FlowControl v);

/// \details Make string representation of bytes array and separate bytes by space
inline QString bytesToString(const QByteArray &v) { return bytesToString(reinterpret_cast<const uint8_t*>(v.constData()), v.size()).data(); }

/// \details Make string representation of ASCII array and separate bytes by space
inline QString asciiToString(const QByteArray &v) { return bytesToString(reinterpret_cast<const uint8_t*>(v.constData()), v.size()).data(); }

/// \details Returns list of string that represent names of serial ports
MODBUS_EXPORT QStringList availableSerialPortList();

/// \details Same as `Modbus::createPort(ProtocolType type, const void *settings, bool blocking)`
/// but `ProtocolType type` and `const void *settings` are defined by `Modbus::Settings` key-value map.
MODBUS_EXPORT ModbusPort *createPort(const Settings &settings, bool blocking = false);

/// \details Same as `Modbus::createClientPort(ProtocolType type, const void *settings, bool blocking)`
/// but `ProtocolType type` and `const void *settings` are defined by `Modbus::Settings` key-value map.
MODBUS_EXPORT ModbusClientPort *createClientPort(const Settings &settings, bool blocking = false);

/// \details Same as `Modbus::createServerPort(ProtocolType type, const void *settings, bool blocking)`
/// but `ProtocolType type` and `const void *settings` are defined by `Modbus::Settings` key-value map.
MODBUS_EXPORT ModbusServerPort *createServerPort(ModbusInterface *device, const Settings &settings, bool blocking = false);

} // namespace Modbus

#endif // MODBUSQT_H
