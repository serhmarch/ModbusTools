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
#include "mbcore.h"

#include <limits>

#include <QColor>
#include <QDateTime>
#include <QTextCodec>

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QRandomGenerator>
#endif

QString MBTOOLS_VERSION_QSTRING(MBTOOLS_VERSION_STR);

namespace mb {


int rand()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    return static_cast<int>(QRandomGenerator::global()->generate64() % (MBTOOLS_RAND_MAX + 1));
#else
    return qrand();
#endif
}


#define MB_ENUM_DEF(type)                                                   \
int enum##type##KeyCount()                                                  \
{                                                                           \
    return QMetaEnum::fromType<type>().keyCount();                          \
}                                                                           \
                                                                            \
QStringList enum##type##KeyList()                                           \
{                                                                           \
    return enumKeyList<type>();                                             \
}                                                                           \
                                                                            \
QString enum##type##Key(type value)                                         \
{                                                                           \
    return enumKey<type>(value);                                            \
}                                                                           \
                                                                            \
QString enum##type##KeyByIndex(int index)                                   \
{                                                                           \
    return enumKeyByIndex<type>(index);                                     \
}                                                                           \
                                                                            \
QString enum##type##Keys(int value)                                         \
{                                                                           \
    return enumKeys<type>(value);                                           \
}                                                                           \
                                                                            \
type enum##type##Value(const QString &key, bool *ok)                        \
{                                                                           \
    return enumValue<type>(key, ok);                                        \
}                                                                           \
                                                                            \
type enum##type##Value(const QVariant &value, bool *ok)                     \
{                                                                           \
    return enumValue<type>(value, ok);                                      \
}                                                                           \
                                                                            \
type enum##type##Value(const QVariant &value, type defaultValue)            \
{                                                                           \
    return enumValue<type>(value, defaultValue);                            \
}                                                                           \
                                                                            \
type enum##type##ValueByIndex(int index)                                    \
{                                                                           \
    return enumValueByIndex<type>(index);                                   \
}

MB_ENUM_DEF(DataType)
MB_ENUM_DEF(DigitalFormat)
MB_ENUM_DEF(Format)
MB_ENUM_DEF(SwapData)
MB_ENUM_DEF(RegisterOrder)
MB_ENUM_DEF(StringLengthType)

Defaults::Defaults() :
    default_string_value("[default]"),
    stringEncoding("UTF-8"),
    stringEncodingSpecial("DefaultStringEncoding")
{
}

const Defaults &Defaults::instance()
{
    static const Defaults d;
    return d;
}

Strings::Strings() :
    ReadCoils                              (QStringLiteral("ReadCoils")),
    ReadDiscreteInputs                     (QStringLiteral("ReadDiscreteInputs")),
    ReadHoldingRegisters                   (QStringLiteral("ReadHoldingRegisters")),
    ReadInputRegisters                     (QStringLiteral("ReadInputRegisters")),
    WriteSingleCoil                        (QStringLiteral("WriteSingleCoil")),
    WriteSingleRegister                    (QStringLiteral("WriteSingleRegister")),
    ReadExceptionStatus                    (QStringLiteral("ReadExceptionStatus")),
    Diagnostics                            (QStringLiteral("Diagnostics")),
    GetCommEventCounter                    (QStringLiteral("GetCommEventCounter")),
    GetCommEventLog                        (QStringLiteral("GetCommEventLog")),
    WriteMultipleCoils                     (QStringLiteral("WriteMultipleCoils")),
    WriteMultipleRegisters                 (QStringLiteral("WriteMultipleRegisters")),
    ReportServerID                         (QStringLiteral("ReportServerID")),
    ReadFileRecord                         (QStringLiteral("ReadFileRecord")),
    WriteFileRecord                        (QStringLiteral("WriteFileRecord")),
    MaskWriteRegister                      (QStringLiteral("MaskWriteRegister")),
    ReadWriteMultipleRegisters             (QStringLiteral("ReadWriteMultipleRegisters")),
    ReadFIFOQueue                          (QStringLiteral("ReadFIFOQueue")),
    DiagnReturnQueryData                   (QStringLiteral("ReturnQueryData")),
    DiagnRestartCommunicationsOption       (QStringLiteral("RestartCommunicationsOption")),
    DiagnReturnDiagnosticRegister          (QStringLiteral("ReturnDiagnosticRegister")),
    DiagnChangeAsciiInputDelimiter         (QStringLiteral("ChangeAsciiInputDelimiter")),
    DiagnForceListenOnlyMode               (QStringLiteral("ForceListenOnlyMode")),
    DiagnClearCountersAndDiagnosticRegister(QStringLiteral("ClearCountersAndDiagnosticRegister")),
    DiagnReturnBusMessageCount             (QStringLiteral("ReturnBusMessageCount")),
    DiagnReturnBusCommunicationErrorCount  (QStringLiteral("ReturnBusCommunicationErrorCount")),
    DiagnReturnBusExceptionErrorCount      (QStringLiteral("ReturnBusExceptionErrorCount")),
    DiagnReturnServerMessageCount          (QStringLiteral("ReturnServerMessageCount")),
    DiagnReturnServerNoResponseCount       (QStringLiteral("ReturnServerNoResponseCount")),
    DiagnReturnServerNAKCount              (QStringLiteral("ReturnServerNAKCount")),
    DiagnReturnServerBusyCount             (QStringLiteral("ReturnServerBusyCount")),
    DiagnReturnBusCharacterOverrunCount    (QStringLiteral("ReturnBusCharacterOverrunCount")),
    DiagnClearOverrunCounterAndFlag        (QStringLiteral("ClearOverrunCounterAndFlags")),
    Address_Default                        (QStringLiteral("Default")),
    Address_Modbus                         (QStringLiteral("Modbus")),
    Address_IEC61131                       (QStringLiteral("IEC61131")),
    Address_IEC61131Hex                    (QStringLiteral("IEC61131Hex")),
    IEC61131Prefix0x                       (QStringLiteral("%Q")),
    IEC61131Prefix1x                       (QStringLiteral("%I")),
    IEC61131Prefix3x                       (QStringLiteral("%IW")),
    IEC61131Prefix4x                       (QStringLiteral("%MW")),
    IEC61131SuffixHex                      ('h')
{
}

