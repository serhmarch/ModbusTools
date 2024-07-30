#ifndef MBCLIENTSCANNER_H
#define MBCLIENTSCANNER_H

#include <QObject>
#include <QReadWriteLock>

#include <ModbusQt.h>

class mbClientScannerThread;

class mbClientScanner : public QObject
{
    Q_OBJECT

public:
    static uint8_t getSettingUnitStart(const Modbus::Settings &s, bool *ok = nullptr);
    static uint8_t getSettingUnitEnd  (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingBaudRates(const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingDataBits (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingParities (const Modbus::Settings &s, bool *ok = nullptr);
    static QVariantList getSettingStopBits (const Modbus::Settings &s, bool *ok = nullptr);

    static void setSettingUnitStart(Modbus::Settings &s, uint8_t v);
    static void setSettingUnitEnd  (Modbus::Settings &s, uint8_t v);
    static void setSettingBaudRates(Modbus::Settings &s, const QVariantList &v);
    static void setSettingDataBits (Modbus::Settings &s, const QVariantList &v);
    static void setSettingParities (Modbus::Settings &s, const QVariantList &v);
    static void setSettingStopBits (Modbus::Settings &s, const QVariantList &v);

public:
    struct Strings
    {
        const QString name     ;
        const QString type     ;
        const QString timeout  ;
        const QString tries    ;
        const QString unitStart;
        const QString unitEnd  ;

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

public:
    explicit mbClientScanner(QObject *parent = nullptr);
    ~mbClientScanner();

public:
    inline int deviceCount() const { return m_deviceInfoList.count(); }
    QString deviceInfoStr(int i) const;
    void deviceAdd(const Modbus::Settings &settings);

public:
    void addToProject(const QList<int> &indexes = QList<int>());
    void clear();
    void startScanning(const Modbus::Settings &settings);
    void stopScanning();

Q_SIGNALS:
    void deviceAdded(int index);
    void cleared();
    void stateChanged(bool run);

private Q_SLOTS:
    void threadStarted();
    void threadFinished();

private:
    mutable QReadWriteLock m_lock;
    QList<DeviceInfo> m_deviceInfoList;
    mbClientScannerThread *m_thread;
};

#endif // MBCLIENTSCANNER_H
