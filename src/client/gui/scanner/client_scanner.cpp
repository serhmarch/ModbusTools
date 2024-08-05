#include "client_scanner.h"

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

mbClientScanner::Strings::Strings() :
    name     (QStringLiteral("Scanner")),
    type     (Modbus::Strings::instance().type),
    timeout  (Modbus::Strings::instance().timeout),
    tries    (QStringLiteral("tries")),
    unitStart(QStringLiteral("unitStart")),
    unitEnd  (QStringLiteral("unitEnd"))
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
    tries    (1),
    unitStart(Modbus::VALID_MODBUS_ADDRESS_BEGIN),
    unitEnd  (Modbus::VALID_MODBUS_ADDRESS_END)
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
            return QString("TCP%1:%2, Unit=%3")
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

}

void mbClientScanner::clear()
{
    QWriteLocker _(&m_lock);
    m_deviceInfoList.clear();
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

void mbClientScanner::threadStarted()
{
    Q_EMIT stateChanged(true);
}

void mbClientScanner::threadFinished()
{
    Q_EMIT stateChanged(false);
}