const Strings &Strings::instance()
{
    static const Strings d;
    return d;
}

BaseStatistics::BaseStatistics()
{
    sinceTimestamp      = mb::currentTimestamp();
    lastStatus          = Modbus::Status_Uncertain;
    lastTimestamp       = 0;
    lastSuccessTimestamp= 0;
    lastErrorStatus     = Modbus::Status_Uncertain;
    lastErrorTimestamp  = 0;
    //lastErrorText       = QString();
    countTx             = 0;
    countRx             = 0;
    countGood           = 0;
    countBad            = 0;
    countBadStandard    = 0;

    timeResponseLast  = 0;
    timeResponseMin   = 0;
    timeResponseMax   = 0;
    timeResponseAvg   = 0;
    timeResponseTotal = 0;
    timeResponseCount = 0;
}

void BaseStatistics::updateResponseTime(quint64 responseTime)
{
    timeResponseLast = responseTime;

    if (timeResponseCount == 0)
    {
        timeResponseMin = responseTime;
        timeResponseMax = responseTime;
    }
    else
    {
        if (responseTime < timeResponseMin)
            timeResponseMin = responseTime;
        if (responseTime > timeResponseMax)
            timeResponseMax = responseTime;
    }

    timeResponseTotal += responseTime;
    timeResponseCount++;
    timeResponseAvg = timeResponseTotal / timeResponseCount;
}

