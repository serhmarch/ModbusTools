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
    uint32_t cycle;
    uint32_t count0x;
    uint32_t count1x;
    uint32_t count3x;
    uint32_t count4x;
    uint32_t exceptionStatusRef;
    uint32_t byteOrder;
    uint32_t registerOrder;
    uint32_t stoDeviceName;
    uint32_t stringTableSize;
    //char stringTable[1];
} DeviceBlock;

typedef struct
{
    uint32_t pycycle;
} PythonBlock;


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
    uint devMemChangeCounter;
    QSharedMemory *shm;
    MemoryBlockHeader *shmHeader;
    uint8_t *shmMem;
    uint8_t *shmMask;
    uint32_t changeCounter;
};

QSharedMemory::SharedMemoryError initMem(QSharedMemory &mem, size_t size)
{
    mem.create(static_cast<int>(size));
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

QString getProcessIdString()
{
    QString s;
    quint64 pid = static_cast<quint64>(QCoreApplication::applicationPid());
    while (pid)
    {
        s += QChar(static_cast<char>(pid % 26) + 'A');
        pid /= 26;
    }
    return s;
}

mbServerRunScriptThread::mbServerRunScriptThread(mbServerDevice *device, const MBSETTINGS &scripts, QObject *parent) : QThread{parent},
    m_device(device)
{
    const mbServerDevice::Strings &s = mbServerDevice::Strings::instance();
    m_deviceName = m_device->name().toUtf8();
    m_ctrlRun = true;
    m_settingImportPath = mbServer::global()->scriptImportPath();
    m_pyInterpreter = mbServer::global()->scriptDefaultExecutable();
    m_scriptUseOptimization = mbServer::global()->scriptUseOptimization();
    m_scriptLoopPeriod = mbServer::global()->scriptLoopPeriod();
    moveToThread(this);
    m_scriptInit  = scripts.value(s.scriptInit ).toString();
    m_scriptLoop  = scripts.value(s.scriptLoop ).toString();
    m_scriptFinal = scripts.value(s.scriptFinal).toString();
}

void mbServerRunScriptThread::run()
{
    QEventLoop eloop;
    mbCoreFileManager *fileManager = mbServer::global()->fileManager();

    const QString prefix = QString("ModbusTools.Server.%1.%2").arg(getProcessIdString(), m_device->name());

    const QString sMemDev = prefix+QStringLiteral(".device");
    const QString sMemPy  = prefix+QStringLiteral(".python");
    const QString sMem0x  = prefix+QStringLiteral(".mem0x" );
    const QString sMem1x  = prefix+QStringLiteral(".mem1x" );
    const QString sMem3x  = prefix+QStringLiteral(".mem3x" );
    const QString sMem4x  = prefix+QStringLiteral(".mem4x" );

    QSharedMemory memDev(sMemDev);
    QSharedMemory memPy(sMemPy);
    QSharedMemory mem0x(sMem0x);
    QSharedMemory mem1x(sMem1x);
    QSharedMemory mem3x(sMem3x);
    QSharedMemory mem4x(sMem4x);

    int szMemDevStringTable = m_deviceName.size()+1;
    int szMemDev = sizeof(DeviceBlock)+szMemDevStringTable;
    initMem(memDev, szMemDev);
    initMem(memPy, sizeof(PythonBlock));
    initMem(mem0x, sizeof(MemoryBlockHeader)+m_device->count_0x_bytes()*2);
    initMem(mem1x, sizeof(MemoryBlockHeader)+m_device->count_1x_bytes()*2);
    initMem(mem3x, sizeof(MemoryBlockHeader)+m_device->count_3x_bytes()*2);
    initMem(mem4x, sizeof(MemoryBlockHeader)+m_device->count_4x_bytes()*2);

    DeviceBlock *devMem = reinterpret_cast<DeviceBlock*>(memDev.data());
    devMem->count0x = m_device->count_0x();
    devMem->count1x = m_device->count_1x();
    devMem->count3x = m_device->count_3x();
    devMem->count4x = m_device->count_4x();
    devMem->exceptionStatusRef = m_device->exceptionStatusAddressInt();
    devMem->byteOrder = m_device->byteOrder();
    devMem->registerOrder = m_device->registerOrder();
    devMem->stoDeviceName = 0;
    devMem->stringTableSize = szMemDevStringTable;
    uint8_t *ptrDevMemStringTable = reinterpret_cast<uint8_t*>(&devMem[1]);
    memcpy(ptrDevMemStringTable, m_deviceName.data(), m_deviceName.size());
    ptrDevMemStringTable[m_deviceName.size()] = 0;

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

    memWork[0].changeCounter = memWork[0].shmHeader->changeCounter;
    memWork[1].changeCounter = memWork[1].shmHeader->changeCounter;
    memWork[2].changeCounter = memWork[2].shmHeader->changeCounter;
    memWork[3].changeCounter = memWork[3].shmHeader->changeCounter;

    // Initialize memory
    for (int i = 0; i < 4; i++)
    {
        memWork[i].devMemChangeCounter = memWork[i].devMemBlock->changeCounter();
        QSharedMemory &shm = *memWork[i].shm;
        shm.lock();
        memWork[i].devMemBlock->memGet(0, memWork[i].shmMem, memWork[i].devMemBlock->sizeBytes());
        shm.unlock();
    }

    qDebug() << "Control: key =" << memDev.key() << " nativeKey =" << memDev.nativeKey();

    devMem->flags |= 1;
    //devMem->flags = 0;
    m_ctrlRun = true;

    QString scriptFileName = QString("script_%1.py").arg(m_device->name());
    QFile scriptfile;
    bool res;
    if (m_scriptUseOptimization)
        res = fileManager->getFile(scriptFileName, scriptfile, QIODevice::ReadOnly);
    else
        res = fileManager->createTemporaryFile(scriptFileName, scriptfile, QIODevice::WriteOnly);
    if (!res)
    {
        mbServer::LogError("Python", QString("Can't create file '%1' to start Python script process").arg(scriptFileName));
        return;
    }
    if (scriptfile.openMode() & QIODevice::WriteOnly)
    {
        QString scriptInit  = getScriptInit ();
        QString scriptLoop  = getScriptLoop ();
        QString scriptFinal = getScriptFinal();

        QFile qrcfile(":/server/python/programhead.py");
        qrcfile.open(QIODevice::ReadOnly | QIODevice::Text);

        // Copy program header
        while (!qrcfile.atEnd())
        {
            QByteArray bLine = qrcfile.readLine();
            scriptfile.write(bLine);
        }
        qrcfile.close();

        // Write Init, Loop and Final script
        scriptfile.write(scriptInit .toUtf8());
        scriptfile.write(scriptLoop .toUtf8());
        scriptfile.write(scriptFinal.toUtf8());
        scriptfile.close();
        scriptfile.open(QIODevice::ReadOnly); // Note: to prevent file deletion
    }

    QString pyscript = QFileInfo(scriptfile).absoluteFilePath();
    QString pyfile = m_pyInterpreter;
    QString importPath = getImportPath();
    QStringList args;
    args << "-u"
         << pyscript
         << "--project"    << mbServer::global()->project()->absoluteFilePath()
         << "--importpath" << importPath
         << "--memid"      << prefix
         << "--period"     << QString::number(m_scriptLoopPeriod);

    mb::Timestamp_t tm;
    const mb::Timestamp_t timeoutStartStop = 1000;

    QProcess py;
    m_py = &py;
    //py.setProcessChannelMode(QProcess::ForwardedChannels);

    py.setProcessChannelMode(QProcess::MergedChannels);
    connect(m_py, &QProcess::readyReadStandardOutput, this, &mbServerRunScriptThread::readPyStdOut);
    mbServer::LogDebug("Python", QString("Try start process '%1' with args '%2'").arg(pyfile, args.join(' ')));
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
                //qDebug() << "New Header:" << head->changeCounter << ". Offset:" << head->changeByteOffset << ". Count: " << head->changeByteCount;
                uint32_t byteOffset = head->changeByteOffset;
                memWork[i].devMemBlock->memSetMask(byteOffset, memWork[i].shmMem+byteOffset, memWork[i].shmMask+byteOffset, head->changeByteCount);
                memset(memWork[i].shmMask+byteOffset, 0, head->changeByteCount);
                head->changeByteOffset = 0xFFFFFFFF;
                head->changeByteCount = 0;
                memWork[i].changeCounter = head->changeCounter;
            }
            if (memWork[i].devMemChangeCounter != memWork[i].devMemBlock->changeCounter())
            {
                memWork[i].devMemChangeCounter = memWork[i].devMemBlock->changeCounter();
                memWork[i].devMemBlock->memGet(0, memWork[i].shmMem, memWork[i].devMemBlock->sizeBytes());
            }
            shm.unlock();
        }
        devMem->cycle++;
        mb::msleep(1);
    }

    // Finish process
    devMem->flags &= (~1);
    if (py.state() != QProcess::NotRunning)
    {
        tm = mb::currentTimestamp();
        Modbus::msleep(1);
        while ((py.state() != QProcess::NotRunning) && (mb::currentTimestamp()-tm < timeoutStartStop))
        {
            eloop.processEvents();
            mb::msleep(1);
        }
        if (py.state() != QProcess::NotRunning)
        {
            mbServer::LogError("Python", QString("Can't stop process '%1'. Killing it").arg(py.program()));
            py.kill();
        }
    }
    eloop.processEvents();

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

