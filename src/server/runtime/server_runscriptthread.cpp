#include "server_runscriptthread.h"

#include <QCoreApplication>
#include <QSharedMemory>
#include <QProcess>
#include <QEventLoop>
#include <QFile>
#include <QFileInfo>
#include <QDebug>

#include <server.h>
#include <core_filemanager.h>
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

typedef struct
{
    uint32_t changeCounter;
    uint32_t changeByteOffset;
    uint32_t changeByteCount;
    uint32_t dummy;
} MemoryBlockHeader;

struct MemWork
{
    mbServerDevice::MemoryBlock *devMemBlock;
    QSharedMemory *shm;
    MemoryBlockHeader *shmHeader;
    uint8_t *shmMem;
    uint8_t *shmMask;
    uint32_t changeCounter;
};

QSharedMemory::SharedMemoryError initMem(QSharedMemory &mem, size_t size)
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

mbServerRunScriptThread::mbServerRunScriptThread(mbServerDevice *device, const MBSETTINGS &scripts, bool useTemporary, QObject *parent) : QThread{parent},
    m_device(device)
{
    const mbServerDevice::Strings &s = mbServerDevice::Strings::instance();
    m_ctrlRun = true;
    m_pyInterpreter = mbServer::global()->scriptDefaultExecutable();
    moveToThread(this);
    m_scriptInit  = scripts.value(s.scriptInit ).toString();
    m_scriptLoop  = scripts.value(s.scriptLoop ).toString();
    m_scriptFinal = scripts.value(s.scriptFinal).toString();
}