void processMinMax(DataType dataType, QVariant &min, QVariant &max)
{
#define CHECK_MIN_MAX(datatype, method, typeMin, typeMax)           \
        {                                                           \
            datatype minVal = min.method();                         \
            datatype maxVal = max.method();                         \
                                                                    \
            if (minVal >= maxVal)                                   \
            {                                                       \
                minVal = typeMin;                                   \
                maxVal = typeMax;                                   \
            }                                                       \
            else                                                    \
            {                                                       \
                if (minVal < typeMin || minVal >= typeMax)          \
                    minVal = typeMin;                               \
                                                                    \
                if (maxVal > typeMax || maxVal <= typeMin)          \
                    maxVal = typeMax;                               \
            }                                                       \
            min = minVal;                                           \
            max = maxVal;                                           \
        }

    switch (dataType)
    {
    case Bit:
        min = 0;
        max = 1;
        break;
    case Int8:
        CHECK_MIN_MAX(qint64, toLongLong, std::numeric_limits<qint8>::min(), std::numeric_limits<qint8>::max());
        break;
    case UInt8:
        CHECK_MIN_MAX(qint64, toLongLong, 0, std::numeric_limits<quint8>::max());
        break;
    case Int16:
        CHECK_MIN_MAX(qint64, toLongLong, std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
        break;
    case UInt16:
        CHECK_MIN_MAX(qint64, toLongLong, 0, std::numeric_limits<quint16>::max());
        break;
    case Int32:
        CHECK_MIN_MAX(qint64, toLongLong, std::numeric_limits<qint32>::min(), std::numeric_limits<qint32>::max());
        break;
    case UInt32:
        CHECK_MIN_MAX(qint64, toLongLong, 0, std::numeric_limits<quint32>::max());
        break;
    case Int64:
        CHECK_MIN_MAX(qint64, toLongLong, std::numeric_limits<qint64>::min(), std::numeric_limits<qint64>::max());
        break;
    case UInt64:
        CHECK_MIN_MAX(quint64, toULongLong, 0, std::numeric_limits<quint64>::max());
        break;
    case Float32:
        CHECK_MIN_MAX(double, toDouble, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max());
        break;
    case Double64:
        CHECK_MIN_MAX(double, toDouble, std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
        break;
    }
}

StringEncoding toStringEncoding(const QString &s)
{
    // Note: for backward compatibility
    if (s == QStringLiteral("Utf8"))
        return StringEncoding("UTF-8");
    if (s == QStringLiteral("Utf16"))
        return StringEncoding("UTF-16");
    if (s == QStringLiteral("Latin1"))
        return StringEncoding("latin1");
    return s.toLatin1();
}

QVariant toVariant(const IntColorMap &v)
{
    QVariantMap res;
    for (auto it = v.constBegin(); it != v.constEnd(); ++it)
    {
        res.insert(QString::number(it.key()), it.value());
    }
    return res;
}

IntColorMap toColorMap(const QVariant &v)
{
    IntColorMap colorMap;
    QVariantMap map = v.toMap();
    for (auto it = map.constBegin(); it != map.constEnd(); ++it)
    {
        bool ok;
        mb::LogFlag flag = static_cast<mb::LogFlag>(it.key().toInt(&ok));
        if (ok)
        {
            colorMap[flag] = it.value().value<QColor>();
        }
    }
    return colorMap;
}

unsigned int sizeOfDataType(DataType dataType)
{
    switch (dataType)
    {
    case Int8:
    case UInt8:
        return sizeof(qint8);
    case Int16:
    case UInt16:
        return sizeof(qint16);
    case Int32:
    case UInt32:
        return sizeof(qint32);
    case Int64:
    case UInt64:
        return sizeof(qint64);
    case Float32:
        return sizeof(float);
    case Double64:
        return sizeof(double);
    default:
        return 0;
    }
}

QVariant::Type toQVariantType(DataType dataType)
{
    switch (dataType)
    {
    case Bit:
        return QVariant::Bool;
    case Int8:
    case Int16:
    case Int32:
        return QVariant::Int;
    case UInt8:
    case UInt16:
    case UInt32:
        return QVariant::UInt;
    case Int64:
        return QVariant::LongLong;
    case UInt64:
        return QVariant::ULongLong;
    case Float32:
    case Double64:
        return QVariant::Double;
    }
    return QVariant::Invalid;
}

size_t sizeofFormat(Format format)
{
    switch (format)
    {
    case Dec16:
        return sizeof(qint16);
    case Bin16:
    case Oct16:
    case UDec16:
    case Hex16:
        return sizeof(quint16);
    case Dec32:
        return sizeof(qint32);
    case Bin32:
    case Oct32:
    case UDec32:
    case Hex32:
        return sizeof(quint32);
    case Dec64:
        return sizeof(qint64);
    case Bin64:
    case Oct64:
    case UDec64:
    case Hex64:
        return sizeof(quint64);
    case Float:
        return sizeof(float);
    case Double:
        return sizeof(double);
    case Bool:
    case ByteArray:
    case String:
        break;
    }
    return 0;
}

QString toString(const Address &address, AddressNotation notation)
{
    return address.toString<QString>(notation);
}

QString toFineString(AddressNotation notation)
{
    switch(notation)
    {
    case mb::Address::Notation_Default    : return QStringLiteral("Default");
    case mb::Address::Notation_Modbus     : return QStringLiteral("Modbus (1-based)");
    case mb::Address::Notation_IEC61131   : return QStringLiteral("IEC61131 (0-based)");
    case mb::Address::Notation_IEC61131Hex: return QStringLiteral("IEC61131 Hex (0-based)");
    }
    return QString();
}

QString toString(AddressNotation notation)
{
    const Strings &s = Strings::instance();
    switch(notation)
    {
    case mb::Address::Notation_Default    : return s.Address_Default    ;
    case mb::Address::Notation_Modbus     : return s.Address_Modbus     ;
    case mb::Address::Notation_IEC61131   : return s.Address_IEC61131   ;
    case mb::Address::Notation_IEC61131Hex: return s.Address_IEC61131Hex;
    default:
        return QString();
    }
}

AddressNotation toAddressNotation(const QString &address, bool *ok)
{
    const Strings &s = Strings::instance();
    if (address == s.Address_Modbus)
    {
        if (ok)
            *ok = true;
        return mb::Address::Notation_Modbus;
    }
    if (address == s.Address_IEC61131)
    {
        if (ok)
            *ok = true;
        return mb::Address::Notation_IEC61131;
    }
    if (address == s.Address_IEC61131Hex)
    {
        if (ok)
            *ok = true;
        return mb::Address::Notation_IEC61131Hex;
    }
    if (address == s.Address_Default)
    {
        if (ok)
            *ok = true;
        return mb::Address::Notation_Default;
    }
    if (ok)
        *ok = false;
    return mb::Address::Notation_Default;
}

AddressNotation toAddressNotation(const QVariant &notation, bool *ok)
{
    bool okInner;
    int n = notation.toInt(&okInner);
    if (okInner)
    {
        switch (n)
        {
        case mb::Address::Notation_Default:
        case mb::Address::Notation_Modbus:
        case mb::Address::Notation_IEC61131:
        case mb::Address::Notation_IEC61131Hex:
            if (ok)
                *ok = true;
            return static_cast<mb::AddressNotation>(n);
        default:
            if (ok)
                *ok = false;
            return mb::Address::Notation_Default;
        }
    }
    return toAddressNotation(notation.toString(), ok);
}

QString resolveEscapeSequnces(const QString &src)
{
    Defaults d = Defaults::instance();
    QString s = src;

    s.replace(QStringLiteral("\\s"), QStringLiteral(" "));
    s.replace(QStringLiteral("\\r"), QStringLiteral("\r"));
    s.replace(QStringLiteral("\\n"), QStringLiteral("\n"));
    s.replace(QStringLiteral("\\t"), QStringLiteral("\t"));
    s.replace(QStringLiteral("\\")+d.default_string_value, d.default_string_value);
    return s;
}

QString makeEscapeSequnces(const QString &src)
{
    Defaults d = Defaults::instance();
    QString s = src;

    s.replace(QStringLiteral(" ") , QStringLiteral("\\s"));
    s.replace(QStringLiteral("\r"), QStringLiteral("\\r"));
    s.replace(QStringLiteral("\n"), QStringLiteral("\\n"));
    s.replace(QStringLiteral("\t"), QStringLiteral("\\t"));
    s.replace(d.default_string_value, QStringLiteral("\\")+d.default_string_value);
    return s;
}

bool isDefaultStringValue(const QString &value)
{
    return value == Defaults::instance().default_string_value;
}

QString toString(Modbus::StatusCode status)
{
    switch (status)
    {
    case Modbus::Status_Good                  : return QStringLiteral("Good");
    case Modbus::Status_BadIllegalFunction    : return QStringLiteral("BadIllegalFunction");
    case Modbus::Status_BadIllegalDataAddress : return QStringLiteral("BadIllegalDataAddress");
    case Modbus::Status_BadIllegalDataValue   : return QStringLiteral("BadIllegalDataValue");
    case Modbus::Status_BadServerDeviceFailure: return QStringLiteral("BadSlaveDeviceFailure");
    case Modbus::Status_BadAcknowledge        : return QStringLiteral("BadAcknowledge");
    case Modbus::Status_BadServerDeviceBusy   : return QStringLiteral("BadSlaveDeviceBusy");
    case Modbus::Status_BadNegativeAcknowledge: return QStringLiteral("BadNegativeAcknowledge");
    case Modbus::Status_BadMemoryParityError  : return QStringLiteral("BadMemoryParityError");
    case Modbus::Status_BadEmptyResponse      : return QStringLiteral("BadEmptyResponse");
    case Modbus::Status_BadNotCorrectRequest  : return QStringLiteral("BadNotCorrectRequest");
    case Modbus::Status_BadNotCorrectResponse : return QStringLiteral("BadNotCorrectResponse");
    case Modbus::Status_BadWriteBufferOverflow: return QStringLiteral("BadWriteBufferOverflow");
    case Modbus::Status_BadReadBufferOverflow : return QStringLiteral("BadReadBufferOverflow");
    case Modbus::Status_BadSerialOpen         : return QStringLiteral("BadSerialOpen");
    case Modbus::Status_BadSerialWrite        : return QStringLiteral("BadSerialWrite");
    case Modbus::Status_BadSerialRead         : return QStringLiteral("BadSerialRead");
    case Modbus::Status_BadSerialReadTimeout  : return QStringLiteral("BadSerialReadTimeout");
    case Modbus::Status_BadSerialWriteTimeout : return QStringLiteral("BadSerialWriteTimeout");
    case Modbus::Status_BadAscMissColon       : return QStringLiteral("BadAscMissColon");
    case Modbus::Status_BadAscMissCrLf        : return QStringLiteral("BadAscMissCrLf");
    case Modbus::Status_BadAscChar            : return QStringLiteral("BadAscChar");
    case Modbus::Status_BadLrc                : return QStringLiteral("BadLrc");
    case Modbus::Status_BadCrc                : return QStringLiteral("BadCrc");
    case Modbus::Status_BadTcpCreate          : return QStringLiteral("BadTcpCreate");
    case Modbus::Status_BadTcpConnect         : return QStringLiteral("BadTcpConnect");
    case Modbus::Status_BadTcpWrite           : return QStringLiteral("BadTcpWrite");
    case Modbus::Status_BadTcpRead            : return QStringLiteral("BadTcpRead");
    case Modbus::Status_BadTcpBind            : return QStringLiteral("BadTcpBind");
    case Modbus::Status_BadTcpListen          : return QStringLiteral("BadTcpRead");
    case Modbus::Status_BadTcpAccept          : return QStringLiteral("BadTcpAccept");
    case Modbus::Status_BadTcpDisconnect      : return QStringLiteral("BadTcpDisconnect");
    case Modbus::Status_BadTcpReadTimeout     : return QStringLiteral("BadTcpReadTimeout");
    case Modbus::Status_BadUdpCreate          : return QStringLiteral("BadUdpCreate");
    case Modbus::Status_BadUdpWrite           : return QStringLiteral("BadUdpWrite");
    case Modbus::Status_BadUdpRead            : return QStringLiteral("BadUdpRead");
    case Modbus::Status_BadUdpBind            : return QStringLiteral("BadUdpBind");
    case Modbus::Status_BadUdpReadTimeout     : return QStringLiteral("BadUdpReadTimeout");
    case Modbus::Status_Bad                   : return QStringLiteral("Bad(Unspecified)");
    default:
        return QString();
    }
}

QString toString(StatusCode status)
{
    switch (status)
    {
    case Status_MbStopped     : return QStringLiteral("Stopped");
    case Status_MbInitializing: return QStringLiteral("Initializing");
    case Status_MbDisabled    : return QStringLiteral("Disabled");
    default:
        return Modbus::toString(static_cast<Modbus::StatusCode>(status));
    }
}

QString toString(LogFlag flag)
{
    switch (flag)
    {
    case Log_Error  : return QStringLiteral("Error");
    case Log_Warning: return QStringLiteral("Warn");
    case Log_Info   : return QStringLiteral("Info");
    case Log_Tx     : return QStringLiteral("Tx");
    case Log_Rx     : return QStringLiteral("Rx");
    case Log_Debug  : return QStringLiteral("Debug");
    default:
        return QString();
    }
}


Timestamp_t currentTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString toString(Timestamp_t timestamp)
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);
    return dt.toString(Qt::ISODateWithMs);
}


