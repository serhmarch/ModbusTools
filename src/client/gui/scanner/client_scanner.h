#ifndef MBCLIENTSCANNER_H
#define MBCLIENTSCANNER_H

#include <QObject>
#include <QReadWriteLock>

#include <ModbusQt.h>

class mbClientProject;
class mbClientPort;
class mbClientDevice;
class mbClientScannerThread;

class mbClientScanner : public QObject
{
    Q_OBJECT

public:
    struct FuncParams
    {
        FuncParams()
        {
            func   = 0;
            offset = 0;
            count  = 0;
        }

        uint8_t  func;
        uint16_t offset;
        uint16_t count;

    };

    typedef QList<FuncParams> Request_t;

public:
    static uint8_t      getSettingUnitStart(const Modbus::Settings &s, bool *ok = nullptr);
    static uint8_t      getSettingUnitEnd  (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingBaudRate (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingDataBits (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingParity   (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingStopBits (const Modbus::Settings &s, bool *ok = nullptr);
    static Request_t    getSettingRequest  (const Modbus::Settings &s, bool *ok = nullptr);

    static void setSettingUnitStart(Modbus::Settings &s, uint8_t v            );
    static void setSettingUnitEnd  (Modbus::Settings &s, uint8_t v            );
    static void setSettingBaudRate (Modbus::Settings &s, const QVariantList &v);
    static void setSettingDataBits (Modbus::Settings &s, const QVariantList &v);
    static void setSettingParity   (Modbus::Settings &s, const QVariantList &v);
    static void setSettingStopBits (Modbus::Settings &s, const QVariantList &v);
    static void setSettingRequest  (Modbus::Settings &s, const Request_t &req );

    static QString toShortParityStr(Modbus::Parity v);
    static QString toShortStopBitsStr(Modbus::StopBits v);

public:
    struct Strings
    {
        const QString name          ;
        const QString type          ;
        const QString timeout       ;
        const QString tries         ;
        const QString unitStart     ;
        const QString unitEnd       ;
        const QString request       ;
        const QString func_prefix   ;
        const QChar   func_param_sep;
        const QChar   func_sep      ;

        Strings();
        static const Strings &instance();
    };

    struct Defaults
    {
        const Modbus::ProtocolType type     ;
        const uint32_t             timeout  ;
        const uint32_t             tries    ;
        const uint8_t              unitStart;
        const uint8_t              unitEnd  ;
        const Request_t            request  ;

        Defaults();
        static const Defaults &instance();
    };

public:
    struct DeviceInfo
    {
        DeviceInfo();

        uint8_t                 unit            ;
        Modbus::ProtocolType    type            ;
        QString                 host            ;
        uint16_t                port            ;
        uint32_t                timeout         ;
        QString                 serialPortName  ;
        int32_t                 baudRate        ;
        int8_t                  dataBits        ;
        Modbus::Parity          parity          ;
        Modbus::StopBits        stopBits        ;
        Modbus::FlowControl     flowControl     ;
        uint32_t                timeoutFirstByte;
        uint32_t                timeoutInterByte;
    };

    static FuncParams toFuncParams(const QString &s, bool *ok = nullptr);
    static QString toString(const FuncParams &f);
    static Request_t toRequest(const QString &s, bool *ok = nullptr);
    static QString toString(const Request_t &req);
    static Request_t createRequestParam2(uint8_t funcNum, uint16_t offset, uint16_t count)
    {
        FuncParams f;
        f.func = funcNum;
        f.offset = offset;
        f.count = count;
        Request_t req;
        req.append(f);
        return req;
    }

public:
    explicit mbClientScanner(QObject *parent = nullptr);
    ~mbClientScanner();

public:
    inline int deviceCount() const { return m_deviceInfoList.count(); }
    QString deviceInfoStr(int i) const;
    void deviceAdd(const Modbus::Settings &settings);
    inline QString statDevice () const { return m_stat.device ; }
    inline quint32 statCountTx() const { return m_stat.countTx; }
    inline quint32 statCountRx() const { return m_stat.countRx; }
    inline quint32 statPercent() const { return m_stat.percent; }

public:
    void addToProject(const QList<int> &indexes = QList<int>());
    void clear();
    void startScanning(const Modbus::Settings &settings);
    void stopScanning();
    void setStatDevice(const QString &device);
    void setStatCountTx(quint32 count);
    void setStatCountRx(quint32 count);
    void setStatPercent(quint32 percent);

Q_SIGNALS:
    void deviceAdded(int index);
    void cleared();
    void stateChanged(bool run);
    void statDeviceChanged(const QString &name);
    void statCountTxChanged(quint32 count);
    void statCountRxChanged(quint32 count);
    void statPercentChanged(quint32 percent);

private Q_SLOTS:
    void threadStarted();
    void threadFinished();

private:
    mbClientPort *findOrCreatePort(mbClientProject *project, const DeviceInfo &d);
    mbClientPort *findPort(const mbClientProject *project, const DeviceInfo &d);
    mbClientPort *createPort(const DeviceInfo &d);
    mbClientDevice *createDevice(const DeviceInfo &d);
    Modbus::Settings toSettings(const DeviceInfo &d);
    void fromSettings(DeviceInfo &d, const Modbus::Settings &s);

private:
    mutable QReadWriteLock m_lock;
    QList<DeviceInfo> m_deviceInfoList;
    mbClientScannerThread *m_thread;

private:
    struct Statistics
    {
        Statistics()
        {

            countTx = 0;
            countRx = 0;
            percent = 0;
        }

        QString device;
        quint32 countTx;
        quint32 countRx;
        quint32 percent;
    };
    Statistics m_stat;
};

#endif // MBCLIENTSCANNER_H
