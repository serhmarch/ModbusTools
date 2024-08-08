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
#ifndef MBCORE_H
#define MBCORE_H

#include "mbcore_config.h"

#define MBTOOLS_VERSION ((MBTOOLS_VERSION_MAJOR<<16)|(MBTOOLS_VERSION_MINOR<<8)|(MBTOOLS_VERSION_PATCH))

/*
   MB_VERSION_STR "major.minor.patch"
*/
#define MBTOOLS_VERSION_STR_HELPER(major,minor,patch) #major"."#minor"."#patch
#define MBTOOLS_VERSION_STR_MAKE(major,minor,patch) MBTOOLS_VERSION_STR_HELPER(major,minor,patch)
#define MBTOOLS_VERSION_STR MBTOOLS_VERSION_STR_MAKE(MBTOOLS_VERSION_MAJOR,MBTOOLS_VERSION_MINOR,MBTOOLS_VERSION_PATCH)

#include <typeinfo>

#include <QMetaEnum>
#include <QString>
#include <QHash>
#include <QMap>
#include <QVector>
#include <QByteArray>
#include <QVariant>
#include <QAtomicInt>

#if defined(MB_EXPORTS) && defined(Q_DECL_EXPORT)
#define MB_EXPORT Q_DECL_EXPORT
#elif defined(Q_DECL_IMPORT)
#define MB_EXPORT Q_DECL_IMPORT
#else
#define MB_EXPORT
#endif

#include <ModbusQt.h>

#include "mbcore_sharedpointer.h"

#define MB_MEMORY_MAX_COUNT 65536

typedef QHash<QString, QVariant> MBSETTINGS;
typedef QMap<int, QVariant> MBPARAMS;

namespace mb
{
Q_NAMESPACE

typedef QAtomicInt RefCount_t;

#define MB_REF_COUNTING                                                     \
public:                                                                     \
    inline void incRef() { m_refCount++; }                                  \
    inline void decRef() { m_refCount--; if (m_refCount < 1) delete this; } \
private:                                                                    \
    mb::RefCount_t m_refCount;                                              \

typedef qint64 Timestamp_t;

template <class EnumType>
inline QMetaEnum metaEnum()
{
    return QMetaEnum::fromType<EnumType>();
}

// Access meta enumeration object of a qobject
template <class QObjectType>
inline const QMetaEnum metaEnum(const char* name)
{
    const int e_index = QObjectType::staticMetaObject.indexOfProperty(name);
    Q_ASSERT(e_index != -1);
    return QObjectType::staticMetaObject.property(e_index).enumerator();
}

template <class EnumType>
inline QStringList enumKeyList()
{
    const QMetaEnum e = QMetaEnum::fromType<EnumType>();
    QStringList keyList;
    for (int i = 0; i < e.keyCount(); i++)
        keyList.append(e.key(i));
    return keyList;
}


// Convert key to value for a given QMetaEnum
template <class EnumType>
inline EnumType enumValue(const QMetaEnum &metaEnum, const char* key)
{
    int val = metaEnum.keyToValue(key);
    if (val == -1)
        val = metaEnum.value(0);
    return static_cast<EnumType>(val);
}

template <class EnumType>
inline EnumType enumValue(const char* key)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return enumValue<EnumType>(me, key);
}

// Convert key to value for enumeration by name
template <class QObjectType, class EnumType>
inline EnumType enumValue(const char* enumName, const char* key)
{
    const QMetaEnum me = metaEnum<QObjectType>(enumName);
    return enumValue<EnumType>(me, key);
}

template <class EnumType>
inline EnumType enumValueByIndex(int index)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return static_cast<EnumType>(me.value(index));
}

// Convert keys to value for a given QMetaEnum
template <class EnumType>
inline EnumType enumValues(const QMetaEnum &metaEnum, const char* keys)
{
    int val = metaEnum.keysToValue(keys);
    if (val == -1)
        val = 0;
    return static_cast<EnumType>(QFlag(val));
}

// Convert value to key for a given QMetaEnum
inline QString enumKey(const QMetaEnum &metaEnum, int value)
{
    return metaEnum.valueToKey(value);
}

template <class EnumType>
inline QString enumKey(int value)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return enumKey(me, value);
}

template <class EnumType>
inline QString enumKey(EnumType value)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return enumKey(me, value);
}

// Convert value to key for enumeration by name
template <class QObjectType>
inline QString enumKey(const char* enumName, int value)
{
    const QMetaEnum me = metaEnum<QObjectType>(enumName);
    return enumKey(me, value);
}