uint8_t ModbusFunction(const QString &func)
{
    const Strings &s = Strings::instance();
    if (func == s.ReadCoils                 ) return MBF_READ_COILS                   ;
    if (func == s.ReadDiscreteInputs        ) return MBF_READ_DISCRETE_INPUTS         ;
    if (func == s.ReadHoldingRegisters      ) return MBF_READ_HOLDING_REGISTERS       ;
    if (func == s.ReadInputRegisters        ) return MBF_READ_INPUT_REGISTERS         ;
    if (func == s.WriteSingleCoil           ) return MBF_WRITE_SINGLE_COIL            ;
    if (func == s.WriteSingleRegister       ) return MBF_WRITE_SINGLE_REGISTER        ;
    if (func == s.ReadExceptionStatus       ) return MBF_READ_EXCEPTION_STATUS        ;
    if (func == s.Diagnostics               ) return MBF_DIAGNOSTICS                  ;
    if (func == s.GetCommEventCounter       ) return MBF_GET_COMM_EVENT_COUNTER       ;
    if (func == s.GetCommEventLog           ) return MBF_GET_COMM_EVENT_LOG           ;
    if (func == s.WriteMultipleCoils        ) return MBF_WRITE_MULTIPLE_COILS         ;
    if (func == s.WriteMultipleRegisters    ) return MBF_WRITE_MULTIPLE_REGISTERS     ;
    if (func == s.ReportServerID            ) return MBF_REPORT_SERVER_ID             ;
    if (func == s.ReadFileRecord            ) return MBF_READ_FILE_RECORD             ;
    if (func == s.WriteFileRecord           ) return MBF_WRITE_FILE_RECORD            ;
    if (func == s.MaskWriteRegister         ) return MBF_MASK_WRITE_REGISTER          ;
    if (func == s.ReadWriteMultipleRegisters) return MBF_READ_WRITE_MULTIPLE_REGISTERS;
    if (func == s.ReadFIFOQueue             ) return MBF_READ_FIFO_QUEUE              ;
    return 0;
}

QString ModbusFunctionString(uint8_t func)
{
    const Strings &s = Strings::instance();
    switch(func)
    {
    case MBF_READ_COILS                   : return s.ReadCoils                 ;
    case MBF_READ_DISCRETE_INPUTS         : return s.ReadDiscreteInputs        ;
    case MBF_READ_HOLDING_REGISTERS       : return s.ReadHoldingRegisters      ;
    case MBF_READ_INPUT_REGISTERS         : return s.ReadInputRegisters        ;
    case MBF_WRITE_SINGLE_COIL            : return s.WriteSingleCoil           ;
    case MBF_WRITE_SINGLE_REGISTER        : return s.WriteSingleRegister       ;
    case MBF_READ_EXCEPTION_STATUS        : return s.ReadExceptionStatus       ;
    case MBF_DIAGNOSTICS                  : return s.Diagnostics               ;
    case MBF_GET_COMM_EVENT_COUNTER       : return s.GetCommEventCounter       ;
    case MBF_GET_COMM_EVENT_LOG           : return s.GetCommEventLog           ;
    case MBF_WRITE_MULTIPLE_COILS         : return s.WriteMultipleCoils        ;
    case MBF_WRITE_MULTIPLE_REGISTERS     : return s.WriteMultipleRegisters    ;
    case MBF_REPORT_SERVER_ID             : return s.ReportServerID            ;
    case MBF_READ_FILE_RECORD             : return s.ReadFileRecord            ;
    case MBF_WRITE_FILE_RECORD            : return s.WriteFileRecord           ;
    case MBF_MASK_WRITE_REGISTER          : return s.MaskWriteRegister         ;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS: return s.ReadWriteMultipleRegisters;
    case MBF_READ_FIFO_QUEUE              : return s.ReadFIFOQueue             ;
    default:
        return QString();
    }
}

