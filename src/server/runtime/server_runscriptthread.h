#ifndef SERVER_RUNSCRIPTTHREAD_H
#define SERVER_RUNSCRIPTTHREAD_H

#include <QThread>

class QProcess;
class mbServerDevice;

class mbServerRunScriptThread : public QThread
{
    Q_OBJECT
public:
    explicit mbServerRunScriptThread(mbServerDevice *device, QObject *parent = nullptr);

public:
    inline void stop() { m_ctrlRun = false; }

protected:
    void run() override;
    void checkStdOut(QProcess &py);

private Q_SLOTS:
    void readPyStdOut();

private:
    QString getImportPath();

private:
    bool m_ctrlRun;

private:
    mbServerDevice *m_device;
    QProcess *m_py;
};

#endif // SERVER_RUNSCRIPTTHREAD_H
