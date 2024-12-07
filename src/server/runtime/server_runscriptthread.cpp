#include "server_runscriptthread.h"

#include <QCoreApplication>
#include <QSharedMemory>
#include <QProcess>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <server.h>
#include <project/server_project.h>
#include <project/server_device.h>

typedef struct
{
    uint32_t flags;
    uint32_t count0x;
    uint32_t count1x;
    uint32_t count3x;
    uint32_t count4x;
    uint32_t cycle;
    uint32_t pycycle;
} ControlBlock;

QSharedMemory::SharedMemoryError initMem(QSharedMemory &mem, int size)
{
    mem.create(size);
    QSharedMemory::SharedMemoryError r = mem.error();
    switch (r)
    {
    case QSharedMemory::NoError:
    case QSharedMemory::AlreadyExists:
        if (!mem.isAttached())
            mem.attach();
        if (mem.isAttached())
            break;
        return mem.error();
    default:
        return mem.error();
    }
    return QSharedMemory::NoError;
}

mbServerRunScriptThread::mbServerRunScriptThread(mbServerDevice *device, QObject *parent) : QThread{parent},
    m_device(device)
{
    m_ctrlRun = true;
    moveToThread(this);
}

void mbServerRunScriptThread::run()
{
    QEventLoop eloop;

    const QString prefix = QString("ModbusTools.Server.PORT1.PLC1.");
    const QString sMemCtrl = prefix+QStringLiteral("control");
    //const QString sMem0x = prefix+QStringLiteral("mem0x");
    //const QString sMem1x = prefix+QStringLiteral("mem1x");
    //const QString sMem3x = prefix+QStringLiteral("mem3x");
    //const QString sMem4x = prefix+QStringLiteral("mem4x");

    QSharedMemory memCtrl(sMemCtrl);
    //QSharedMemory mem0x(sMem0x);
    //QSharedMemory mem1x(sMem1x);
    //QSharedMemory mem3x(sMem3x);
    //QSharedMemory mem4x(sMem4x);

    initMem(memCtrl, sizeof(ControlBlock));
    //initMem(mem0x, m_device->count_0x_bytes());
    //initMem(mem1x, m_device->count_1x_bytes());
    //initMem(mem3x, m_device->count_3x_bytes());
    //initMem(mem4x, m_device->count_4x_bytes());

    ControlBlock *control = reinterpret_cast<ControlBlock*>(memCtrl.data());
    control->count0x = m_device->count_0x();
    control->count1x = m_device->count_1x();
    control->count3x = m_device->count_3x();
    control->count4x = m_device->count_4x();
    //uint16_t *arr0x = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(mem0x.data()));
    //uint16_t *arr1x = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(mem1x.data()));
    //uint16_t *arr3x = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(mem3x.data()));
    //uint16_t *arr4x = reinterpret_cast<uint16_t*>(reinterpret_cast<char*>(mem4x.data()));

    qDebug() << "Control: key =" << memCtrl.key() << " nativeKey =" << memCtrl.nativeKey();

    control->flags |= 1;
    //control->flags = 0;
    m_ctrlRun = true;

    QFile qrcfile(":/server/python/program.py");
    QString projectPath = mbServer::global()->project()->absoluteDirPath();
    QString genFileName = projectPath + "/" + QFileInfo(qrcfile).fileName();
    qrcfile.copy(genFileName);
    QString pyscript = genFileName;//QStringLiteral("c:/Users/march/Dropbox/PRJ/ModbusTools/src/server/python/test_sharedmem.py");
    QString pyfile = QStringLiteral("c:/Python312-32/python.exe");
    QString importPath = getImportPath();
    QStringList args;
    args << "-u"
         << pyscript
         << "--importpath" << importPath;
    QProcess py;
    m_py = &py;
    //py.setProcessChannelMode(QProcess::ForwardedChannels);
    py.setProcessChannelMode(QProcess::MergedChannels);
    connect(m_py, &QProcess::readyReadStandardOutput, this, &mbServerRunScriptThread::readPyStdOut);
    py.start(pyfile, args);

    const mb::Timestamp_t timeoutStartStop = 1000;

    // Wait for start
    mb::Timestamp_t tm = mb::currentTimestamp();
    while (m_ctrlRun && (py.state() != QProcess::Running) && (mb::currentTimestamp()-tm < timeoutStartStop))
    {
        eloop.processEvents();
        Modbus::msleep(1);
    }

    if (py.state() != QProcess::Running)
    {
        mbServer::LogError("Python", QString("Can't start process '%1'").arg(py.program()));
        m_ctrlRun = false;
    }

    // Main Loop
    while (m_ctrlRun)
    {
        eloop.processEvents();
        //checkStdOut(py);
        control->cycle++;
        Modbus::msleep(1);
    }

    // Finish process
    control->flags &= (~1);
    if (py.state() != QProcess::NotRunning)
    {
        tm = mb::currentTimestamp();
        Modbus::msleep(1);
        while ((py.state() != QProcess::NotRunning) && (mb::currentTimestamp()-tm < timeoutStartStop))
        {
            eloop.processEvents();
            //checkStdOut(py);
            Modbus::msleep(1);
        }
        if (py.state() != QProcess::NotRunning)
        {
            mbServer::LogError("Python", QString("Can't stop process '%1'. Killing it").arg(py.program()));
            py.kill();
        }
    }
    eloop.processEvents();
    //checkStdOut(py);

}

void mbServerRunScriptThread::checkStdOut(QProcess &py)
{
    //QByteArray b = py.readAllStandardOutput();
    //if (b.size())
    //    mbServer::LogInfo("Python", QString::fromUtf8(b));
    if (py.bytesAvailable())
        mbServer::LogInfo("Python", QString::fromUtf8(py.readLine()));
}

void mbServerRunScriptThread::readPyStdOut()
{
    mbServer::OutputMessage(QString::fromUtf8(m_py->readAllStandardOutput()));
}

QString mbServerRunScriptThread::getImportPath()
{
    QStringList pathList;
    mbServerProject *project = mbServer::global()->project();
    QString projectPath = project->absoluteDirPath();
    pathList.append(projectPath);
    pathList.append(QCoreApplication::applicationDirPath()+QStringLiteral("/script/server"));
    pathList.append(QStringLiteral("c:/Users/march/Dropbox/PRJ/ModbusTools/src/server/python")); // Note: tmp
    QString res = pathList.join(";");
    return res;
}