uint16_t ModbusDiagnSubfunction(const QString &func)
{
    const Strings &s = Strings::instance();
    if (func == s.DiagnReturnQueryData                   ) return MBF_DIAGNOSTICS_RETURN_QUERY_DATA                     ;
    if (func == s.DiagnRestartCommunicationsOption       ) return MBF_DIAGNOSTICS_RESTART_COMMUNICATIONS_OPTION         ;
    if (func == s.DiagnReturnDiagnosticRegister          ) return MBF_DIAGNOSTICS_RETURN_DIAGNOSTIC_REGISTER            ;
    if (func == s.DiagnChangeAsciiInputDelimiter         ) return MBF_DIAGNOSTICS_CHANGE_ASCII_INPUT_DELIMITER          ;
    if (func == s.DiagnForceListenOnlyMode               ) return MBF_DIAGNOSTICS_FORCE_LISTEN_ONLY_MODE                ;
    if (func == s.DiagnClearCountersAndDiagnosticRegister) return MBF_DIAGNOSTICS_CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER;
    if (func == s.DiagnReturnBusMessageCount             ) return MBF_DIAGNOSTICS_RETURN_BUS_MESSAGE_COUNT              ;
    if (func == s.DiagnReturnBusCommunicationErrorCount  ) return MBF_DIAGNOSTICS_RETURN_BUS_COMMUNICATION_ERROR_COUNT  ;
    if (func == s.DiagnReturnBusExceptionErrorCount      ) return MBF_DIAGNOSTICS_RETURN_BUS_EXCEPTION_ERROR_COUNT      ;
    if (func == s.DiagnReturnServerMessageCount          ) return MBF_DIAGNOSTICS_RETURN_SERVER_MESSAGE_COUNT           ;
    if (func == s.DiagnReturnServerNoResponseCount       ) return MBF_DIAGNOSTICS_RETURN_SERVER_NO_RESPONSE_COUNT       ;
    if (func == s.DiagnReturnServerNAKCount              ) return MBF_DIAGNOSTICS_RETURN_SERVER_NAK_COUNT               ;
    if (func == s.DiagnReturnServerBusyCount             ) return MBF_DIAGNOSTICS_RETURN_SERVER_BUSY_COUNT              ;
    if (func == s.DiagnReturnBusCharacterOverrunCount    ) return MBF_DIAGNOSTICS_RETURN_BUS_CHARACTER_OVERRUN_COUNT    ;
    if (func == s.DiagnClearOverrunCounterAndFlag        ) return MBF_DIAGNOSTICS_CLEAR_OVERRUN_COUNTER_AND_FLAG        ;
    return 0;
}

QString ModbusDiagnSubfunctionString(uint16_t func)
{
    const Strings &s = Strings::instance();
    switch(func)
    {
    case MBF_DIAGNOSTICS_RETURN_QUERY_DATA                     : return s.DiagnReturnQueryData                   ;
    case MBF_DIAGNOSTICS_RESTART_COMMUNICATIONS_OPTION         : return s.DiagnRestartCommunicationsOption       ;
    case MBF_DIAGNOSTICS_RETURN_DIAGNOSTIC_REGISTER            : return s.DiagnReturnDiagnosticRegister          ;
    case MBF_DIAGNOSTICS_CHANGE_ASCII_INPUT_DELIMITER          : return s.DiagnChangeAsciiInputDelimiter         ;
    case MBF_DIAGNOSTICS_FORCE_LISTEN_ONLY_MODE                : return s.DiagnForceListenOnlyMode               ;
    case MBF_DIAGNOSTICS_CLEAR_COUNTERS_AND_DIAGNOSTIC_REGISTER: return s.DiagnClearCountersAndDiagnosticRegister;
    case MBF_DIAGNOSTICS_RETURN_BUS_MESSAGE_COUNT              : return s.DiagnReturnBusMessageCount             ;
    case MBF_DIAGNOSTICS_RETURN_BUS_COMMUNICATION_ERROR_COUNT  : return s.DiagnReturnBusCommunicationErrorCount  ;
    case MBF_DIAGNOSTICS_RETURN_BUS_EXCEPTION_ERROR_COUNT      : return s.DiagnReturnBusExceptionErrorCount      ;
    case MBF_DIAGNOSTICS_RETURN_SERVER_MESSAGE_COUNT           : return s.DiagnReturnServerMessageCount          ;
    case MBF_DIAGNOSTICS_RETURN_SERVER_NO_RESPONSE_COUNT       : return s.DiagnReturnServerNoResponseCount       ;
    case MBF_DIAGNOSTICS_RETURN_SERVER_NAK_COUNT               : return s.DiagnReturnServerNAKCount              ;
    case MBF_DIAGNOSTICS_RETURN_SERVER_BUSY_COUNT              : return s.DiagnReturnServerBusyCount             ;
    case MBF_DIAGNOSTICS_RETURN_BUS_CHARACTER_OVERRUN_COUNT    : return s.DiagnReturnBusCharacterOverrunCount    ;
    case MBF_DIAGNOSTICS_CLEAR_OVERRUN_COUNTER_AND_FLAG        : return s.DiagnClearOverrunCounterAndFlag        ;
    default:
        return QString();
    }
}

