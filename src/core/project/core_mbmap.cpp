/*
    Modbus Tools

    Created: 2026
    Author: Afonso Nobrega, https://github.com/nobrega8

    Copyright (C) 2026  Afonso Nobrega

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
#include "core_mbmap.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

#include "core_dataview.h"

namespace {

// An integer written as a JSON number or as a string holding a decimal or
// 0x hex integer (SPEC.md 2.1)
bool toInteger(const QJsonValue &v, qint64 *out)
{
    if (v.isDouble())
    {
        double d = v.toDouble();
        if (d != static_cast<double>(static_cast<qint64>(d)))
            return false;
        *out = static_cast<qint64>(d);
        return true;
    }
    if (v.isString())
    {
        QString s = v.toString();
        bool ok = false;
        if (s.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive))
            *out = s.mid(2).toLongLong(&ok, 16);
        else
            *out = s.toLongLong(&ok, 10);
        return ok;
    }
    return false;
}

Modbus::MemoryType toMemoryType(const QString &area)
{
    if (area == QStringLiteral("coil"))
        return Modbus::Memory_0x;
    if (area == QStringLiteral("discrete"))
        return Modbus::Memory_1x;
    if (area == QStringLiteral("input"))
        return Modbus::Memory_3x;
    if (area == QStringLiteral("holding"))
        return Modbus::Memory_4x;
    return Modbus::Memory_Unknown;
}

// Format of a whole-register type; Bin16 for bits of a register
bool toFormat(const QString &type, mb::Format *format)
{
    static const QHash<QString, mb::Format> formats = {
        { QStringLiteral("int16")  , mb::Dec16  },
        { QStringLiteral("uint16") , mb::UDec16 },
        { QStringLiteral("int32")  , mb::Dec32  },
        { QStringLiteral("uint32") , mb::UDec32 },
        { QStringLiteral("int64")  , mb::Dec64  },
        { QStringLiteral("uint64") , mb::UDec64 },
        { QStringLiteral("float32"), mb::Float  },
        { QStringLiteral("float64"), mb::Double },
        { QStringLiteral("string") , mb::String }
    };
    auto it = formats.find(type);
    if (it == formats.end())
        return false;
    *format = it.value();
    return true;
}

// Name, unit and the scale to apply to the raw value, e.g. "Voltage [V] x0.1"
QString describe(const QJsonObject &r)
{
    QString text = r.value(QStringLiteral("name")).toString();
    QString unit = r.value(QStringLiteral("unit")).toString();
    if (!unit.isEmpty())
        text += QStringLiteral(" [%1]").arg(unit);
    double scale = r.value(QStringLiteral("scale")).toDouble(1);
    if (scale != 1)
        text += QStringLiteral(" x%1").arg(scale);
    double offset = r.value(QStringLiteral("offset")).toDouble(0);
    if (offset != 0)
        text += QStringLiteral(" %1%2").arg(offset > 0 ? QStringLiteral("+") : QString()).arg(offset);
    return text;
}

} // namespace

mbCoreMbMap::Strings::Strings() :
    suffix(QStringLiteral(".mbmap.json"))
{
}

const mbCoreMbMap::Strings &mbCoreMbMap::Strings::instance()
{
    static const Strings s;
    return s;
}

bool mbCoreMbMap::isMapFile(const QString &file)
{
    return file.endsWith(Strings::instance().suffix, Qt::CaseInsensitive);
}

QList<MBSETTINGS> mbCoreMbMap::parse(const QByteArray &data, QString *name, QString *error)
{
    const mbCoreDataViewItem::Strings &s = mbCoreDataViewItem::Strings::instance();
    QList<MBSETTINGS> items;

    QByteArray content = data;
    if (content.startsWith("\xEF\xBB\xBF"))
        content.remove(0, 3);

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(content, &parseError);
    if ((parseError.error != QJsonParseError::NoError) || !doc.isObject())
    {
        *error = QStringLiteral("Not a valid JSON file: %1").arg(parseError.errorString());
        return items;
    }
    QJsonObject root = doc.object();
    qint64 n = 0;
    if (!toInteger(root.value(QStringLiteral("mbmap")), &n))
    {
        *error = QStringLiteral("Not a .mbmap file: the \"mbmap\" version is missing");
        return items;
    }
    if (n != 2)
    {
        *error = QStringLiteral("Unsupported .mbmap version %1 (version 2 is supported)").arg(n);
        return items;
    }
    QString vendor = root.value(QStringLiteral("vendor")).toString();
    QString model = root.value(QStringLiteral("model")).toString();
    if (vendor.isEmpty() || model.isEmpty() || !root.value(QStringLiteral("registers")).isArray())
    {
        *error = QStringLiteral("The map has no \"vendor\", \"model\" or \"registers\"");
        return items;
    }
    *name = QStringLiteral("%1 %2").arg(vendor, model);

    qint64 base = 0;
    if (toInteger(root.value(QStringLiteral("address_base")), &n) && (n == 0 || n == 1))
        base = n;
    bool fileByteSwap = root.value(QStringLiteral("byte_order")).toString() == QStringLiteral("little");
    bool fileWordSwap = root.value(QStringLiteral("word_order")).toString() == QStringLiteral("little");

    // Items by Modbus address, so the bits of one register share one item
    QHash<int, int> bitItems;

    const QJsonArray registers = root.value(QStringLiteral("registers")).toArray();
    for (const QJsonValue &item : registers)
    {
        QJsonObject r = item.toObject();
        QString type = r.value(QStringLiteral("type")).toString();
        Modbus::MemoryType memoryType = toMemoryType(r.value(QStringLiteral("area")).toString());
        qint64 address = 0;
        if ((memoryType == Modbus::Memory_Unknown) ||
            r.value(QStringLiteral("name")).toString().isEmpty() ||
            !toInteger(r.value(QStringLiteral("address")), &address))
            continue;
        address -= base;
        if ((address < 0) || (address > 65535))
            continue;
        int adr = Modbus::Address(memoryType, static_cast<quint16>(address)).toInt();

        bool bitArea = (memoryType == Modbus::Memory_0x) || (memoryType == Modbus::Memory_1x);
        qint64 bit = -1;
        if (r.contains(QStringLiteral("bit")))
            toInteger(r.value(QStringLiteral("bit")), &bit);
        bool registerBits = !bitArea && (bit >= 0) && ((type == QStringLiteral("bool")) || (type == QStringLiteral("uint16")));

        if (bitArea)
        {
            if (type != QStringLiteral("bool"))
                continue;
            MBSETTINGS sets;
            sets[s.address] = adr;
            sets[s.format ] = mb::enumFormatKey(mb::Bool);
            sets[s.comment] = describe(r);
            items.append(sets);
            continue;
        }

        if (registerBits)
        {
            // One Bin16 item per register, its comment naming each bit or field
            qint64 bits = 1;
            if (type == QStringLiteral("uint16"))
                toInteger(r.value(QStringLiteral("bits")), &bits);
            QString part = (bits > 1) ? QStringLiteral("bits %1-%2: ").arg(bit).arg(bit + bits - 1)
                                      : QStringLiteral("bit %1: ").arg(bit);
            part += describe(r);
            auto it = bitItems.find(adr);
            if (it != bitItems.end())
            {
                MBSETTINGS &sets = items[it.value()];
                sets[s.comment] = sets.value(s.comment).toString() + QStringLiteral("; ") + part;
                continue;
            }
            MBSETTINGS sets;
            sets[s.address] = adr;
            sets[s.format ] = mb::enumFormatKey(mb::Bin16);
            sets[s.comment] = part;
            bitItems.insert(adr, items.count());
            items.append(sets);
            continue;
        }

        mb::Format format;
        if (!toFormat(type, &format))
            continue;
        bool byteSwap = r.contains(QStringLiteral("byte_order")) ?
                    r.value(QStringLiteral("byte_order")).toString() == QStringLiteral("little") : fileByteSwap;
        bool wordSwap = r.contains(QStringLiteral("word_order")) ?
                    r.value(QStringLiteral("word_order")).toString() == QStringLiteral("little") : fileWordSwap;

        MBSETTINGS sets;
        sets[s.address      ] = adr;
        sets[s.format       ] = mb::enumFormatKey(format);
        sets[s.comment      ] = describe(r);
        sets[s.swapBytes    ] = mb::enumSwapDataKey(byteSwap ? mb::SwapYes : mb::SwapNo);
        // R3R2R1R0 reverses the 2 registers of a 32-bit value and the 4 of a
        // 64-bit one, as "word_order": "little" does
        sets[s.registerOrder] = mb::enumRegisterOrderKey(wordSwap ? mb::R3R2R1R0 : mb::R0R1R2R3);
        if (format == mb::String)
        {
            if (!toInteger(r.value(QStringLiteral("length")), &n) || (n < 1))
                continue;
            sets[s.variableLength] = static_cast<int>(n * 2); // 2 characters per register
        }
        items.append(sets);
    }

    error->clear();
    return items;
}
