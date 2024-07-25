#include "ModbusQt.h"

#include "ModbusTcpPort.h"
#include "ModbusSerialPort.h"
#include "ModbusClientPort.h"
#include "ModbusServerPort.h"
#include "ModbusServerResource.h"
#include "ModbusTcpServer.h"

namespace Modbus {

Strings::Strings() :
    unit            (QStringLiteral("unit")),
    type            (QStringLiteral("type")),
    host            (QStringLiteral("host")),
    port            (QStringLiteral("port")),
    timeout         (QStringLiteral("timeout")),
    serialPortName  (QStringLiteral("serialPortName")),
    baudRate        (QStringLiteral("baudRate")),
    dataBits        (QStringLiteral("dataBits")),
    parity          (QStringLiteral("parity")),
    stopBits        (QStringLiteral("stopBits")),
    flowControl     (QStringLiteral("flowControl")),
    timeoutFirstByte(QStringLiteral("timeoutFirstByte")),
    timeoutInterByte(QStringLiteral("timeoutInterByte"))
{
}

const Strings &Strings::instance()
{
    static Strings s;
    return s;
}

Defaults::Defaults() :
    unit            (1),
    type            (TCP),
    host            (ModbusTcpPort   ::Defaults::instance().host            ),
    port            (ModbusTcpPort   ::Defaults::instance().port            ),
    timeout         (ModbusTcpPort   ::Defaults::instance().timeout         ),
    serialPortName  (ModbusSerialPort::Defaults::instance().portName        ),
    baudRate        (ModbusSerialPort::Defaults::instance().baudRate        ),
    dataBits        (ModbusSerialPort::Defaults::instance().dataBits        ),
    parity          (ModbusSerialPort::Defaults::instance().parity          ),
    stopBits        (ModbusSerialPort::Defaults::instance().stopBits        ),
    flowControl     (ModbusSerialPort::Defaults::instance().flowControl     ),
    timeoutFirstByte(ModbusSerialPort::Defaults::instance().timeoutFirstByte),
    timeoutInterByte(ModbusSerialPort::Defaults::instance().timeoutInterByte)
{
}

const Defaults &Defaults::instance()
{
    static Defaults s;
    return s;
}

Address::Address()
{
    m_type = Memory_Unknown;
    m_offset = 0;
}

Address::Address(MemoryType type, quint16 offset) :
    m_type(type),
    m_offset(offset)
{
}

Address::Address(quint32 adr)
{
    this->operator=(adr);
}

QString Address::toString() const
{
    if (isValid())
        return QString("%1%2").arg(m_type).arg(number(), 5, 10, QChar('0'));
    else
        return QString();
}

Address &Address::operator=(quint32 v)
{
    quint32 number = v % 100000;
    if ((number < 1) || (number > 65536))
    {
        m_type = Memory_Unknown;
        m_offset = 0;
        return *this;
    }
    quint16 type = static_cast<quint16>(v/100000);
    switch(type)
    {
    case Memory_0x:
    case Memory_1x:
    case Memory_3x:
    case Memory_4x:
        m_type = type;
        m_offset = static_cast<quint16>(number-1);
        break;
    default:
        m_type = Memory_Unknown;
        m_offset = 0;
        break;
    }
    return *this;
}

ProtocolType toProtocolType(const QString &v, bool *ok)
{
    return enumValue<ProtocolType>(v, ok);
}

ProtocolType toProtocolType(const QVariant &v, bool *ok)
{
    return enumValue<ProtocolType>(v, ok);
}

int8_t toDataBits(const QString &s, bool *ok)
{
    bool okInner;
    int8_t r = static_cast<int8_t>(s.toInt(&okInner));
    if (!okInner)
    {
        okInner = true;
        if      (s == QStringLiteral("Data8"))
            r = 8;
        else if (s == QStringLiteral("Data7"))
            r = 7;
        else if (s == QStringLiteral("Data6"))
            r = 6;
        else if (s == QStringLiteral("Data5"))
            r = 5;
        else
            okInner = false;
    }
    if (ok)
        *ok = okInner;
    return r;
}

int8_t toDataBits(const QVariant &v, bool *ok)
{
    bool okInner;
    int8_t r = static_cast<int8_t>(v.toInt(&okInner));
    if (!okInner)
        r = toDataBits(v.toString(), &okInner);
    if (ok)
        *ok = okInner;
    return r;
}

Parity toParity(const QString &v, bool *ok)
{
    return enumValue<Parity>(v, ok);
}

Parity toParity(const QVariant &v, bool *ok)
{
    return enumValue<Parity>(v, ok);
}

StopBits toStopBits(const QString &v, bool *ok)
{
    return enumValue<StopBits>(v, ok);
}

StopBits toStopBits(const QVariant &v, bool *ok)
{
    return enumValue<StopBits>(v, ok);
}

FlowControl toFlowControl(const QString &v, bool *ok)
{
    return enumValue<FlowControl>(v, ok);
}

FlowControl toFlowControl(const QVariant &v, bool *ok)
{
    return enumValue<FlowControl>(v, ok);
}

QString toString(StatusCode v)
{
    static const int index = QString("Status_").size();

    QString s = enumKey(v);
    if (s.size())
        return s.mid(index);
    return s;
}

QString toString(ProtocolType v)
{
    return enumKey(v);
}

QString toString(Parity v)
{
    return enumKey(v);
}

QString toString(StopBits v)
{
    return enumKey(v);
}

QString toString(FlowControl v)
{
    return enumKey(v);
}

QStringList availableSerialPortList()
{
    List<String> ports = availableSerialPorts();
    QStringList portList;
    for (const String &s : ports)
        portList.append(QString(s.data()));
    return portList;
}

ModbusPort *createPort(const Settings &settings, bool blocking)
{
    const Strings &s = Strings::instance();
    const Settings::const_iterator it = settings.constFind(s.type);
    if (it != settings.constEnd())
    {
        bool ok;
        ProtocolType type = toProtocolType(it.value(), &ok);
        if (ok)
        {
            switch (type)
            {
            case Modbus::TCP:
            {
                const ModbusTcpPort::Defaults &d = ModbusTcpPort::Defaults::instance();
                QByteArray host = settings.value(s.host, d.host).toString().toLatin1();
                Modbus::TcpSettings tc;
                tc.host = host.data();
                tc.port = settings.value(s.port, d.port).toUInt();
                tc.timeout = settings.value(s.timeout, d.timeout).toUInt();
                return Modbus::createPort(type, &tc, false);
            }
            break;
            case Modbus::RTU:
            case Modbus::ASC:
            {
                const ModbusSerialPort::Defaults &d = ModbusSerialPort::Defaults::instance();
                QByteArray portName = settings.value(s.serialPortName, d.portName).toString().toLatin1();
                Modbus::SerialSettings sl;
                sl.portName = portName.data();
                sl.baudRate = settings.value(s.baudRate, d.baudRate).toInt();
                sl.dataBits = settings.value(s.dataBits, d.dataBits).toInt();
                sl.parity   = toParity(settings.value(s.parity, d.parity));
                sl.stopBits = toStopBits(settings.value(s.stopBits, d.stopBits));
                sl.flowControl = toFlowControl(settings.value(s.flowControl, d.flowControl));
                sl.timeoutFirstByte = settings.value(s.timeoutFirstByte, d.timeoutFirstByte).toUInt();
                sl.timeoutInterByte = settings.value(s.timeoutInterByte, d.timeoutInterByte).toUInt();
                return Modbus::createPort(type, &sl, blocking);
            }
            break;
            default:
                return nullptr;
            }
        }
    }
    return nullptr;
}

ModbusClientPort *createClientPort(const Settings &settings, bool blocking)
{
    ModbusPort *port = createPort(settings, blocking);
    if (port)
        return new ModbusClientPort(port);
    return nullptr;
}

ModbusServerPort *createServerPort(ModbusInterface *device, const Settings &settings, bool blocking)
{
    const Strings &s = Strings::instance();
    const Settings::const_iterator it = settings.constFind(s.type);
    if (it != settings.constEnd())
    {
        bool ok;
        ProtocolType type = toProtocolType(it.value(), &ok);
        if (ok)
        {
            switch (type)
            {
            case Modbus::TCP:
            {
                const ModbusTcpPort::Defaults &d = ModbusTcpPort::Defaults::instance();
                ModbusTcpServer *tcp = new ModbusTcpServer(device);
                tcp->setPort   (settings.value(s.port, d.port).toUInt());
                tcp->setTimeout(settings.value(s.timeout, d.timeout).toUInt());
                return tcp;
            }
            break;
            case Modbus::RTU:
            case Modbus::ASC:
            {
                ModbusPort *port = createPort(settings, blocking);
                return new ModbusServerResource(port, device);
            }
            break;
            default:
                return nullptr;
            }
        }
    }
    return nullptr;
}

} // namespace Modbus