QString toModbusMemoryTypeString(Modbus::MemoryType mem, AddressNotation notation)
{
    switch(notation)
    {
    case mb::Address::Notation_IEC61131:
    case mb::Address::Notation_IEC61131Hex:
        switch (mem)
        {
        case Modbus::Memory_0x: return Strings::instance().IEC61131Prefix0x;
        case Modbus::Memory_1x: return Strings::instance().IEC61131Prefix1x;
        case Modbus::Memory_3x: return Strings::instance().IEC61131Prefix3x;
        case Modbus::Memory_4x: return Strings::instance().IEC61131Prefix4x;
        default: return QString();
        }
        break;
    case mb::Address::Notation_Modbus:
    default:
        switch (mem)
        {
        case Modbus::Memory_0x: return QStringLiteral("0x");
        case Modbus::Memory_1x: return QStringLiteral("1x");
        case Modbus::Memory_3x: return QStringLiteral("3x");
        case Modbus::Memory_4x: return QStringLiteral("4x");
        default: return QString();
        }
        break;
    }
}

Modbus::MemoryType toModbusMemoryType(const QString &mem)
{
    if (mem == QStringLiteral("0x")) return Modbus::Memory_0x;
    if (mem == QStringLiteral("1x")) return Modbus::Memory_1x;
    if (mem == QStringLiteral("3x")) return Modbus::Memory_3x;
    return Modbus::Memory_4x;
}


QString toUnitsString(const QList<quint8> units)
{
    QStringList ls;
    for (int i = 0; i < units.count(); i++)
    {
        if ((i < (units.count()-2)) &&
            (units[i]+1 == units[i+1]) &&
            (units[i]+2 == units[i+2]))
        {
            int b = i;
            i += 2;
            while ((i < (units.count()-1)) && (units[i]+1 == units[i+1]))
                i++;
            ls.append(QString("%1-%2").arg(units[b]).arg(units[i]));
        }
        else
            ls.append(QString("%1").arg(units[i]));
    }
    return ls.join(',');
}

QList<quint8> toUnitsList(const QString &unitsStr, bool *ok)
{
    // TODO: (maybe) make this function more tolerant for user input
    QList<quint8> lu;
    QStringList params = unitsStr.split(',');
    if (ok)
        *ok = false;
    Q_FOREACH(QString param, params)
    {
        bool ok;
        int v = param.toInt(&ok);
        if (ok)
            lu.append(static_cast<quint8>(v));
        else if (param.contains('-'))
        {
            QStringList range = param.split('-');
            if (range.count() != 2)
                return QList<quint8>();
            bool okBegin, okEnd;
            int begin = range.at(0).toInt(&okBegin);
            // TODO: replace [0, 255] to [UINT8_MIN_VALUE, UINT8_MAX_VALUE]
            if (!okBegin || (begin < 0) || (begin > 255))
                return QList<quint8>();
            int end = range.at(1).toInt(&okEnd);
            if (!okEnd || (end < 0) || (end > 255))
                return QList<quint8>();
            for (int i = begin; i <= end; i++)
                lu.append(static_cast<quint8>(i));
        }
        else
            return QList<quint8>();
    }
    if (ok)
        *ok = true;
    return lu;
}

void changeByteOrder(void *data, int len)
{
    char *d = reinterpret_cast<char*>(data);
    for (int i = 0; i < len/2; i++)
    {
        int n = 2*i, n1 = 2*i+1;
        char v = d[n];
        d[n] = d[n1];
        d[n1] = v;
    }
}

void swapRegisters32(void *buff)
{
    uint16_t t;
    t = reinterpret_cast<uint16_t*>(buff)[0];
    reinterpret_cast<uint16_t*>(buff)[0] = reinterpret_cast<const uint16_t*>(buff)[1];
    reinterpret_cast<uint16_t*>(buff)[1] = t;
}

void swapRegisters64(void *buff, RegisterOrder order)
{
    switch (order)
    {
    case R3R2R1R0:
    {
        uint16_t t;
        uint16_t *r = reinterpret_cast<uint16_t*>(buff);
        t = r[0]; r[0] = r[3]; r[3] = t;
        t = r[1]; r[1] = r[2]; r[2] = t;
    }
        break;
    case R2R3R0R1:
    {
        uint16_t t;
        uint16_t *r = reinterpret_cast<uint16_t*>(buff);
        t = r[0]; r[0] = r[2]; r[2] = t;
        t = r[1]; r[1] = r[3]; r[3] = t;
    }
        break;
    case R1R0R3R2:
    {
        uint16_t t;
        uint16_t *r = reinterpret_cast<uint16_t*>(buff);
        t = r[0]; r[0] = r[1]; r[1] = t;
        t = r[2]; r[2] = r[3]; r[3] = t;
    }
        break;
    default:
        break;
    }
}