template <class EnumType>
inline QString enumKeyByIndex(int index)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return me.key(index);
}

// Convert value to keys for a given QMetaEnum
inline QString enumKeys(const QMetaEnum &metaEnum, int value)
{
    return metaEnum.valueToKeys(value);
}

template <class EnumType>
inline QString enumKeys(int value)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return enumKeys(me, value);
}

// Convert value to keys for enumeration by name
template <class QObjectType>
inline QString enumKeys(const char* enumName, int value)
{
    const QMetaEnum me = metaEnum<QObjectType>(enumName);
    return enumKeys(me, value);
}

// Convert value to const char* key for type
template <class EnumType>
inline const char* enumKeyType(int value)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return me.valueToKey(value);
}

// Convert value to QString key for type
template <class EnumType>
inline QString enumKeyTypeStr(int value)
{
    return QString(enumKeyType<EnumType>(value));
}

// Convert key to value for enumeration by char key
template <class EnumType>
inline EnumType enumValueType(const char* key, bool* ok = nullptr)
{
    const QMetaEnum me = metaEnum<EnumType>();
    return static_cast<EnumType>(me.keyToValue(key, ok));
}

// Convert key to value for enumeration by QString key
template <class EnumType>
inline EnumType enumValueType(const QString& key, bool* ok = nullptr)
{
    return enumValueType<EnumType>(key.toLatin1().constData(), ok);
}

// Convert key to value for enumeration by QString key
template <class EnumType>
inline EnumType enumValueTypeStr(const QString& key, bool* ok = nullptr)
{
    return enumValueType<EnumType>(key.toLatin1().constData(), ok);
}

