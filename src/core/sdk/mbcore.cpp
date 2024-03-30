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

#include <QDateTime>

mb::Defaults::Defaults() :
    default_string_value("[default]")
{
}

const mb::Defaults &mb::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mb::Strings::Strings() :
    ReadCoils(QStringLiteral("ReadCoils")),
    ReadDiscreteInputs(QStringLiteral("ReadDiscreteInputs")),
    ReadHoldingRegisters(QStringLiteral("ReadHoldingRegisters")),
    ReadInputRegisters(QStringLiteral("ReadInputRegisters")),
    WriteSingleCoil(QStringLiteral("WriteSingleCoil")),
    WriteSingleRegister(QStringLiteral("WriteSingleRegister")),
    ReadExceptionStatus(QStringLiteral("ReadExceptionStatus")),
    WriteMultipleCoils(QStringLiteral("WriteMultipleCoils")),
    WriteMultipleRegisters(QStringLiteral("WriteMultipleRegisters"))
{
}

const mb::Strings &mb::Strings::instance()
{
    static const Strings d;
    return d;
}

unsigned int mb::sizeOfDataType(mb::DataType dataType)
{
    switch (dataType)
    {
    case mb::Int8:
    case mb::UInt8:
        return sizeof(qint8);
    case mb::Int16:
    case mb::UInt16:
        return sizeof(qint16);
    case mb::Int32:
    case mb::UInt32:
        return sizeof(qint32);
    case mb::Int64:
    case mb::UInt64:
        return sizeof(qint64);
    case mb::Float32:
        return sizeof(float);
    case mb::Double64:
        return sizeof(double);
    default:
        return 0;
    }
}

QVariant::Type mb::toQVariantType(mb::DataType dataType)
{
    switch (dataType)
    {
    case mb::Bit:
        return QVariant::Bool;
    case mb::Int8:
    case mb::Int16:
    case mb::Int32:
        return QVariant::Int;
    case mb::UInt8:
    case mb::UInt16:
    case mb::UInt32:
        return QVariant::UInt;
    case mb::Int64:
        return QVariant::LongLong;
    case mb::UInt64:
        return QVariant::ULongLong;
    case mb::Float32:
    case mb::Double64:
        return QVariant::Double;
    }
    return QVariant::Invalid;
}

size_t mb::sizeofFormat(mb::Format format)
{
    switch (format)
    {
    case mb::Dec16:
        return sizeof(qint16);
    case mb::Bin16:
    case mb::Oct16:
    case mb::UDec16:
    case mb::Hex16:
        return sizeof(quint16);
    case mb::Dec32:
        return sizeof(qint32);
    case mb::Bin32:
    case mb::Oct32:
    case mb::UDec32:
    case mb::Hex32:
        return sizeof(quint32);
    case mb::Dec64:
        return sizeof(qint64);
    case mb::Bin64:
    case mb::Oct64:
    case mb::UDec64:
    case mb::Hex64:
        return sizeof(quint64);
    case mb::Float:
        return sizeof(float);
    case mb::Double:
        return sizeof(double);
    case mb::Bool:
    case mb::ByteArray:
    case mb::String:
        break;
    }
    return 0;
}

#define DIVIDER 100000

mb::Address mb::toAddress(int address)
{
    int adrType = address / DIVIDER;
    int adrOffset = address % DIVIDER - 1;
    switch (adrType)
    {
    case Modbus::Memory_0x:
    case Modbus::Memory_1x:
    case Modbus::Memory_3x:
    case Modbus::Memory_4x:
        break;
    default:
        adrType = Modbus::Memory_4x;
        break;
    }
    if ((adrOffset < 0) || (adrOffset > 65535))
        adrOffset = 0;
    mb::Address adr;
    adr.type = static_cast<Modbus::MemoryType>(adrType);
    adr.offset = static_cast<quint16>(adrOffset);
    return adr;
}

int mb::toInt(const mb::Address &address)
{
    return address.type*DIVIDER+address.offset+1;
}

mb::Address mb::toAddress(const QString &address)
{
    return toAddress(address.toInt());
}

QString mb::toString(const mb::Address &address)
{
    return QString("%1%2").arg(address.type).arg(static_cast<int>(address.offset)+1, 5, 10, QLatin1Char('0'));
}

