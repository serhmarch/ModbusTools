#include "client_scanner.h"

#include <client.h>
#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

#include "client_scannerthread.h"

#define MBCLIENTSCANNER_GET_SETTING_MACRO(type, name, assign)                                \
    type v;                                                                                  \
    bool okInner = false;                                                                    \
    Modbus::Settings::const_iterator it = s.find(mbClientScanner::Strings::instance().name); \
    if (it != s.end())                                                                       \
    {                                                                                        \
        QVariant var = it.value();                                                           \
        assign;                                                                              \
    }                                                                                        \
    if (ok)                                                                                  \
        *ok = okInner;                                                                       \
    if (okInner)                                                                             \
        return v;                                                                            \
    return mbClientScanner::Defaults::instance().name;

uint8_t mbClientScanner::getSettingUnitStart(const Modbus::Settings &s, bool *ok)
{
    MBCLIENTSCANNER_GET_SETTING_MACRO(uint8_t, unitStart, v = static_cast<uint8_t>(var.toUInt(&okInner)))
}

uint8_t mbClientScanner::getSettingUnitEnd(const Modbus::Settings &s, bool *ok)
{
    MBCLIENTSCANNER_GET_SETTING_MACRO(uint8_t, unitEnd, v = static_cast<uint8_t>(var.toUInt(&okInner)))
}

QVariantList mbClientScanner::getSettingBaudRate(const Modbus::Settings &s, bool *ok)
{
    return s.value(Modbus::Strings::instance().baudRate).toList();
}

QVariantList mbClientScanner::getSettingDataBits(const Modbus::Settings &s, bool *ok)
{
    return s.value(Modbus::Strings::instance().dataBits).toList();
}

QVariantList mbClientScanner::getSettingParity(const Modbus::Settings &s, bool *ok)
{
    return s.value(Modbus::Strings::instance().parity).toList();
}

QVariantList mbClientScanner::getSettingStopBits(const Modbus::Settings &s, bool *ok)
{
    return s.value(Modbus::Strings::instance().stopBits).toList();
}

mbClientScanner::Request_t mbClientScanner::getSettingRequest(const Modbus::Settings &s, bool *ok)
{
    return toRequest(s.value(mbClientScanner::Strings::instance().request).toString(), ok);
}

void mbClientScanner::setSettingUnitStart(Modbus::Settings &s, uint8_t v)
{
    s[mbClientScanner::Strings::instance().unitStart] = v;
}

void mbClientScanner::setSettingUnitEnd(Modbus::Settings &s, uint8_t v)
{
    s[mbClientScanner::Strings::instance().unitEnd] = v;
}

void mbClientScanner::setSettingBaudRate(Modbus::Settings &s, const QVariantList &v)
{
    s[Modbus::Strings::instance().baudRate] = v;
}

void mbClientScanner::setSettingDataBits(Modbus::Settings &s, const QVariantList &v)
{
    s[Modbus::Strings::instance().dataBits] = v;
}

void mbClientScanner::setSettingParity(Modbus::Settings &s, const QVariantList &v)
{
    s[Modbus::Strings::instance().parity] = v;
}

void mbClientScanner::setSettingStopBits(Modbus::Settings &s, const QVariantList &v)
{
    s[Modbus::Strings::instance().stopBits] = v;
}

void mbClientScanner::setSettingRequest(Modbus::Settings &s, const Request_t &req)
{
    s[mbClientScanner::Strings::instance().request] = toString(req);
}

QString mbClientScanner::toShortParityStr(Modbus::Parity v)
{
    switch (v)
    {
    case Modbus::EvenParity : return QString("E");
    case Modbus::OddParity  : return QString("O");
    default                 : return QString("N");
    }
}

 QString mbClientScanner::toShortStopBitsStr(Modbus::StopBits v)
{
    switch (v)
    {
    case Modbus::OneAndHalfStop : return QString("1.5");
    case Modbus::TwoStop        : return QString("2");
    default                     : return QString("1");
    }
}

/*
   Format of string repr of FuncParams:

   Func Prefix | Func Number (2 sym) | Sep | Offset (opt) | Sep(opt) | Count (opt)
   ------------|---------------------|-----|--------------|----------|------------
    `FN`       | `XX`                | `:` | `0-65535`    | `:`      | `0-<max>`

   Format of string repr of FuncParams: `F1;F2;F3;...;Fn`
 */