// Convert value for enumeration by QVariant (int - value, string - key
template <class EnumType>
inline EnumType enumValue(const QVariant& value, bool *ok)
{
    bool okInner;
    int v = value.toInt(&okInner);
    if (okInner)
    {
        const QMetaEnum me = metaEnum<EnumType>();
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
    return enumValueType<EnumType>(value.toString(), ok);
}

template <class EnumType>
inline EnumType enumValue(const QVariant& value, EnumType defaultValue)
{
    bool okInner;
    EnumType v = enumValue<EnumType>(value, &okInner);
    if (okInner)
        return v;
    return defaultValue;
}

template <class EnumType>
inline EnumType enumValue(const QVariant& value)
{
    return enumValue<EnumType>(value, nullptr);
}

#define MB_ENUM_DECL_EXPORT(type)                                           \
MB_EXPORT int enum##type##KeyCount();                                       \
MB_EXPORT QStringList enum##type##KeyList();                                \
MB_EXPORT QString enum##type##Key(type value);                              \
MB_EXPORT QString enum##type##KeyByIndex(int index);                        \
MB_EXPORT QString enum##type##Keys(int value);                              \
MB_EXPORT type enum##type##Value(const QString& key, bool* ok = nullptr);   \
MB_EXPORT type enum##type##Value(const QVariant& value, bool* ok = nullptr);\
MB_EXPORT type enum##type##Value(const QVariant& value, type defaultValue); \
MB_EXPORT type enum##type##ValueByIndex(int index);

enum StatusCode
{
    Status_Mb             = 0x80000000,
    Status_MbStopped      = Status_Mb | 1,
    Status_MbInitializing = Status_Mb | 2
    // next values is Modbus::StatusCode-s except Status_Processing
};

enum LogFlag
{
    Log_Error       = 0x00000001,
    Log_Warning     = 0x00000002,
    Log_Info        = 0x00000004,
    Log_TxRx        = 0x00000008,
    // ---- Qt Message Flags ----
    Log_QtFatal     = 0x08000000,
    Log_QtCritical  = 0x10000000,
    Log_QtWarning   = 0x20000000,
    Log_QtDebug     = 0x40000000,
    Log_QtInfo      = 0x80000000,
};
Q_DECLARE_FLAGS(LogFlags, LogFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(LogFlags)
Q_FLAG_NS(LogFlags)

enum DataType
{
    Bit,
    Int8,
    UInt8,
    Int16,
    UInt16,
    Int32,
    UInt32,
    Int64,
    UInt64,
    Float32,
    Double64
};
Q_ENUM_NS(DataType)
MB_ENUM_DECL_EXPORT(DataType)


/*enum AddressType
{
    X0 = 0,
    X1 = 1,
    X3 = 3,
    X4 = 4
};
Q_ENUM(AddressType)*/
//typedef Modbus::MemoryType AddressType;

struct Address
{
    Modbus::MemoryType type;
    quint16 offset;
};

enum DigitalFormat
{
    DefaultDigitalFormat = -1,
    Bin,
    Oct,
    Dec,
    UDec,
    Hex
};
Q_ENUM_NS(DigitalFormat)
MB_ENUM_DECL_EXPORT(DigitalFormat)

enum Format
{
    Bool,
    Bin16,
    Oct16,
    Dec16,
    UDec16,
    Hex16,
    Bin32,
    Oct32,
    Dec32,
    UDec32,
    Hex32,
    Bin64,
    Oct64,
    Dec64,
    UDec64,
    Hex64,
    Float,
    Double,
    ByteArray,
    String
};
Q_ENUM_NS(Format)
MB_ENUM_DECL_EXPORT(Format)

enum DataOrder
{
    DefaultOrder = -1,
    LessSignifiedFirst,
    MostSignifiedFirst
};
Q_ENUM_NS(DataOrder)
MB_ENUM_DECL_EXPORT(DataOrder)

enum StringLengthType
{
    DefaultStringLengthType = -1,
    ZerroEnded,
    FullLength
};
Q_ENUM_NS(StringLengthType)
MB_ENUM_DECL_EXPORT(StringLengthType)

enum StringEncoding
{
    DefaultStringEncoding = -1,
    Utf8,
    Utf16,
    Latin1
};
Q_ENUM_NS(StringEncoding)
MB_ENUM_DECL_EXPORT(StringEncoding)

struct MB_EXPORT Defaults
{
    const QString default_string_value;

    Defaults();
    static const Defaults &instance();
};

struct MB_EXPORT Strings
{
    const QString ReadCoils                 ;
    const QString ReadDiscreteInputs        ;
    const QString ReadHoldingRegisters      ;
    const QString ReadInputRegisters        ;
    const QString WriteSingleCoil           ;
    const QString WriteSingleRegister       ;
    const QString ReadExceptionStatus       ;
    const QString WriteMultipleCoils        ;
    const QString WriteMultipleRegisters    ;
    const QString MaskWriteRegister         ;
    const QString ReadWriteMultipleRegisters;

    Strings();
    static const Strings &instance();
};

template<typename T>
constexpr mb::DataType dataTypeFromT()
{
    return std::is_same_v<T, bool   > ? mb::Bit     :
           std::is_same_v<T, qint8  > ? mb::Int8    :
           std::is_same_v<T, quint8 > ? mb::UInt8   :
           std::is_same_v<T, qint16 > ? mb::Int16   :
           std::is_same_v<T, quint16> ? mb::UInt16  :
           std::is_same_v<T, qint32 > ? mb::Int32   :
           std::is_same_v<T, quint32> ? mb::UInt32  :
           std::is_same_v<T, qint64 > ? mb::Int64   :
           std::is_same_v<T, quint64> ? mb::UInt64  :
           std::is_same_v<T, float  > ? mb::Float32 :
           std::is_same_v<T, double > ? mb::Double64:
           static_cast<mb::DataType>(-1);
}
// size of data type in bytes
MB_EXPORT unsigned int sizeOfDataType(mb::DataType dataType);

// DataType to QVariant::Type converter function
MB_EXPORT QVariant::Type toQVariantType(mb::DataType dataType);

// size of format type value in bytes
MB_EXPORT size_t sizeofFormat(mb::Format format);

// size of format type value in regs
inline size_t sizeFormat(mb::Format format) { return sizeofFormat(format)/sizeof(short); }

// convert int representation of address to struct 'Address'
MB_EXPORT mb::Address toAddress(int address);

// convert struct 'Address' to int representation of address
MB_EXPORT int toInt(const mb::Address& address);

// convert string representation of address to struct 'Address'
MB_EXPORT mb::Address toAddress(const QString& address);

// convert struct 'Address' to string representation of address
MB_EXPORT QString toString(const mb::Address& address);

// convert string representation of format to enumeration 'Format'
inline mb::Format toFormat(const QString& format, bool *ok = nullptr) { return enumValueTypeStr<mb::Format>(format, ok); }

// convert enumeration 'Format' to string representation of format
inline QString toString(mb::Format format) { return enumKey<mb::Format>(format); }

MB_EXPORT QString resolveEscapeSequnces(const QString &src);

MB_EXPORT QString makeEscapeSequnces(const QString &src);

MB_EXPORT bool isDefaultStringValue(const QString &value);

// convert enum 'Modbus::StatusCode' to string representation
MB_EXPORT QString toString(Modbus::StatusCode status);

// convert enum 'StatusCode' to string representation
MB_EXPORT QString toString(mb::StatusCode status);

// return current timestamp
MB_EXPORT Timestamp_t currentTimestamp();

// convert integer timestamp to string representation
MB_EXPORT QString toString(mb::Timestamp_t timestamp);

// convert string representation to Modbus function number
MB_EXPORT uint8_t ModbusFunction(const QString &s);

// convert Modbus function number to string representation
MB_EXPORT QString ModbusFunctionString(uint8_t func);

MB_EXPORT QString toModbusMemoryTypeString(Modbus::MemoryType mem);

MB_EXPORT Modbus::MemoryType toModbusMemoryType(const QString &mem);

MB_EXPORT QString toUnitsString(const QList<quint8> units);

MB_EXPORT QList<quint8> toUnitsList(const QString &unitsStr, bool *ok = nullptr);

MB_EXPORT void changeByteOrder(char *data, int len);

inline void swapRegisters32(void *buff)
{
    reinterpret_cast<uint16_t*>(buff)[1] ^= reinterpret_cast<const uint16_t*>(buff)[0];
    reinterpret_cast<uint16_t*>(buff)[0] ^= reinterpret_cast<const uint16_t*>(buff)[1];
    reinterpret_cast<uint16_t*>(buff)[1] ^= reinterpret_cast<const uint16_t*>(buff)[0];
}


inline void swapRegisters64(void *buff)
{
    reinterpret_cast<uint16_t*>(buff)[3] ^= reinterpret_cast<const uint16_t*>(buff)[0];
    reinterpret_cast<uint16_t*>(buff)[0] ^= reinterpret_cast<const uint16_t*>(buff)[3];
    reinterpret_cast<uint16_t*>(buff)[3] ^= reinterpret_cast<const uint16_t*>(buff)[0];

    reinterpret_cast<uint16_t*>(buff)[1] ^= reinterpret_cast<const uint16_t*>(buff)[2];
    reinterpret_cast<uint16_t*>(buff)[2] ^= reinterpret_cast<const uint16_t*>(buff)[1];
    reinterpret_cast<uint16_t*>(buff)[1] ^= reinterpret_cast<const uint16_t*>(buff)[2];
}

MB_EXPORT QByteArray toByteArray(const QVariant &v,
                                 mb::Format format,
                                 Modbus::MemoryType memoryType,
                                 mb::DataOrder byteOrder,
                                 mb::DataOrder registerOrder,
                                 mb::DigitalFormat byteArrayFormat,
                                 mb::StringEncoding stringEncoding,
                                 mb::StringLengthType stringLengthType,
                                 const QString &byteArraySeparator,
                                 int variableLength);

MB_EXPORT QVariant toVariant(const QByteArray &v,
                             mb::Format format,
                             Modbus::MemoryType memoryType,
                             mb::DataOrder byteOrder,
                             mb::DataOrder registerOrder,
                             mb::DigitalFormat byteArrayFormat,
                             mb::StringEncoding stringEncoding,
                             mb::StringLengthType stringLengthType,
                             const QString &byteArraySeparator,
                             int variableLength);

MB_EXPORT QString escapeSequence(const QString &src);
MB_EXPORT QString fromEscapeSequence(const QString &esc);

template<class T>
QString toBinString(T value)
{
    int c = sizeof(value) * MB_BYTE_SZ_BITES;
    QString res(c, '0');
    while (value)
    {
        res[c-1] = '0' + static_cast<char>(value & 1);
        value >>= 1;
        c--;
    }
    return res;
}

template<class T>
QString toOctString(T value)
{
    int c = (sizeof(value) * MB_BYTE_SZ_BITES + 2) / 3;
    QString res(c, '0');
    while (value)
    {
        res[c-1] = '0' + static_cast<char>(value & 7);
        value >>= 3;
        c--;
    }
    return res;
}

template<class T>
QString toHexString(T value)
{
    int c = sizeof(value) * 2;
    QString res(c, '0');
    T v;
    while (value)
    {
        v = value & 0xF;
        if (v < 10)
            res[c-1] = '0' + static_cast<char>(v);
        else
            res[c-1] = 'A' - 10 + static_cast<char>(v);
        value >>= 4;
        c--;
    }
    return res;
}

Modbus::MemoryType memoryType(int index);

int memoryTypeIndex(Modbus::MemoryType type);

} // namespace mb

#endif // MBCORE_H