QString mb::resolveEscapeSequnces(const QString &src)
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

QString mb::makeEscapeSequnces(const QString &src)
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

bool mb::isDefaultStringValue(const QString &value)
{
    return value == Defaults::instance().default_string_value;
}

QString mb::toString(Modbus::StatusCode status)
{
    switch (status)
    {
    case Modbus::Status_Good                    : return QStringLiteral("Good");
    case Modbus::Status_BadIllegalFunction      : return QStringLiteral("BadIllegalFunction");
    case Modbus::Status_BadIllegalDataAddress   : return QStringLiteral("BadIllegalDataAddress");
    case Modbus::Status_BadIllegalDataValue     : return QStringLiteral("BadIllegalDataValue");
    case Modbus::Status_BadSlaveDeviceFailure   : return QStringLiteral("BadSlaveDeviceFailure");
    case Modbus::Status_BadAcknowledge          : return QStringLiteral("BadAcknowledge");
    case Modbus::Status_BadSlaveDeviceBusy      : return QStringLiteral("BadSlaveDeviceBusy");
    case Modbus::Status_BadNegativeAcknowledge  : return QStringLiteral("BadNegativeAcknowledge");
    case Modbus::Status_BadMemoryParityError    : return QStringLiteral("BadMemoryParityError");
    case Modbus::Status_BadEmptyResponse        : return QStringLiteral("BadEmptyResponse");
    case Modbus::Status_BadNotCorrectRequest    : return QStringLiteral("BadNotCorrectRequest");
    case Modbus::Status_BadNotCorrectResponse   : return QStringLiteral("BadNotCorrectResponse");
    case Modbus::Status_BadWriteBufferOverflow  : return QStringLiteral("BadWriteBufferOverflow");
    case Modbus::Status_BadReadBufferOverflow   : return QStringLiteral("BadReadBufferOverflow");
    case Modbus::Status_BadSerialOpen           : return QStringLiteral("BadSerialOpen");
    case Modbus::Status_BadSerialWrite          : return QStringLiteral("BadSerialWrite");
    case Modbus::Status_BadSerialRead           : return QStringLiteral("BadSerialRead");
    case Modbus::Status_BadAscMissColon         : return QStringLiteral("BadAscMissColon");
    case Modbus::Status_BadAscMissCrLf          : return QStringLiteral("BadAscMissCrLf");
    case Modbus::Status_BadAscChar              : return QStringLiteral("BadAscChar");
    case Modbus::Status_BadLrc                  : return QStringLiteral("BadLrc");
    case Modbus::Status_BadCrc                  : return QStringLiteral("BadCrc");
    case Modbus::Status_BadTcpCreate            : return QStringLiteral("BadTcpCreate");
    case Modbus::Status_BadTcpConnect           : return QStringLiteral("BadTcpConnect");
    case Modbus::Status_BadTcpDisconnect        : return QStringLiteral("BadTcpDisconnect");
    case Modbus::Status_BadTcpWrite             : return QStringLiteral("BadTcpWrite");
    case Modbus::Status_BadTcpRead              : return QStringLiteral("BadTcpRead");
    case Modbus::Status_Bad                     : return QStringLiteral("Bad(Unspecified)");
    default:
        return QString();
    }
}

QString mb::toString(StatusCode status)
{
    uint code = status;
    switch (code)
    {
    case mb::Status_MbStopped                   : return QStringLiteral("Stopped");
    case mb::Status_MbInitializing              : return QStringLiteral("Initializing");
    default:
        return toString(static_cast<Modbus::StatusCode>(code));
    }
}

mb::Timestamp_t mb::currentTimestamp()
{
    return QDateTime::currentMSecsSinceEpoch();
}

QString mb::toString(Timestamp_t timestamp)
{
    QDateTime dt = QDateTime::fromMSecsSinceEpoch(timestamp);
    return dt.toString(Qt::ISODateWithMs);
}