#ifdef QT_DEBUG
    QString currentFile = __FILE__;  // Expands to the full path of the source file at compile time
    QString dir = QFileInfo(currentFile).absolutePath();
    dir = QDir::cleanPath(dir+"/../python");
    pathList.append(dir); // tmp
#endif // QT_DEBUG

    pathList.append(QCoreApplication::applicationDirPath()+QStringLiteral("/script/server"));
    if (m_settingImportPath.count() > 0)
        pathList.append(m_settingImportPath);
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
    res += "_mb_time_start = 0.0\n";
    res += "while (mbdevice.getflags() & 1):\n";
    res += "    if (time() - _mb_time_start) < _mb_time_period:\n";
    res += "        sleep(0.001)\n";
    res += "        continue\n";
    res += "    _mb_time_start = time()\n";
    QStringList lines = m_scriptLoop.split('\n', Qt::SkipEmptyParts);
    bool multiline = false;
    Q_FOREACH(const QString &line, lines)
    {
        if (processMultiLineStringLiteral(line, multiline))
            res += line + QChar('\n');
        else
            res += QStringLiteral("    ") + line + QChar('\n');
    }
    res += "    mbdevice._incpycycle()\n";
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

bool mbServerRunScriptThread::processMultiLineStringLiteral(const QString &line, bool &multiline)
{
    static const QString TripleQuotes = QStringLiteral("\"\"\"" );
    static const int szTripleQuotes = TripleQuotes.size();
    bool prev = multiline;
    int pos = 0;
    const int len = line.size();
    while (pos < len)
    {
        if (line.mid(pos, szTripleQuotes) == TripleQuotes)
        {
            pos += szTripleQuotes;
            multiline = !multiline;
        }
        else
            ++pos;
    }
    return prev;
}

