#ifndef CLIENT_RUNPORT_H
#define CLIENT_RUNPORT_H

#include <QQueue>
#include <QReadWriteLock>

#include <client_global.h>
#include <project/client_port.h>

class mbClientRunDevice;

class mbClientRunPort
{
public:
    explicit mbClientRunPort(mbClientPort *port);

public:
    inline mbClientPort *port() { return m_port; }

public: // mbClientPort interface
    // Note: as port in runtime is unchangeable so access functions can be used safely
    inline QString name() const { return m_port->name(); }
    inline MBSETTINGS settings() const { return m_port->settings(); }
    inline bool isBroadcastEnabled() const { return m_port->isBroadcastEnabled(); }
    inline mbClientPort::Statistic statistic() const { return m_port->statistic(); }
    inline void setStatCountTx(quint32 count) { m_port->setStatCountTx(count); }
    inline void setStatCountRx(quint32 count) { m_port->setStatCountRx(count); }

public:
    inline QList<mbClientRunDevice*> devices() const { return m_devices; }
    inline void pushDevices(const QList<mbClientRunDevice*> &devices) { m_devices.append(devices); }

public:
    bool hasExternalMessage() const;
    void pushExternalMessage(const mbClientRunMessagePtr &message);
    bool popExternalMessage(mbClientRunMessagePtr *message);

private:
    mutable QReadWriteLock m_lock;
    mbClientPort *m_port;
    QList<mbClientRunDevice*> m_devices;
    QQueue<mbClientRunMessagePtr> m_externalMessages;
};

#endif // CLIENT_RUNPORT_H