uint8_t mb::ModbusFunction(const QString &func)
{
    const Strings &s = Strings::instance();
    if (func == s.ReadCoils             ) return MBF_READ_COILS              ;
    if (func == s.ReadDiscreteInputs    ) return MBF_READ_DISCRETE_INPUTS    ;
    if (func == s.ReadHoldingRegisters  ) return MBF_READ_HOLDING_REGISTERS  ;
    if (func == s.ReadInputRegisters    ) return MBF_READ_INPUT_REGISTERS    ;
    if (func == s.WriteSingleCoil       ) return MBF_WRITE_SINGLE_COIL       ;
    if (func == s.WriteSingleRegister  ) return MBF_WRITE_SINGLE_REGISTER   ;
    if (func == s.ReadExceptionStatus   ) return MBF_READ_EXCEPTION_STATUS   ;
    if (func == s.WriteMultipleCoils    ) return MBF_WRITE_MULTIPLE_COILS    ;
    if (func == s.WriteMultipleRegisters) return MBF_WRITE_MULTIPLE_REGISTERS;
    return 0;
}

QString mb::ModbusFunctionString(uint8_t func)
{
    const Strings &s = Strings::instance();
    if (func == MBF_READ_COILS              ) return s.ReadCoils             ;
    if (func == MBF_READ_DISCRETE_INPUTS    ) return s.ReadDiscreteInputs    ;
    if (func == MBF_READ_HOLDING_REGISTERS  ) return s.ReadHoldingRegisters  ;
    if (func == MBF_READ_INPUT_REGISTERS    ) return s.ReadInputRegisters    ;
    if (func == MBF_WRITE_SINGLE_COIL       ) return s.WriteSingleCoil       ;
    if (func == MBF_WRITE_SINGLE_REGISTER   ) return s.WriteSingleRegister  ;
    if (func == MBF_READ_EXCEPTION_STATUS   ) return s.ReadExceptionStatus   ;
    if (func == MBF_WRITE_MULTIPLE_COILS    ) return s.WriteMultipleCoils    ;
    if (func == MBF_WRITE_MULTIPLE_REGISTERS) return s.WriteMultipleRegisters;
    return QString();
}

QString mb::toModbusMemoryTypeString(Modbus::MemoryType mem)
{
    switch (mem)
    {
    case Modbus::Memory_0x: return QStringLiteral("0x");
    case Modbus::Memory_1x: return QStringLiteral("1x");
    case Modbus::Memory_3x: return QStringLiteral("3x");
    case Modbus::Memory_4x: return QStringLiteral("4x");
    }
    return QString();
}

Modbus::MemoryType mb::toModbusMemoryType(const QString &mem)
{
    if (mem == QStringLiteral("0x")) return Modbus::Memory_0x;
    if (mem == QStringLiteral("1x")) return Modbus::Memory_1x;
    if (mem == QStringLiteral("3x")) return Modbus::Memory_3x;
    if (mem == QStringLiteral("4x")) return Modbus::Memory_4x;
    return Modbus::Memory_4x;
}


QString mb::toUnitsString(const QList<quint8> units)
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

QList<quint8> mb::toUnitsList(const QString &unitsStr, bool *ok)
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

void mb::changeByteOrder(char *data, int len)
{
    for (int i = 0; i < len/2; i++)
    {
        int n = 2*i, n1 = 2*i+1;
        char v = data[n];
        data[n] = data[n1];
        data[n1] = v;
    }
}