mbClientScanner::FuncParams mbClientScanner::toFuncParams(const QString &sf, bool *ok)
{
    QString s = sf.trimmed().toUpper();
    bool okInner = false;
    FuncParams f;
    const QString &func_prefix = Strings::instance().func_prefix;
    if (s.leftRef(func_prefix.size()) == func_prefix)
    {
        int i = func_prefix.size();
        int v = s.midRef(i, 2).toInt(&okInner);
        i += 2;
        if (okInner)
        {
            uint8_t func = static_cast<uint8_t>(v);
            switch (func)
            {
            case MBF_READ_COILS:
            case MBF_READ_DISCRETE_INPUTS:
            case MBF_READ_HOLDING_REGISTERS:
            case MBF_READ_INPUT_REGISTERS:
            case MBF_WRITE_MULTIPLE_COILS:
            case MBF_WRITE_MULTIPLE_REGISTERS:
            case MBF_READ_WRITE_MULTIPLE_REGISTERS:
            {
                QStringList params = s.split(Strings::instance().func_param_sep);
                if (params.count() == 3)
                {
                    uint16_t offset = static_cast<uint16_t>(params.at(1).trimmed().toUInt(&okInner));
                    if (okInner)
                    {
                        uint16_t count = static_cast<uint16_t>(params.at(2).trimmed().toUInt(&okInner));
                        if (okInner)
                        {
                            f.func = func;
                            f.offset = offset;
                            f.count = count;
                        }
                    }
                }
            }
                break;
            case MBF_WRITE_SINGLE_COIL:
            case MBF_WRITE_SINGLE_REGISTER:
            case MBF_MASK_WRITE_REGISTER:
            {
                QStringList params = s.split(Strings::instance().func_param_sep);
                if (params.count() == 2)
                {
                    uint16_t offset = static_cast<uint16_t>(params.at(1).trimmed().toUInt(&okInner));
                    if (okInner)
                    {
                        f.func = func;
                        f.offset = offset;
                        f.count = 1;
                    }
                }
            }
                break;
            case MBF_READ_EXCEPTION_STATUS:
                if (i < s.length())
                    okInner = false;
                f.func = func;
                f.offset = 0;
                f.count = 0;
                break;
            default:
                okInner = false;
                break;
            }
        }
    }
    if (ok)
        *ok = okInner;
    return f;

}

QString mbClientScanner::toString(const FuncParams &f)
{
    switch (f.func)
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        return QString("%1%2%3%4%5%6")
            .arg(Strings::instance().func_prefix)
            .arg(f.func, 2, 10, QChar('0'))
            .arg(Strings::instance().func_param_sep)
            .arg(f.offset)
            .arg(Strings::instance().func_param_sep)
            .arg(f.count);
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
    case MBF_MASK_WRITE_REGISTER:
        return QString("%1%2%3%4")
            .arg(Strings::instance().func_prefix)
            .arg(f.func, 2, 10, QChar('0'))
            .arg(Strings::instance().func_param_sep)
            .arg(f.offset);
    case MBF_READ_EXCEPTION_STATUS:
        return QString("%1%2").arg(Strings::instance().func_prefix).arg(f.func, 2, 10, QChar('0'));
    default:
        return QString();
    }
}

mbClientScanner::Request_t mbClientScanner::toRequest(const QString &sr, bool *ok)
{
    QStringList sParams = sr.split(Strings::instance().func_sep);
    bool okInner = true;
    Request_t req;
    Q_FOREACH (const QString &s, sParams)
    {
        FuncParams f = toFuncParams(s, &okInner);
        if (!okInner)
        {
            req.clear();
            break;
        }
        req.append(f);
    }
    if (ok)
        *ok = okInner;
    return req;
}

QString mbClientScanner::toString(const Request_t &req)
{
    QString sReq;
    if (req.count())
    {
        sReq = toString(req.first());
        const QChar& sep = Strings::instance().func_sep;
        for (int i = 1; i < req.count(); i++)
            sReq += sep + toString(req.at(i));
    }
    return sReq;
}

mbClientScanner::Strings::Strings() :
    name          (QStringLiteral("Scanner")),
    type          (Modbus::Strings::instance().type),
    timeout       (Modbus::Strings::instance().timeout),
    tries         (QStringLiteral("tries")),
    unitStart     (QStringLiteral("unitStart")),
    unitEnd       (QStringLiteral("unitEnd")),
    request       (QStringLiteral("request")),
    func_prefix   (QStringLiteral("FN")),
    func_param_sep(':'),
    func_sep      (';')
{
}

const mbClientScanner::Strings &mbClientScanner::Strings::instance()
{
    static Strings s;
    return s;
}

mbClientScanner::Defaults::Defaults() :
    type     (Modbus::Defaults::instance().type),
    timeout  (1000),
    tries    (Modbus::Defaults::instance().tries),
    unitStart(Modbus::VALID_MODBUS_ADDRESS_BEGIN),
    unitEnd  (Modbus::VALID_MODBUS_ADDRESS_END),
    request  (mbClientScanner::createRequestParam2(MBF_READ_HOLDING_REGISTERS, 0, 1))
{
}

const mbClientScanner::Defaults &mbClientScanner::Defaults::instance()
{
    static Defaults s;
    return s;
}

