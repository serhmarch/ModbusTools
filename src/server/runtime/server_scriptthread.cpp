#include "server_scriptthread.h"

#include <QSharedMemory>
#include <QDebug>

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

mbServerScriptThread::mbServerScriptThread(mbServerDevice *device, QObject *parent) : QThread{parent},
    m_device(device)
{
    m_ctrlRun = true;
}

void mbServerScriptThread::run()
{
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
    control->flags |= 1;

    qDebug() << "Control: key =" << memCtrl.key() << " nativeKey =" << memCtrl.nativeKey();

    control->flags |= 1;
    m_ctrlRun = true;
    while (m_ctrlRun)
    {
        /*
        if (control->cycle % 1000 == 0)
        {
            int t0[16];
            for (int i = 0; i < 16; i++)
                t0[i] = (arr0x[0]&(1<<i)) != 0;
            int t1[16];
            for (int i = 0; i < 16; i++)
                t1[i] = (arr1x[0]&(1<<i)) != 0;

            qDebug() << "Cycle :" << control->cycle << "PyCycle:" << control->pycycle;
            qDebug() << "mem0x =" << t0[0 ]
                     << t0[1 ]
                     << t0[2 ]
                     << t0[3 ]
                     << t0[4 ]
                     << t0[5 ]
                     << t0[6 ]
                     << t0[7 ]
                     << t0[8 ]
                     << t0[9 ]
                     << t0[10]
                     << t0[11]
                     << t0[12]
                     << t0[13]
                     << t0[14]
                     << t0[15]
                ;

            qDebug() << "mem1x =" << t1[0 ]
                     << t1[1 ]
                     << t1[2 ]
                     << t1[3 ]
                     << t1[4 ]
                     << t1[5 ]
                     << t1[6 ]
                     << t1[7 ]
                     << t1[8 ]
                     << t1[9 ]
                     << t1[10]
                     << t1[11]
                     << t1[12]
                     << t1[13]
                     << t1[14]
                     << t1[15]
                ;

            qDebug() << "mem3x =" << arr3x[0 ]
                     << arr3x[1 ]
                     << arr3x[2 ]
                     << arr3x[3 ]
                     << arr3x[4 ]
                     << arr3x[5 ]
                     << arr3x[6 ]
                     << arr3x[7 ]
                     << arr3x[8 ]
                     << arr3x[9 ]
                     << arr3x[10]
                     << arr3x[11]
                     << arr3x[12]
                     << arr3x[13]
                     << arr3x[14]
                     << arr3x[15]
                ;

            qDebug() << "mem4x =" << arr4x[0 ]
                     << arr4x[1 ]
                     << arr4x[2 ]
                     << arr4x[3 ]
                     << arr4x[4 ]
                     << arr4x[5 ]
                     << arr4x[6 ]
                     << arr4x[7 ]
                     << arr4x[8 ]
                     << arr4x[9 ]
                     << arr4x[10]
                     << arr4x[11]
                     << arr4x[12]
                     << arr4x[13]
                     << arr4x[14]
                     << arr4x[15]
                ;
            qDebug() << "=================================";
        }
        */
        control->cycle++;
        Modbus::msleep(1);
    }
    control->flags &= (~1);

}