void mbServerRunScriptThread::run()
{
    QEventLoop eloop;
    mbCoreFileManager *fileManager = mbServer::global()->fileManager();

    const QString prefix = QString("ModbusTools.Server.PORT1.PLC1");
    const QString sMemCtrl = prefix+QStringLiteral(".control");
    const QString sMem0x = prefix+QStringLiteral(".mem0x");
    const QString sMem1x = prefix+QStringLiteral(".mem1x");
    const QString sMem3x = prefix+QStringLiteral(".mem3x");
    const QString sMem4x = prefix+QStringLiteral(".mem4x");

    QSharedMemory memCtrl(sMemCtrl);
    QSharedMemory mem0x(sMem0x);
    QSharedMemory mem1x(sMem1x);
    QSharedMemory mem3x(sMem3x);
    QSharedMemory mem4x(sMem4x);

    initMem(memCtrl, sizeof(ControlBlock));
    initMem(mem0x, sizeof(MemoryBlockHeader)+m_device->count_0x_bytes()*2);
    initMem(mem1x, sizeof(MemoryBlockHeader)+m_device->count_1x_bytes()*2);
    initMem(mem3x, sizeof(MemoryBlockHeader)+m_device->count_3x_bytes()*2);
    initMem(mem4x, sizeof(MemoryBlockHeader)+m_device->count_4x_bytes()*2);

    ControlBlock *control = reinterpret_cast<ControlBlock*>(memCtrl.data());
    control->count0x = m_device->count_0x();
    control->count1x = m_device->count_1x();
    control->count3x = m_device->count_3x();
    control->count4x = m_device->count_4x();

    MemWork memWork[4];

    memWork[0].devMemBlock = &m_device->memBlockRef_0x();
    memWork[1].devMemBlock = &m_device->memBlockRef_1x();
    memWork[2].devMemBlock = &m_device->memBlockRef_3x();
    memWork[3].devMemBlock = &m_device->memBlockRef_4x();

    memWork[0].shm = &mem0x;
    memWork[1].shm = &mem1x;
    memWork[2].shm = &mem3x;
    memWork[3].shm = &mem4x;

    memWork[0].shmHeader = reinterpret_cast<MemoryBlockHeader*>(mem0x.data());
    memWork[1].shmHeader = reinterpret_cast<MemoryBlockHeader*>(mem1x.data());
    memWork[2].shmHeader = reinterpret_cast<MemoryBlockHeader*>(mem3x.data());
    memWork[3].shmHeader = reinterpret_cast<MemoryBlockHeader*>(mem4x.data());

    memWork[0].shmMem = reinterpret_cast<uint8_t*>(mem0x.data())+sizeof(MemoryBlockHeader);
    memWork[1].shmMem = reinterpret_cast<uint8_t*>(mem1x.data())+sizeof(MemoryBlockHeader);
    memWork[2].shmMem = reinterpret_cast<uint8_t*>(mem3x.data())+sizeof(MemoryBlockHeader);
    memWork[3].shmMem = reinterpret_cast<uint8_t*>(mem4x.data())+sizeof(MemoryBlockHeader);

    memWork[0].shmMask = reinterpret_cast<uint8_t*>(mem0x.data())+sizeof(MemoryBlockHeader)+m_device->count_0x_bytes();
    memWork[1].shmMask = reinterpret_cast<uint8_t*>(mem1x.data())+sizeof(MemoryBlockHeader)+m_device->count_1x_bytes();
    memWork[2].shmMask = reinterpret_cast<uint8_t*>(mem3x.data())+sizeof(MemoryBlockHeader)+m_device->count_3x_bytes();
    memWork[3].shmMask = reinterpret_cast<uint8_t*>(mem4x.data())+sizeof(MemoryBlockHeader)+m_device->count_4x_bytes();

    //int szBytes0x = mem0x.size();
    //int szBytes1x = mem1x.size();
    //int szBytes3x = mem3x.size();
    //int szBytes4x = mem4x.size();

    //void *ptrShmMem0 = memWork[0].shmMem;
    //void *ptrShmMem1 = memWork[1].shmMem;
    //void *ptrShmMem3 = memWork[2].shmMem;
    //void *ptrShmMem4 = memWork[3].shmMem;

    //void *ptrShmMask0 = memWork[0].shmMask;
    //void *ptrShmMask1 = memWork[1].shmMask;
    //void *ptrShmMask3 = memWork[2].shmMask;
    //void *ptrShmMask4 = memWork[3].shmMask;

    memWork[0].changeCounter = memWork[0].shmHeader->changeCounter;
    memWork[1].changeCounter = memWork[1].shmHeader->changeCounter;
    memWork[2].changeCounter = memWork[2].shmHeader->changeCounter;
    memWork[3].changeCounter = memWork[3].shmHeader->changeCounter;

    qDebug() << "Control: key =" << memCtrl.key() << " nativeKey =" << memCtrl.nativeKey();

    control->flags |= 1;
    //control->flags = 0;
    m_ctrlRun = true;

    QString scriptInit  = getScriptInit ();
    QString scriptLoop  = getScriptLoop ();
    QString scriptFinal = getScriptFinal();

    QFile qrcfile(":/server/python/programhead.py");
    qrcfile.open(QIODevice::ReadOnly  | QIODevice::Text);

    //QString projectPath = mbServer::global()->project()->absoluteDirPath();
    //QString genFileName = projectPath + "/" + QFileInfo(qrcfile).fileName();
    //QFile genfile(genFileName);
    //genfile.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
    //QString genFileName = QFileInfo(qrcfile).fileName();
    QString genFileName("program.py");
    QFile genfile;
    fileManager->createTemporaryFile(genFileName, genfile, QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);

    // Copy program header
    while (!qrcfile.atEnd())
    {
        QByteArray bLine = qrcfile.readLine();
        genfile.write(bLine);
    }
    qrcfile.close();

    // Write Init, Loop and Final script
    genfile.write(scriptInit .toUtf8());
    genfile.write(scriptLoop .toUtf8());
    genfile.write(scriptFinal.toUtf8());
    genfile.close();

    QString pyscript = QFileInfo(genfile).absoluteFilePath();//QStringLiteral("c:/Users/march/Dropbox/PRJ/ModbusTools/src/server/python/test_sharedmem.py");
    QString pyfile = m_pyInterpreter; //QStringLiteral("c:/Python312-32/python.exe");
    QString importPath = getImportPath();
    QStringList args;
    args << "-u"
         << pyscript
         << "--importpath" << importPath
         << "--memid"     << prefix;

    mb::Timestamp_t tm;
    const mb::Timestamp_t timeoutStartStop = 1000;

    QProcess py;
    m_py = &py;
    //py.setProcessChannelMode(QProcess::ForwardedChannels);

    py.setProcessChannelMode(QProcess::MergedChannels);
    connect(m_py, &QProcess::readyReadStandardOutput, this, &mbServerRunScriptThread::readPyStdOut);
    py.start(pyfile, args);

    // Wait for start
    tm = mb::currentTimestamp();
    while (m_ctrlRun && (py.state() != QProcess::Running) && (mb::currentTimestamp()-tm < timeoutStartStop))
    {
        eloop.processEvents();
        mb::msleep(1);
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
        for (int i = 0; i < 4; i++)
        {
            QSharedMemory &shm = *memWork[i].shm;
            MemoryBlockHeader *head = memWork[i].shmHeader;
            shm.lock();
            if (memWork[i].changeCounter != head->changeCounter)
            {
                qDebug() << "New Header:" << head->changeCounter << ". Offset:" << head->changeByteOffset << ". Count: " << head->changeByteCount;
                uint32_t byteOffset = head->changeByteOffset;
                memWork[i].devMemBlock->memSetMask(byteOffset, memWork[i].shmMem+byteOffset, memWork[i].shmMask+byteOffset, head->changeByteCount);
                memset(memWork[i].shmMask+byteOffset, 0, head->changeByteCount);
                head->changeByteOffset = 0xFFFFFFFF;
                head->changeByteCount = 0;
                memWork[i].changeCounter = head->changeCounter;
            }
            memWork[i].devMemBlock->memGet(0, memWork[i].shmMem, memWork[i].devMemBlock->sizeBytes());
            shm.unlock();
        }
        control->cycle++;
        mb::msleep(1);
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
            mb::msleep(1);
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

QString mbServerRunScriptThread::getScriptInit()
{
    QString res("\n");
    res += "#############################################\n"
           "########### USER CODE: INITIALIZE ###########\n"
           "#############################################\n\n";
    res += m_scriptInit;
    res += QChar('\n');
    return res;
}

QString mbServerRunScriptThread::getScriptLoop()
{
    QString res("\n");
    res += "#############################################\n"
           "############## USER CODE: LOOP ##############\n"
           "#############################################\n\n";
    res += "while (_ctrl.getflags() & 1):\n";
    QStringList lines = m_scriptLoop.split('\n', Qt::SkipEmptyParts);
    Q_FOREACH(const QString &line, lines)
        res += QStringLiteral("    ") + line + QChar('\n');
    res += QStringLiteral("    ") + QStringLiteral("sleep(0.001)") + QChar('\n');
    res += QChar('\n');
    return res;
}

QString mbServerRunScriptThread::getScriptFinal()
{
    QString res("\n");
    res += "#############################################\n"
           "############ USER CODE: FINALIZE ############\n"
           "#############################################\n\n";
    res += m_scriptFinal;
    res += QChar('\n');
    return res;
}
