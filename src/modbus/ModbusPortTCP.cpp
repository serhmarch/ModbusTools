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
#include "ModbusPortTCP.h"

#include <QDateTime>
#include <QTcpSocket>

namespace Modbus {

PortTCP::Strings::Strings() : Port::Strings(),
    host(QStringLiteral("host")),
    port(QStringLiteral("port")),
    timeout(QStringLiteral("timeout"))
{
}

const PortTCP::Strings &PortTCP::Strings::instance()
{
    static const Strings s;
    return s;
}

PortTCP::Defaults::Defaults() : Port::Defaults(),
    host(QStringLiteral("127.0.0.1")),
    port(static_cast<uint16_t>(STANDARD_TCP_PORT)),
    timeout(3000)
{
}

const PortTCP::Defaults &PortTCP::Defaults::instance()
{
    static const Defaults d;
    return d;
}

PortTCP::PortTCP(QTcpSocket *socket, QObject *parent) :
    Port(parent),
    m_socket(socket)
{
    const Defaults &d = Defaults::instance();

    m_autoIncrement = true;
    m_host = d.host;
    m_port = d.port;
    m_timeout = d.timeout;
    m_transaction = 0;
}

PortTCP::PortTCP(QObject *parent) :
    PortTCP(new QTcpSocket, parent)

{
    m_socket->setParent(this);
}

PortTCP::~PortTCP()
{
}

Settings PortTCP::settings() const
{
    Settings params;
    Strings s = Strings::instance();
    params[s.host] = host();
    params[s.port] = port();
    params[s.timeout] = timeout();
    return params;
}

bool PortTCP::setSettings(const Settings &settings)
{
    const Strings &s = Strings::instance();

    Settings::const_iterator it;
    Settings::const_iterator end = settings.end();

    it = settings.find(s.host);
    if (it != end)
    {
        QVariant v = it.value();
        setHost(v.toString());
    }

    it = settings.find(s.port);
    if (it != end)
    {
        QVariant v = it.value();
        setPort(static_cast<uint16_t>(v.toUInt()));
    }

    it = settings.find(s.timeout);
    if (it != end)
    {
        QVariant v = it.value();
        setTimeout(v.toUInt());
    }

    return true;
}

StatusCode PortTCP::open()
{
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_UNKNOWN:
        case STATE_CLOSED:
        {
            clearChanged();
            if (isOpen())
            {
                m_state = STATE_BEGIN;
                return Status_Good;
            }
            m_socket->connectToHost(m_host, m_port);
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_OPEN;
        }
            // no need break
        case STATE_WAIT_FOR_OPEN:
        {
            QAbstractSocket::SocketState s = m_socket->state();
            if (s == QAbstractSocket::ConnectedState)
            {
                setMessage(QString("Connected to host '%1:%2'").arg(host()).arg(port()));
                m_state = STATE_BEGIN;
                return Status_Good;
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp >= m_timeout)
            {
                m_socket->abort();
                m_state = STATE_CLOSED;
                return setError(Status_BadTcpConnect, QStringLiteral("TCP. Error while connecting - timeout"));
            }
        }
            break;
        default:
            if (!isOpen())
            {
                m_state = STATE_CLOSED;
                fRepeatAgain = true;
                break;
            }
            return Status_Good;
        }
    } 
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode PortTCP::close()
{
    m_socket->close();
    m_state = STATE_CLOSED;
    return Status_Good;
}

bool PortTCP::isOpen() const
{
    return m_socket->state() == QAbstractSocket::ConnectedState;
}

void PortTCP::setHost(const QString & host) 
{
    if (m_host != host)
    {
        m_host = host;
        setChanged();
    }
}

void PortTCP::setPort(uint16_t port) 
{ 
    if (m_port != port)
    {
        m_port = port;
        setChanged();
    }
}

void PortTCP::setNextRequestRepeated(bool v)
{
    m_autoIncrement = !v;
}