QByteArray toByteArray(const QVariant &value, Format format, Modbus::MemoryType memoryType, SwapData swapBytes, RegisterOrder registerOrder, DigitalFormat byteArrayFormat, const StringEncoding &stringEncoding, StringLengthType stringLengthType, const QString &byteArraySeparator, int variableLength)
{
    bool ok;
    char v[sizeof(qint64)];
    int sz = 0;
    QByteArray data;

    switch (format)
    {
    case Bool:
        *v = value.toBool();
        switch (memoryType)
        {
        case Modbus::Memory_3x:
        case Modbus::Memory_4x:
            sz = sizeof(quint16);
            break;
        default:
            sz = sizeof(bool);
            break;
        }
        break;
    case Bin16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 2));
        sz = sizeof(quint16);
        break;
    case Oct16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 8));
        sz = sizeof(quint16);
        break;
    case Dec16:
        *reinterpret_cast<qint16*>(v) = static_cast<qint16>(value.toInt());
        sz = sizeof(qint16);
        break;
    case UDec16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toInt());
        sz = sizeof(quint16);
        break;
    case Hex16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 16));
        sz = sizeof(quint16);
        break;
    case Bin32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 2));
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case Oct32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 8));
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case Dec32:
        *reinterpret_cast<qint32*>(v) = static_cast<qint32>(value.toInt());
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(qint32);
        break;
    case UDec32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toUInt());
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case Hex32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 16));
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case Float:
        *reinterpret_cast<float*>(v) = value.toFloat();
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(v);
        sz = sizeof(float);
        break;
    case Bin64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 2));
        swapRegisters64(v, registerOrder);
        sz = sizeof(quint64);
        break;
    case Oct64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 8));
        swapRegisters64(v, registerOrder);
        sz = sizeof(quint64);
        break;
    case Dec64:
        *reinterpret_cast<qint64*>(v) = static_cast<qint64>(value.toLongLong());
        swapRegisters64(v, registerOrder);
        sz = sizeof(qint64);
        break;
    case UDec64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toULongLong());
        swapRegisters64(v, registerOrder);
        sz = sizeof(quint64);
        break;
    case Hex64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 16));
        swapRegisters64(v, registerOrder);
        sz = sizeof(quint64);
        break;
    case Double:
        *reinterpret_cast<double*>(v) = value.toDouble();
        swapRegisters64(v, registerOrder);
        sz = sizeof(double);
        break;
    case ByteArray:
    {
        const QString sep = byteArraySeparator;
        QString s = value.toString();
        int i = 0;
        QStringList bytes = s.split(sep);
        int cBytes;
        if (variableLength > 0)
        {
            if (variableLength < bytes.length())
                cBytes = variableLength;
            else
                cBytes = bytes.length();
            data = QByteArray(variableLength, '\0');
        }
        else
        {
            cBytes = bytes.length();
            data = QByteArray(cBytes, '\0');
        }
        for (int i = 0; i < cBytes; ++i)
        {
            QString byteStr = bytes[i].trimmed();
            bool ok = false;
            switch (byteArrayFormat)
            {
            case Bin:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 2));
                data[i] = static_cast<char>(v);
            }
                break;
            case Oct:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 8));
                data[i] = static_cast<char>(v);
            }
                break;
            case Dec:
            {
                quint8 v = static_cast<quint8>(byteStr.toInt(&ok, 10));
                data[i] = static_cast<char>(v);
            }
                break;
            case UDec:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 10));
                data[i] = static_cast<char>(v);
            }
                break;
            case Hex:
            default:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 16));
                data[i] = static_cast<char>(v);
            }
                break;
            }
            if (!ok)
                return QByteArray();
        }
    }
        break;
    case String:
    {
        QTextCodec *codec = QTextCodec::codecForName(stringEncoding);
        QString s = fromEscapeSequence(value.toString());
        if (stringLengthType == ZeroEnded)
        {
            int index = s.indexOf(QChar('\0'));
            if (index < 0)
                s.append(QChar('\0'));
            else
                s = s.left(index+1);
        }
        QTextCodec::ConverterState state;
        state.flags = QTextCodec::IgnoreHeader;
        data = codec->fromUnicode(s.constData(), s.size(), &state);
        if (variableLength > 0)
        {
            const int cBytes = variableLength;
            if ((stringLengthType == FullLength) && data.length() < cBytes)
                data.append(cBytes-data.length(), '\0');
            else if (data.length() > cBytes)
                data = data.left(cBytes);
        }
        if ((data.length() & 1) && ((memoryType == Modbus::Memory_3x) || (memoryType == Modbus::Memory_4x)))// data len is odd number
            data.append(1, '\0');
    }
        break;
    }

    switch (format)
    {
    case ByteArray:
    case String:
        break;
    default:
        data = QByteArray(v, sz);
        break;
    }

    if (data.length() > 0)
    {
        if (swapBytes == SwapYes)
            changeByteOrder(data.data(), data.length());
    }
    return data;
}

// TODO: swapBytes count
QVariant toVariant(const QByteArray &data, Format format, Modbus::MemoryType memoryType, SwapData swapBytes, RegisterOrder registerOrder, DigitalFormat byteArrayFormat, const StringEncoding &stringEncoding, StringLengthType stringLengthType, const QString &byteArraySeparator, int variableLength)
{
    QVariant value;
    const void *buff = data.constData();
    QByteArray newData = data;
    if (swapBytes == SwapYes)
    {
        newData = QByteArray(reinterpret_cast<const char*>(buff), data.length()); // TODO:
        changeByteOrder(newData.data(), newData.length());
        buff = newData.constData();
    }
    switch (format)
    {
    case Bool:
        switch (memoryType)
        {
        case Modbus::Memory_3x:
        case Modbus::Memory_4x:
            value = static_cast<int>(*reinterpret_cast<const quint16*>(buff) != 0);
            break;
        default:
            value = *reinterpret_cast<const quint8*>(buff) & 0x1;
            break;
        }
        break;
    case Bin16:
        value = toBinString(*reinterpret_cast<const quint16*>(buff));
        break;
    case Oct16:
        value = toOctString(*reinterpret_cast<const quint16*>(buff));
        break;
    case Dec16:
        value = QVariant(*reinterpret_cast<const qint16*>(buff));
        break;
    case UDec16:
        value = QVariant(*reinterpret_cast<const quint16*>(buff));
        break;
    case Hex16:
        value = toHexString(*reinterpret_cast<const quint16*>(buff));
        break;
    case Bin32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = toBinString(v);
    }
    break;
    case Oct32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = toOctString(v);
    }
    break;
    case Dec32:
    {
        qint32 v = *reinterpret_cast<const qint32*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case UDec32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case Hex32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = toHexString(v);
    }
    break;
    case Float:
    {
        float v = *reinterpret_cast<const float*>(buff);
        if (toSwapData(registerOrder) == SwapYes)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case Bin64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        swapRegisters64(&v, registerOrder);
        value = toBinString(v);
    }
    break;
    case Oct64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        swapRegisters64(&v, registerOrder);
        value = toOctString(v);
    }
    break;
    case Dec64:
    {
        qint64 v = *reinterpret_cast<const qint64*>(buff);
        swapRegisters64(&v, registerOrder);
        value = QVariant(v);
    }
    break;
    case UDec64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        swapRegisters64(&v, registerOrder);
        value = QVariant(v);
    }
    break;
    case Hex64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        swapRegisters64(&v, registerOrder);
        value = toHexString(v);
    }
    break;
    case Double:
    {
        double v = *reinterpret_cast<const double*>(buff);
        swapRegisters64(&v, registerOrder);
        value = QVariant(v);
    }
    break;
    case ByteArray:
    {
        const QString sep = byteArraySeparator;
        QString s;
        int cBytes;
        if ((variableLength <= 0) || (variableLength > data.length()))
            cBytes = data.length();
        else
            cBytes = variableLength;
        switch (byteArrayFormat)
        {
        case Bin:
            for (int i = 0; i < cBytes; i++)
                s += toBinString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        case Oct:
            for (int i = 0; i < cBytes; i++)
                s += toOctString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        case Dec:
            for (int i = 0; i < cBytes; i++)
                s += QString::number(static_cast<int>(reinterpret_cast<const char*>(buff)[i]), 10) + sep;
            break;
        case UDec:
            for (int i = 0; i < cBytes; i++)
                s += QString::number(static_cast<unsigned int>(reinterpret_cast<const unsigned char*>(buff)[i]), 10) + sep;
            break;
        case Hex:
        default:
            for (int i = 0; i < cBytes; i++)
                s += toHexString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        }
        value = s.left(s.length()-sep.length());
    }
        break;
    case String:
    {
        QTextCodec *codec = QTextCodec::codecForName(stringEncoding);
        QTextCodec::ConverterState state;
        state.flags = QTextCodec::IgnoreHeader;
        QString s = codec->toUnicode(newData.constData(), newData.size(), &state);
        if (stringLengthType == ZeroEnded)
        {
            int i = s.indexOf(QChar(0));
            if ((i >= 0) && (i < s.length()))
                s = s.left(i);
        }
        value = escapeSequence(s);
    }
    break;
    }
    return value;
}

