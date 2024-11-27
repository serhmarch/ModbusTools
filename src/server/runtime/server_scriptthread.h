#ifndef SERVER_SCRIPTTHREAD_H
#define SERVER_SCRIPTTHREAD_H

#include <QThread>

class mbServerDevice;

class mbServerScriptThread : public QThread
{
    Q_OBJECT
public:
    explicit mbServerScriptThread(mbServerDevice *device, QObject *parent = nullptr);

public:
    inline void stop() { m_ctrlRun = false; }

protected:
    void run() override;

private:
    bool m_ctrlRun;

private:
    mbServerDevice *m_device;
};

#endif // SERVER_SCRIPTTHREAD_H
