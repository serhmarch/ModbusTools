#ifndef SERVER_RUNSCRIPTTHREAD_H
#define SERVER_RUNSCRIPTTHREAD_H

#include <QThread>
#include <mbcore.h>

class QProcess;
class mbServerDevice;

class mbServerRunScriptThread : public QThread
{
    Q_OBJECT
public:
    explicit mbServerRunScriptThread(mbServerDevice *device, const MBSETTINGS &scripts, QObject *parent = nullptr);

public:
    inline void stop() { m_ctrlRun = false; }

protected:
    void run() override;
    void checkStdOut(QProcess &py);

private Q_SLOTS:
    void readPyStdOut();

private:
    QString getImportPath();
    QString getScriptInit();
    QString getScriptLoop();
    QString getScriptFinal();

private:
    bool m_ctrlRun;

private:
    mbServerDevice *m_device;
    QString m_pyInterpreter;
    bool m_scriptUseOptimization;
    QString m_scriptInit ;
    QString m_scriptLoop ;
    QString m_scriptFinal;
    QProcess *m_py;
};

#endif // SERVER_RUNSCRIPTTHREAD_H
