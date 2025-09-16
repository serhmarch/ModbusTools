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

private Q_SLOTS:
    void readPyStdOut();

private:
    QString getImportPath();
    QString getScriptInit();
    QString getScriptLoop();
    QString getScriptFinal();
    bool processMultiLineStringLiteral(const QString &line, bool &multiline);

private:
    bool m_ctrlRun;

private:
    mbServerDevice *m_device;
    QStringList m_settingImportPath;
    QByteArray m_deviceName;
    QString m_pyInterpreter;
    bool m_scriptUseOptimization;
    int m_scriptLoopPeriod;
    QString m_scriptInit ;
    QString m_scriptLoop ;
    QString m_scriptFinal;
    QString m_quotes;
    QProcess *m_py;
};

#endif // SERVER_RUNSCRIPTTHREAD_H