mbClientScanner::DeviceInfo::DeviceInfo()
{
    const Modbus::Defaults &d = Modbus::Defaults::instance();

    unit             = d.unit            ;;
    type             = d.type            ;;
    host             = d.host            ;;
    port             = d.port            ;;
    timeout          = d.timeout         ;;
    serialPortName   = d.serialPortName  ;;
    baudRate         = d.baudRate        ;;
    dataBits         = d.dataBits        ;;
    parity           = d.parity          ;;
    stopBits         = d.stopBits        ;;
    flowControl      = d.flowControl     ;;
    timeoutFirstByte = d.timeoutFirstByte;;
    timeoutInterByte = d.timeoutInterByte;;
}

mbClientScanner::mbClientScanner(QObject *parent)
    : QObject{parent}
{
    m_thread = new mbClientScannerThread(this);
    connect(m_thread, &QThread::started, this, &mbClientScanner::threadStarted);
    connect(m_thread, &QThread::finished, this, &mbClientScanner::threadFinished);
}

mbClientScanner::~mbClientScanner()
{
    m_thread->stop();
    while (m_thread->isRunning())
        Modbus::msleep(1);
    delete m_thread;
}

QString mbClientScanner::deviceInfoStr(int i) const
{
    QReadLocker _(&m_lock);
    if ((i >= 0) && (i < m_deviceInfoList.count()))
    {
        const DeviceInfo &d = m_deviceInfoList.at(i);
        switch (d.type)
        {
        case Modbus::ASC:
            return QString("ASC:%1:%2:%3%4%5, Unit=%6")
                .arg(d.serialPortName,
                     QString::number(d.baudRate),
                     QString::number(d.dataBits),
                     toShortParityStr(d.parity),
                     toShortStopBitsStr(d.stopBits),
                     QString::number(d.unit));
        case Modbus::RTU:
            return QString("RTU:%1:%2:%3%4%5, Unit=%6")
                .arg(d.serialPortName,
                     QString::number(d.baudRate),
                     QString::number(d.dataBits),
                     toShortParityStr(d.parity),
                     toShortStopBitsStr(d.stopBits),
                     QString::number(d.unit));
        default:
            return QString("TCP:%1:%2, Unit=%3")
                .arg(d.host,
                     QString::number(d.port),
                     QString::number(d.unit));
        }
    }
    return QString();
}

void mbClientScanner::deviceAdd(const Modbus::Settings &settings)
{
    DeviceInfo d;
    d.unit             = Modbus::getSettingUnit            (settings);
    d.type             = Modbus::getSettingType            (settings);
    d.host             = Modbus::getSettingHost            (settings);
    d.port             = Modbus::getSettingPort            (settings);
    d.timeout          = Modbus::getSettingTimeout         (settings);
    d.serialPortName   = Modbus::getSettingSerialPortName  (settings);
    d.baudRate         = Modbus::getSettingBaudRate        (settings);
    d.dataBits         = Modbus::getSettingDataBits        (settings);
    d.parity           = Modbus::getSettingParity          (settings);
    d.stopBits         = Modbus::getSettingStopBits        (settings);
    d.flowControl      = Modbus::getSettingFlowControl     (settings);
    d.timeoutFirstByte = Modbus::getSettingTimeoutFirstByte(settings);
    d.timeoutInterByte = Modbus::getSettingTimeoutInterByte(settings);
    m_lock.lockForWrite();
    int i = m_deviceInfoList.count();
    m_deviceInfoList.append(d);
    m_lock.unlock();
    Q_EMIT deviceAdded(i);
}

void mbClientScanner::addToProject(const QList<int> &indexes)
{
    QList<DeviceInfo> deviceInfos;
    if (indexes.count())
    {
        QReadLocker _(&m_lock);
        Q_FOREACH(int i, indexes)
            deviceInfos.append(m_deviceInfoList.at(i));
    }
    else
    {
        QReadLocker _(&m_lock);
        deviceInfos = m_deviceInfoList;
    }
    if (mbClient::global()->isRunning())
        return;
    mbClientProject *project = mbClient::global()->project();
    if (!project)
        return;
    Q_FOREACH(const DeviceInfo &d, deviceInfos)
    {
        mbClientPort *port = findOrCreatePort(project, d);
        mbClientDevice *device = createDevice(d);
        project->deviceAdd(device);
        port->deviceAdd(device);
    }
}

void mbClientScanner::clear()
{
    QWriteLocker _(&m_lock);
    m_deviceInfoList.clear();
    setStatDevice(QString());
    setStatFound  (0);
    setStatCountTx(0);
    setStatCountRx(0);
    setStatPercent(0);
    Q_EMIT cleared();
}

void mbClientScanner::startScanning(const Modbus::Settings &settings)
{
    if (!m_thread->isRunning())
    {
        m_thread->setSettings(settings);
        m_thread->start();
    }
}

