/*
    Modbus

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "ModbusClientPort.h"

#include <QDateTime>

#include "ModbusPort.h"

namespace Modbus {

struct ClientPort::RequestParams
{
    void *object;
    QString name;
};

ClientPort::Strings::Strings() :
    repeatCount(QStringLiteral("repeatCount"))
{
}

const ClientPort::Strings& ClientPort::Strings::instance()
{
    static const Strings s;
    return s;
}

ClientPort::Defaults::Defaults() :
    repeatCount(1)
{
}

const ClientPort::Defaults& ClientPort::Defaults::instance()
{
    static const Defaults d;
    return d;
}

ClientPort::ClientPort(Port *port, QObject *parent) :
    QObject(parent)
{
    m_state = STATE_UNKNOWN;
    m_currentRequestParams = nullptr;
    m_port = port;
    port->setParent(this);
    port->setServerMode(false);
    m_repeats = 0;
    m_settings.repeatCount = Defaults::instance().repeatCount;
    m_lastStatusTimestamp = 0;

    connect(m_port, &Port::signalTx     , this, &ClientPort::slotTx     );
    connect(m_port, &Port::signalRx     , this, &ClientPort::slotRx     );
    connect(m_port, &Port::signalError  , this, &ClientPort::setError   );
    connect(m_port, &Port::signalMessage, this, &ClientPort::setMessage );
}

ClientPort::~ClientPort()
{
}

Type ClientPort::type() const
{
    return m_port->type();
}

StatusCode ClientPort::close()
{
    StatusCode s = Status_Good;
    if (m_port->isOpen())
    {
        s = m_port->close();
        m_currentRequestParams = nullptr;
    }
    return s;
}

bool ClientPort::isOpen() const
{
    return m_port->isOpen();
}

Settings ClientPort::settings() const
{
    return m_port->settings();
}

bool ClientPort::setSettings(const Settings &settings)
{
    const Strings& s = Strings::instance();

    Settings::const_iterator it;
    Settings::const_iterator end = settings.end();

    it = settings.find(s.repeatCount);
    if (it != end)
    {
        QVariant v = it.value();
        setRepeatCount(v.toUInt());
    }

    return m_port->setSettings(settings);
}

StatusCode ClientPort::request(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff, uint16_t maxSzBuff, uint16_t *szOutBuff)
{
    m_port->writeBuffer(unit, func, buff, szInBuff);
    StatusCode r = process();
    if (StatusIsProcessing(r))
        return r;
    if (StatusIsBad(r))
    {
        setStatus(r);
        m_repeats++;
        if (m_repeats < m_settings.repeatCount)
        {
            m_port->setNextRequestRepeated(true);
            return Status_Processing;
        }
    }
    m_repeats = 0;
    m_currentRequestParams = nullptr;
    if (StatusIsBad(r))
    {
        setError(r, m_port->lastErrorText());
        return r;
    }
    r = m_port->readBuffer(unit, func, buff, maxSzBuff, szOutBuff);
    setStatus(r);
    return r;
}

StatusCode ClientPort::process()
{
    StatusCode r;
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_BEGIN:
        case STATE_CLOSED:
        case STATE_WAIT_FOR_OPEN:
            r = m_port->open();
            if (r != Status_Good) // if not OK it's mean that an error occured or in process
            {
                if (!StatusIsProcessing(r)) // an error occured
                    m_port->freeWriteBuffer(); // mark the buffer is free to store new data
                return r;
            }
            m_state = STATE_OPENED;
            fRepeatAgain = true;
            break;
        case STATE_WAIT_FOR_CLOSE:
            r = m_port->close();
            if (r != Status_Good) // if not OK it's mean that an error occured or in process
                return r;
            m_state = STATE_CLOSED;
            //fRepeatAgain = true;
            break;
        case STATE_OPENED:
        case STATE_BEGIN_WRITE:
            // send data to server
            if (m_port->isChanged())
            {
                m_state = STATE_WAIT_FOR_CLOSE;
                fRepeatAgain = true;
                break;
            }
            if (!m_port->isOpen())
            {
                m_state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            m_state = STATE_WRITE;
            // no need break
        case STATE_WRITE:
            r = m_port->write();
            if (r != Status_Good) // if not OK it's mean that an error occured or in process
            {
                if (!StatusIsProcessing(r)) // an error occured
                {
                    m_state = STATE_BEGIN_WRITE;
                    m_port->freeWriteBuffer(); // mark the buffer is free to store new data
                }
                return r;
            }
            m_state = STATE_BEGIN_READ;
            return Status_ProcessingBegin;
            // no need break
        case STATE_BEGIN_READ:
        case STATE_READ:
            r = m_port->read();
            if (!StatusIsProcessing(r)) // if process finished (Good or Bad)
            {
                m_port->freeWriteBuffer(); // mark the buffer is free to store new data
                m_state = STATE_BEGIN_WRITE;
                return r;
            }
            break;
        default:
            if (m_port->isOpen())
                m_state = STATE_OPENED;
            else
                m_state = STATE_CLOSED;
            fRepeatAgain = true;
            break;
        }
    } 
    while (fRepeatAgain);
    return Status_Processing;
}

const void *ClientPort::currentClient() const
{
    if (m_currentRequestParams)
        return m_currentRequestParams->object;
    return nullptr;
}

ClientPort::RequestParams *ClientPort::createRequestParams(void *obj, const QString &name)
{
    RequestParams *rp = new RequestParams;
    rp->object = obj;
    rp->name = name;
    return rp;
}

void ClientPort::renameRequestParams(RequestParams *rp, const QString &name)
{
    rp->name = name;
}

void ClientPort::deleteRequestParams(RequestParams *rp)
{
    delete rp;
}

ClientPort::RequestStatus ClientPort::getRequestStatus(RequestParams *rp)
{
    if (m_currentRequestParams)
    {
        if (m_currentRequestParams == rp)
            return Process;
        return Disable;
    }
    else
    {
        m_currentRequestParams = rp;
        return Enable;
    }
}

void ClientPort::cancelRequest(RequestParams* rp)
{
    if (m_currentRequestParams == rp)
        m_currentRequestParams = nullptr;
}

void ClientPort::slotTx(const QByteArray &bytes)
{
    if (m_currentRequestParams)
        Q_EMIT signalTx(m_currentRequestParams->name, bytes);
}

void ClientPort::slotRx(const QByteArray &bytes)
{
    if (m_currentRequestParams)
        Q_EMIT signalRx(m_currentRequestParams->name, bytes);
}

StatusCode ClientPort::setError(StatusCode status, const QString &message)
{
    if (m_currentRequestParams)
        Q_EMIT signalError(m_currentRequestParams->name, status, message);
    setStatus(status);
    return status;
}

void ClientPort::setMessage(const QString &message)
{
    if (m_currentRequestParams)
        Q_EMIT signalMessage(m_currentRequestParams->name, message);
}

void ClientPort::setStatus(StatusCode s)
{
    m_lastStatus = s;
    m_lastStatusTimestamp = QDateTime::currentMSecsSinceEpoch();
}

} // namespace Modbus 