QString escapeSequence(const QString &src)
{
    QString result;
    for (const QChar &ch : src)
    {
        if (ch == '\\')
        {
            result += "\\\\";
        }
        //else if (ch.isPrint() && ch < 128)
        else if (ch.isPrint())
        {
            result += ch;
        }
        else
        {
            switch (ch.unicode())
            {
            case '\0': result += "\\0"; break;
            case '\a': result += "\\a"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            case '\v': result += "\\v"; break;
            default:
                if (ch.unicode() < 256)
                    result += QString("\\x%1").arg(ch.unicode(), 2, 16, QLatin1Char('0'));
                else
                    result += QString("\\u%1").arg(ch.unicode(), 4, 16, QLatin1Char('0'));
                break;
            }
        }
    }
    return result;
}

QString fromEscapeSequence(const QString &esc)
{
    QString result;

    for (int i = 0; i < esc.length(); )
    {
        if (esc[i] == '\\' && i + 1 < esc.length())
        {
            switch (esc[i + 1].unicode())
            {
            case '0': result += '\0'; i += 2; break;
            case 'a': result += '\a'; i += 2; break;
            case 'b': result += '\b'; i += 2; break;
            case 'f': result += '\f'; i += 2; break;
            case 'n': result += '\n'; i += 2; break;
            case 'r': result += '\r'; i += 2; break;
            case 't': result += '\t'; i += 2; break;
            case 'v': result += '\v'; i += 2; break;
            case '\\': result += '\\'; i += 2; break;
            case 'x':
                if (i + 3 < esc.length())
                {
                    bool ok;
                    int hexValue = esc.midRef(i + 2, 2).toInt(&ok, 16);
                    if (ok)
                    {
                        result += QChar(hexValue);
                        i += 4;
                    }
                    else
                    {
                        result += esc[i];
                        i++;
                    }
                }
                else
                {
                    result += esc[i];
                    i++;
                }
                break;
            case 'u':
                if (i + 5 < esc.length())
                {
                    bool ok;
                    int hexValue = esc.midRef(i + 2, 4).toInt(&ok, 16);
                    if (ok)
                    {
                        result += QChar(hexValue);
                        i += 6;
                    }
                    else
                    {
                        result += esc[i];
                        i++;
                    }
                }
                else
                {
                    result += esc[i];
                    i++;
                }
                break;
            default:
                result += esc[i + 1];
                i += 2;
                break;
            }
        }
        else
        {
            result += esc[i];
            i++;
        }
    }
    return result;
}

Modbus::MemoryType memoryType(int index)
{
    switch(index)
    {
    case 0: return Modbus::Memory_0x;
    case 1: return Modbus::Memory_1x;
    case 2: return Modbus::Memory_3x;
    case 3: return Modbus::Memory_4x;
    default:return Modbus::Memory_Unknown;
    }
}

int memoryTypeIndex(Modbus::MemoryType type)
{
    switch(type)
    {
    case Modbus::Memory_0x: return 0;
    case Modbus::Memory_1x: return 1;
    case Modbus::Memory_3x: return 2;
    case Modbus::Memory_4x: return 3;
    default:return -1;
    }
}

void unite(MBSETTINGS &s1, const MBSETTINGS &s2)
{
    for (auto it = s2.constBegin(); it != s2.constEnd(); ++it)
        s1.insert(it.key(), it.value());
}

QString currentUser()
{
#ifdef Q_OS_WINDOWS
    return QString::fromLocal8Bit(qgetenv("USERNAME"));
#else
    return QString::fromLocal8Bit(qgetenv("USER"));
#endif
}

Format toFormat(DigitalFormat digitalFormat)
{
    switch (digitalFormat)
    {
    case Bin:
        return Bin16;
    case Oct:
        return Oct16;
    case Hex:
        return Hex16;
    case UDec:
        return UDec16;
    case Dec:
    default:
        return Dec16;
    }
}

RegisterOrder toRegisterOrder(const QString &s, bool *ok)
{
    bool okInner;
    RegisterOrder ro = enumRegisterOrderValue(s, &okInner);
    if (!okInner)
    {
        if (s == QStringLiteral("DefaultSwapData"))
        {
            ro = DefaultRegisterOrder;
            okInner = true;
        }
        else if (s == QStringLiteral("SwapNo"))
        {
            ro = R0R1R2R3;
            okInner = true;
        }
        else if (s == QStringLiteral("SwapYes"))
        {
            ro = R3R2R1R0;
            okInner = true;
        }
    }
    if (ok)
        *ok = okInner;
    return ro;
}

RegisterOrder toRegisterOrder(const QVariant &v, bool *ok)
{
    if (v.type() == QVariant::String)
        return toRegisterOrder(v.toString(), ok);
    return enumRegisterOrderValue(v, ok);
}

RegisterOrder toRegisterOrder(const QVariant &v, RegisterOrder defaultValue)
{
    bool ok;
    RegisterOrder r = toRegisterOrder(v, &ok);
    if (ok)
        return r;
    return defaultValue;
}

} // namespace mb