QByteArray mb::toByteArray(const QVariant &value, Format format, Modbus::MemoryType memoryType, DataOrder byteOrder, DataOrder registerOrder, DigitalFormat byteArrayFormat, StringEncoding stringEncoding, StringLengthType stringLengthType, const QString &byteArraySeparator, int variableLength)
{
    bool ok;
    char v[sizeof(qint64)];
    int sz = 0;
    QByteArray data;

    switch (format)
    {
    case mb::Bool:
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
    case mb::Bin16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 2));
        sz = sizeof(quint16);
        break;
    case mb::Oct16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 8));
        sz = sizeof(quint16);
        break;
    case mb::Dec16:
        *reinterpret_cast<qint16*>(v) = static_cast<qint16>(value.toInt());
        sz = sizeof(qint16);
        break;
    case mb::UDec16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toInt());
        sz = sizeof(quint16);
        break;
    case mb::Hex16:
        *reinterpret_cast<quint16*>(v) = static_cast<quint16>(value.toString().toUShort(&ok, 16));
        sz = sizeof(quint16);
        break;
    case mb::Bin32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 2));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case mb::Oct32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 8));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case mb::Dec32:
        *reinterpret_cast<qint32*>(v) = static_cast<qint32>(value.toInt());
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(qint32);
        break;
    case mb::UDec32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toUInt());
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case mb::Hex32:
        *reinterpret_cast<quint32*>(v) = static_cast<quint32>(value.toString().toULong(&ok, 16));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(quint32);
        break;
    case mb::Float:
        *reinterpret_cast<float*>(v) = value.toFloat();
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(v);
        sz = sizeof(float);
        break;
    case mb::Bin64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 2));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(quint64);
        break;
    case mb::Oct64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 8));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(quint64);
        break;
    case mb::Dec64:
        *reinterpret_cast<qint64*>(v) = static_cast<qint64>(value.toLongLong());
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(qint64);
        break;
    case mb::UDec64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toULongLong());
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(quint64);
        break;
    case mb::Hex64:
        *reinterpret_cast<quint64*>(v) = static_cast<quint64>(value.toString().toULongLong(&ok, 16));
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(quint64);
        break;
    case mb::Double:
        *reinterpret_cast<double*>(v) = value.toDouble();
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(v);
        sz = sizeof(double);
        break;
    case mb::ByteArray:
    {
        const QString sep = byteArraySeparator;
        QString s = value.toString();
        int i = 0;
        data = QByteArray(variableLength, '\0');
        QStringList bytes = s.split(sep);
        Q_FOREACH(QString byteStr, bytes)
        {
            byteStr = byteStr.trimmed();
            bool ok = false;
            switch (byteArrayFormat)
            {
            case mb::Bin:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 2));
                data[i] = static_cast<char>(v);
            }
            break;
            case mb::Oct:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 8));
                data[i] = static_cast<char>(v);
            }
            break;
            case mb::Dec:
            {
                quint8 v = static_cast<quint8>(byteStr.toInt(&ok, 10));
                data[i] = static_cast<char>(v);
            }
            break;
            case mb::UDec:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 10));
                data[i] = static_cast<char>(v);
            }
            break;
            case mb::Hex:
            default:
            {
                quint8 v = static_cast<quint8>(byteStr.toUInt(&ok, 16));
                data[i] = static_cast<char>(v);
            }
            break;
            }
            if (!ok)
                return QByteArray();
            i++;
        }
    }
    break;
    case mb::String:
    {
        QString s = value.toString();
        const int cBytes = variableLength*2;
        int lenChar = 1;
        switch (stringEncoding)
        {
        case mb::Utf16:
            lenChar = 2;
            data = QByteArray(reinterpret_cast<const char*>(s.utf16()), s.length()*2);
            break;
        case mb::Latin1:
            data = s.toLatin1();
            break;
        case mb::Utf8:
        default:
            data = s.toUtf8();
            break;
        }

        if (data.length() > cBytes)
            data = data.left(cBytes);
        else if ((stringLengthType == mb::ZerroEnded) && (data.length() < cBytes))
            data.append(lenChar, '\0'); // fill zerro-ended string with ending zerro(s)

        if ((data.length() & 1) && (data.length() < cBytes)) // data len is odd number
            data.append(1, '\0');
    }
    break;
    }

    switch (format)
    {
    case mb::ByteArray:
    case mb::String:
        break;
    default:
        data = QByteArray(v, sz);
        break;
    }

    if (data.length() > 0)
    {
        if (byteOrder == mb::MostSignifiedFirst)
            changeByteOrder(data.data(), data.length());
    }
    return data;
}