StatusCode PortTCP::write()
{
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_BEGIN:
        case STATE_PREPARE_TO_WRITE:
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_WRITE;
            // no need break
        case STATE_WAIT_FOR_WRITE:
            if (m_socket->write(reinterpret_cast<char*>(m_buff), m_sz) == -1)
            {
                m_state = STATE_BEGIN;
                return setError(Status_BadTcpWrite, QString("TCP. Error while writing - %1").arg(m_socket->errorString()));
            }
            m_state = STATE_WAIT_FOR_WRITE_ALL;
            // no need break
        case STATE_WAIT_FOR_WRITE_ALL:
            Q_EMIT signalTx(QByteArray(reinterpret_cast<char*>(m_buff), m_sz));
            m_state = STATE_BEGIN;
            return Status_Good;
            /*if (m_socket->bytesToWrite() == 0)
            {
                Q_EMIT signalTx(QByteArray(reinterpret_cast<char*>(m_buff), m_sz));
                m_state = STATE_BEGIN;
                return Status_Good;
            }
            else
            {
                close();
                m_state = STATE_CLOSED;
                return setError(Status_BadTcpWrite, QStringLiteral("TCP. Error while writing"));
            }
            break;*/
        default:
            if (isOpen())
            {
                m_state = STATE_BEGIN;
                fRepeatAgain = true;
            }
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode PortTCP::read()
{
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch (m_state)
        {
        case STATE_BEGIN:
        case STATE_PREPARE_TO_READ:
            m_sz = 0;
            m_packetSz = 0;
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_READ;
            // no need break
        case STATE_WAIT_FOR_READ:
            if (m_socket->bytesAvailable())
            {
                qint64 c = m_socket->read(reinterpret_cast<char*>(m_buff), MB_TCP_PREFIX_SZ);
                if (c < 0)
                {
                    m_state = STATE_BEGIN;
                    return setError(Status_BadTcpRead, QString("TCP. Error while reading - %1").arg(m_socket->errorString()));
                }
                m_sz = static_cast<uint16_t>(c);
                m_packetSz = (m_buff[5] | (m_buff[4] << 8)) + MB_TCP_PREFIX_SZ;
                m_state = STATE_WAIT_FOR_READ_ALL;
                // no need break
            }
            else if (QDateTime::currentMSecsSinceEpoch()-m_timestamp >= m_timeout) // waiting timeout read first byte elapsed
            {
                close();
                return setError(Status_BadTcpRead, QStringLiteral("TCP. Error while reading - timeout"));
            }
            else
                break;
        case STATE_WAIT_FOR_READ_ALL:
        {
            if (m_socket->bytesAvailable())
            {
                qint64 c = m_socket->read(reinterpret_cast<char*>(m_buff+m_sz), m_packetSz-m_sz);
                if (c < 0)
                {
                    m_state = STATE_BEGIN;
                    return setError(Status_BadTcpRead, QString("TCP. Error while reading - %1").arg(m_socket->errorString()));
                }
                m_sz += static_cast<uint16_t>(c);
                if (m_sz >= m_packetSz)
                {
                    Q_EMIT signalRx(QByteArray(reinterpret_cast<char*>(m_buff), m_sz));
                    m_state = STATE_BEGIN;
                    return Status_Good;
                }
            }
            else if (QDateTime::currentMSecsSinceEpoch()-m_timestamp >= m_timeout) // waiting timeout read first byte elapsed
            {
                close();
                return setError(Status_BadTcpRead, QStringLiteral("TCP. Error while reading - timeout"));
            }
        }
            break;
        default:
            if (isOpen())
            {
                m_state = STATE_BEGIN;
                fRepeatAgain = true;
            }
            break;
        }
    }
    while (fRepeatAgain);
    return Status_Processing;
}

StatusCode PortTCP::writeBuffer(uint8_t slave, uint8_t func, uint8_t *buff, uint16_t szInBuff)
{
    if (!m_modeServer)
    {
        if (m_block)
            return Status_Processing;
        m_transaction += m_autoIncrement;
        m_autoIncrement = true;
        m_unit = slave;
        m_func = func;
        m_block = true;
    } // if (!m_modeServer)

    // 8 = 6(TCP prefix size in bytes) + 2(slave and function bytes)
    if (szInBuff > MBCLIENTTCP_BUFF_SZ - 8)
        return setError(Status_BadWriteBufferOverflow, QStringLiteral("TCP. Write-buffer overflow"));
    // standart TCP message prefix
    m_buff[0] = static_cast<uint8_t>(m_transaction >> 8);  // transaction id
    m_buff[1] = static_cast<uint8_t>(m_transaction);       // transaction id
    m_buff[2] = 0;
    m_buff[3] = 0;
    uint16_t cBytes = szInBuff + 2; // quantity of next bytes
    m_buff[4] = reinterpret_cast<uint8_t*>(&cBytes)[1]; // quantity of next bytes (MSB)
    m_buff[5] = reinterpret_cast<uint8_t*>(&cBytes)[0]; // quantity of next bytes (LSB)
    // slave, function, data
    m_buff[6] = slave;
    m_buff[7] = func;
    memcpy(&m_buff[8], buff, szInBuff);
    m_sz = szInBuff + 8;
    return Status_Good;
}

StatusCode PortTCP::readBuffer(uint8_t &slave, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff)
{
    if (m_sz < 8)
        return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct response. Responsed data length to small"));

    uint16_t transaction = m_buff[1] | (m_buff[0] << 8);

    if (!((m_buff[2] == 0) && (m_buff[3] == 0)))
        return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct read-buffer's TCP-prefix"));

    uint16_t cBytes = m_buff[5] | (m_buff[4] << 8);
    if (cBytes != (m_sz-6))
        return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct read-buffer's TCP-prefix. Size defined in TCP-prefix is not equal to actual response-size"));
    
    if (m_modeServer)
    {
        m_transaction = transaction;
    }
    else
    {
        if (m_transaction != transaction)
            return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct response. Requested transaction id is not equal to responded"));

        if (m_buff[6] != m_unit)
            return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct response. Requested unit (slave) is not equal to responsed"));

        if ((m_buff[7] & MBF_EXCEPTION) == MBF_EXCEPTION)
        {
            if (m_sz > 8)
            {
                StatusCode r = static_cast<StatusCode>(m_buff[8]); // Returned modbus exception
                return setError(static_cast<StatusCode>(Status_Bad | r), QString(QStringLiteral("TCP. Returned Modbus-exception with code '%1'")).arg(static_cast<int>(r)));
            }
            else
                return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Exception status missed"));
        }

        if (m_buff[7] != m_func)
            return setError(Status_BadNotCorrectResponse, QStringLiteral("TCP. Not correct response. Requested function is not equal to responsed"));
    }
    slave = m_buff[6];
    func = m_buff[7];

    m_sz = m_sz - 8;
    if (m_sz > maxSzBuff)
        m_sz = maxSzBuff;
    memcpy(buff, &m_buff[8], m_sz);
    *szOutBuff = m_sz;
    return Status_Good;
}

} // namespace Modbus 