void mbClientScanner::stopScanning()
{
    m_thread->stop();
}

void mbClientScanner::setStatDevice(const QString &device)
{
    if (m_stat.device != device)
    {
        m_stat.device = device;
        Q_EMIT statDeviceChanged(device);
    }
}

void mbClientScanner::setStatFound(quint32 count)
{
    if (m_stat.found != count)
    {
        m_stat.found = count;
        Q_EMIT statFoundChanged(count);
    }
}

void mbClientScanner::setStatCountTx(quint32 count)
{
    if (m_stat.countTx != count)
    {
        m_stat.countTx = count;
        Q_EMIT statCountTxChanged(count);
    }
}

void mbClientScanner::setStatCountRx(quint32 count)
{
    if (m_stat.countRx != count)
    {
        m_stat.countRx = count;
        Q_EMIT statCountRxChanged(count);
    }
}

void mbClientScanner::setStatPercent(quint32 percent)
{
    if (m_stat.percent != percent)
    {
        m_stat.percent = percent;
        Q_EMIT statPercentChanged(percent);
    }
}

void mbClientScanner::threadStarted()
{
    Q_EMIT stateChanged(true);
}

void mbClientScanner::threadFinished()
{
    Q_EMIT stateChanged(false);
}

mbClientPort *mbClientScanner::findOrCreatePort(mbClientProject *project, const DeviceInfo &d)
{
    mbClientPort *port = findPort(project, d);
    if (!port)
    {
        port = createPort(d);
        project->portAdd(port);
    }
    return port;
}

mbClientPort *mbClientScanner::findPort(const mbClientProject *project, const DeviceInfo &d)
{
    QList<mbClientPort*> ports = project->ports();
    Q_FOREACH(mbClientPort *e, ports)
    {
        if (e->type() != d.type)
            continue;
        switch (d.type)
        {
        case Modbus::ASC:
        case Modbus::RTU:
            if ((e->serialPortName() == d.serialPortName) &&
                (e->baudRate()       == d.baudRate      ) &&
                (e->dataBits()       == d.dataBits      ) &&
                (e->parity()         == d.parity        ) &&
                (e->stopBits()       == d.stopBits      ))
                return e;
            continue;
        default:
            if ((e->host() == d.host) &&
                (e->port() == d.port))
                return e;
            continue;
        }
    }
    return nullptr;
}

mbClientPort *mbClientScanner::createPort(const DeviceInfo &d)
{
    Modbus::Settings s = toSettings(d);
    mbClientPort *v = new mbClientPort;
    v->setSettings(s);
    return v;
}

mbClientDevice *mbClientScanner::createDevice(const DeviceInfo &d)
{
    Modbus::Settings s = toSettings(d);
    mbClientDevice *v = new mbClientDevice;
    v->setSettings(s);
    return v;
}

Modbus::Settings mbClientScanner::toSettings(const DeviceInfo &d)
{
    Modbus::Settings s;
    Modbus::setSettingUnit            (s, d.unit            );
    Modbus::setSettingType            (s, d.type            );
    Modbus::setSettingHost            (s, d.host            );
    Modbus::setSettingPort            (s, d.port            );
    Modbus::setSettingTimeout         (s, d.timeout         );
    Modbus::setSettingSerialPortName  (s, d.serialPortName  );
    Modbus::setSettingBaudRate        (s, d.baudRate        );
    Modbus::setSettingDataBits        (s, d.dataBits        );
    Modbus::setSettingParity          (s, d.parity          );
    Modbus::setSettingStopBits        (s, d.stopBits        );
    Modbus::setSettingFlowControl     (s, d.flowControl     );
    Modbus::setSettingTimeoutFirstByte(s, d.timeoutFirstByte);
    Modbus::setSettingTimeoutInterByte(s, d.timeoutInterByte);
    return s;
}

void mbClientScanner::fromSettings(DeviceInfo &d, const Modbus::Settings &s)
{
    d.unit             = Modbus::getSettingUnit            (s);
    d.type             = Modbus::getSettingType            (s);
    d.host             = Modbus::getSettingHost            (s);
    d.port             = Modbus::getSettingPort            (s);
    d.timeout          = Modbus::getSettingTimeout         (s);
    d.serialPortName   = Modbus::getSettingSerialPortName  (s);
    d.baudRate         = Modbus::getSettingBaudRate        (s);
    d.dataBits         = Modbus::getSettingDataBits        (s);
    d.parity           = Modbus::getSettingParity          (s);
    d.stopBits         = Modbus::getSettingStopBits        (s);
    d.flowControl      = Modbus::getSettingFlowControl     (s);
    d.timeoutFirstByte = Modbus::getSettingTimeoutFirstByte(s);
    d.timeoutInterByte = Modbus::getSettingTimeoutInterByte(s);
}