// TODO: byteOrder count
QVariant mb::toVariant(const QByteArray &data, Format format, Modbus::MemoryType memoryType, DataOrder byteOrder, DataOrder registerOrder, DigitalFormat byteArrayFormat, StringEncoding stringEncoding, StringLengthType stringLengthType, const QString &byteArraySeparator, int variableLength)
{
    QVariant value;
    const void *buff = data.constData();
    QByteArray newData;
    if (byteOrder == mb::MostSignifiedFirst)
    {
        newData = QByteArray(reinterpret_cast<const char*>(buff), data.length()); // TODO:
        mb::changeByteOrder(newData.data(), newData.length());
        buff = newData.constData();
    }
    switch (format)
    {
    case mb::Bool:
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
    case mb::Bin16:
        value = mb::toBinString(*reinterpret_cast<const quint16*>(buff));
        break;
    case mb::Oct16:
        value = mb::toOctString(*reinterpret_cast<const quint16*>(buff));
        break;
    case mb::Dec16:
        value = QVariant(*reinterpret_cast<const qint16*>(buff));
        break;
    case mb::UDec16:
        value = QVariant(*reinterpret_cast<const quint16*>(buff));
        break;
    case mb::Hex16:
        value = mb::toHexString(*reinterpret_cast<const quint16*>(buff));
        break;
    case mb::Bin32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = mb::toBinString(v);
    }
    break;
    case mb::Oct32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = mb::toOctString(v);
    }
    break;
    case mb::Dec32:
    {
        qint32 v = *reinterpret_cast<const qint32*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case mb::UDec32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case mb::Hex32:
    {
        quint32 v = *reinterpret_cast<const quint32*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = mb::toHexString(v);
    }
    break;
    case mb::Float:
    {
        float v = *reinterpret_cast<const float*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters32(&v);
        value = QVariant(v);
    }
    break;
    case mb::Bin64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = mb::toBinString(v);
    }
    break;
    case mb::Oct64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = mb::toOctString(v);
    }
    break;
    case mb::Dec64:
    {
        qint64 v = *reinterpret_cast<const qint64*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = QVariant(v);
    }
    break;
    case mb::UDec64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = QVariant(v);
    }
    break;
    case mb::Hex64:
    {
        quint64 v = *reinterpret_cast<const quint64*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = mb::toHexString(v);
    }
    break;
    case mb::Double:
    {
        double v = *reinterpret_cast<const double*>(buff);
        if (registerOrder == mb::MostSignifiedFirst)
            swapRegisters64(&v);
        value = QVariant(v);
    }
    break;
    case mb::ByteArray:
    {
        const QString sep = byteArraySeparator;
        QString s;
        switch (byteArrayFormat)
        {
        case mb::Bin:
            for (int i = 0; i < variableLength; i++)
                s += toBinString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        case mb::Oct:
            for (int i = 0; i < variableLength; i++)
                s += toOctString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        case mb::Dec:
            for (int i = 0; i < variableLength; i++)
                s += QString::number(static_cast<int>(reinterpret_cast<const char*>(buff)[i]), 10) + sep;
            break;
        case mb::UDec:
            for (int i = 0; i < variableLength; i++)
                s += QString::number(static_cast<unsigned int>(reinterpret_cast<const unsigned char*>(buff)[i]), 10) + sep;
            break;
        case mb::Hex:
        default:
            for (int i = 0; i < variableLength; i++)
                s += toHexString(reinterpret_cast<const unsigned char*>(buff)[i]) + sep;
            break;
        }
        value = s.left(s.length()-sep.length());
    }
    break;
    case mb::String:
    {
        QString s;
        switch (stringEncoding)
        {
        case mb::Utf16:
        {
            int cBytes = variableLength*2;
            QByteArray b(static_cast<const char*>(buff), cBytes);
            //quint16 BOM = (byteOrder == mb::MostSignifiedFirst) ? 0xFFFE : 0xFEFF;
            quint16 BOM = 0xFEFF;
            b.prepend(reinterpret_cast<const char*>(&BOM), sizeof(BOM));
            s = QString::fromUtf16(reinterpret_cast<const ushort*>(b.constData()), variableLength);
        }
        break;

        case mb::Latin1:
        {
            int cBytes = variableLength*2;
            s = QString::fromLatin1(static_cast<const char*>(buff), cBytes);
        }
        break;
        case mb::Utf8:
        default:
        {
            int cBytes = variableLength*2;
            s = QString::fromUtf8(static_cast<const char*>(buff), cBytes);
        }
        break;
        }

        if (stringLengthType == mb::ZerroEnded)
        {
            int i = s.indexOf(QChar(0));
            if ((i >= 0) && (i < s.length()))
                s = s.left(i);
        }
        value = s;
    }
    break;
    }
    return value;
}
