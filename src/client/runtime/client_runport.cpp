#include "client_runport.h"

#include "client_runmessage.h"

mbClientRunPort::mbClientRunPort(mbClientPort *port) :
    m_port(port)
{
}

bool mbClientRunPort::hasExternalMessage() const
{
    QReadLocker _(&m_lock);
    return m_externalMessages.count();
}

void mbClientRunPort::pushExternalMessage(const mbClientRunMessagePtr &message)
{
    QWriteLocker _(&m_lock);
    m_externalMessages.enqueue(message);
}

bool mbClientRunPort::popExternalMessage(mbClientRunMessagePtr *message)
{
    QWriteLocker _(&m_lock);
    if (m_externalMessages.count())
    {
        *message = m_externalMessages.dequeue();
        return true;
    }
    return false;
}

