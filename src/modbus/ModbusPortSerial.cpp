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
#include "ModbusPortSerial.h"

#include <QVariant>
#include <QDateTime>

namespace Modbus {

PortSerial::Strings::Strings() : Port::Strings (),
    serialPortName(QStringLiteral("serialPortName")),
    baudRate(QStringLiteral("baudRate")),
    dataBits(QStringLiteral("dataBits")),
    parity(QStringLiteral("parity")),
    stopBits(QStringLiteral("stopBits")),
    flowControl(QStringLiteral("flowControl")),
    timeoutFirstByte(QStringLiteral("timeoutFirstByte")),
    timeoutInterByte(QStringLiteral("timeoutInterByte"))
{
}

const PortSerial::Strings &PortSerial::Strings::instance()
{
    static const Strings s;
    return s;
}

PortSerial::Defaults::Defaults() : Port::Defaults(),
    serialPortName(QString()),
    baudRate(9600),
    dataBits(QSerialPort::Data8),
    parity(QSerialPort::NoParity),
    stopBits(QSerialPort::OneStop),
    flowControl(QSerialPort::NoFlowControl),
    timeoutFirstByte(500),
    timeoutInterByte(50)
{
}

const PortSerial::Defaults &PortSerial::Defaults::instance()
{
    static const Defaults d;
    return d;
}

PortSerial::PortSerial(QSerialPort *serialPort, QObject *parent) :
    Port(parent),
    c_buffSz(0) // must be reinitialized in derived class
{
    Defaults d = Defaults::instance();

    m_buff = nullptr;
    m_serialPort = serialPort;

    setBaudRate(d.baudRate);
    setDataBits(d.dataBits);
    setStopBits(d.stopBits);
    setParity(d.parity);
    setFlowControl(d.flowControl);
    setTimeoutFirstByte(d.timeoutFirstByte);
    setTimeoutInterByte(d.timeoutInterByte);
}

PortSerial::PortSerial(QObject *parent) :
    PortSerial(new QSerialPort, parent)
{
    m_serialPort->setParent(this);
}

StatusCode PortSerial::open()
{
    bool fRepeatAgain;
    //QAbstractSocket::SocketState s;
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
            if (m_serialPort->open(QIODevice::ReadWrite))
            {
                setMessage(QString("Serial port '%1' is opened").arg(serialPortName()));
                m_state = STATE_BEGIN;
                return Status_Good;
            }
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_OPEN;
        }
            // no need break
        case STATE_WAIT_FOR_OPEN:
            if (m_serialPort->isOpen())
            {
                setMessage(QString("Serial port '%1' is opened").arg(serialPortName()));
                m_state = STATE_BEGIN;
                return Status_Good;
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp >= m_timeoutFB)
            {
                //m_serialPort->close();
                m_state = STATE_CLOSED;
                return setError(Status_BadSerialOpen, QString("Can't open serial port '%1' - %2").arg(serialPortName(), m_serialPort->errorString()));
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

StatusCode PortSerial::close()
{
    if (m_serialPort->isOpen())
        m_serialPort->close();
    setMessage(QString("Serial port '%1' is closed").arg(serialPortName()));
    m_state = STATE_CLOSED;
    return Status_Good;
}

bool PortSerial::isOpen() const
{
    return m_serialPort->isOpen();
}

Settings PortSerial::settings() const
{
    Settings params;
    Strings s = Strings::instance();
    params.insert(s.serialPortName, serialPortName());
    params.insert(s.baudRate, baudRate());
    params.insert(s.dataBits, dataBits());
    params.insert(s.parity, Modbus::enumKey<QSerialPort::Parity>(parity()));
    params.insert(s.stopBits, Modbus::enumKey<QSerialPort::StopBits>(stopBits()));
    params.insert(s.flowControl, Modbus::enumKey<QSerialPort::FlowControl>(flowControl()));
    params.insert(s.timeoutFirstByte, timeoutFirstByte());
    params.insert(s.timeoutInterByte, timeoutInterByte());
    return params;
}

bool PortSerial::setSettings(const Settings &settings)
{
    const Strings &s = Strings::instance();

    Settings::const_iterator it;
    Settings::const_iterator end = settings.end();

    bool ok;
    it = settings.find(s.serialPortName);
    if (it != end)
    {
        QVariant v = it.value();
        setSerialPortName(v.toString());
    }

    it = settings.find(s.baudRate);
    if (it != end)
    {
        QVariant v = it.value();
        setBaudRate(static_cast<qint32>(v.toInt()));
    }

    it = settings.find(s.dataBits);
    if (it != end)
    {
        QVariant v = it.value();
        QSerialPort::DataBits t = Modbus::enumValue<QSerialPort::DataBits>(v, &ok);
        if (ok)
            setDataBits(t);
    }

    it = settings.find(s.parity);
    if (it != end)
    {
        QVariant v = it.value();
        QSerialPort::Parity t = Modbus::enumValue<QSerialPort::Parity>(v, &ok);
        if (ok)
            setParity(t);
    }

    it = settings.find(s.stopBits);
    if (it != end)
    {
        QVariant v = it.value();
        QSerialPort::StopBits t = Modbus::enumValue<QSerialPort::StopBits>(v, &ok);
        if (ok)
            setStopBits(t);
    }

    it = settings.find(s.flowControl);
    if (it != end)
    {
        QVariant v = it.value();
        QSerialPort::FlowControl t = Modbus::enumValue<QSerialPort::FlowControl>(v, &ok);
        if (ok)
            setFlowControl(t);
    }

    it = settings.find(s.timeoutFirstByte);
    if (it != end)
    {
        QVariant v = it.value();
        setTimeoutFirstByte(v.toUInt());
    }

    it = settings.find(s.timeoutInterByte);
    if (it != end)
    {
        QVariant v = it.value();
        setTimeoutInterByte(v.toUInt());
    }

    return true;
}

StatusCode PortSerial::write()
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
            // Note: clean read buffer from garbage before write
            m_serialPort->readAll();
            if (m_serialPort->write(reinterpret_cast<char*>(m_buff), m_sz) == -1)
            {
                m_state = STATE_BEGIN;
                return setError(Status_BadSerialWrite, QString("Error while writing serial port '%1' - %2")
                                                          .arg(serialPortName(), m_serialPort->errorString()));
            }
            m_state = STATE_WAIT_FOR_WRITE_ALL;
            // no need break
        case STATE_WAIT_FOR_WRITE_ALL:
            if (m_serialPort->bytesToWrite() == 0)
            {
                m_state = STATE_BEGIN;
                Q_EMIT signalTx(QByteArray(reinterpret_cast<char*>(m_buff), m_sz));
                return Status_Good;
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp > m_timeoutFB)
            {
                m_state = STATE_BEGIN;
                return setError(Status_BadSerialWrite, QString("Error while writing serial port '%1' - %2")
                                                          .arg(serialPortName(), m_serialPort->errorString()));
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

StatusCode PortSerial::read()
{
    bool fRepeatAgain;
    do
    {
        fRepeatAgain = false;
        switch(m_state)
        {
        case STATE_BEGIN:
        case STATE_PREPARE_TO_READ:
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_READ;
            m_sz = 0;
            // no need break
        case STATE_WAIT_FOR_READ:
            // read first byte state
            if (m_serialPort->bytesAvailable())
            {
                qint64 c = m_serialPort->read(reinterpret_cast<char*>(&m_buff[0]), c_buffSz);
                if (c < 0)
                {
                    m_state = STATE_BEGIN;
                    return setError(Status_BadSerialRead, QString("Error while reading serial port '%1' - %2")
                                                             .arg(serialPortName(), m_serialPort->errorString()));
                }
                m_sz += static_cast<quint16>(c);
                if (m_sz > c_buffSz)
                {
                    m_state = STATE_BEGIN;
                    return setError(Status_BadReadBufferOverflow, QString("Serial port's '%1' read-buffer overflow").arg(serialPortName()));
                }
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp >= m_timeoutFB) // waiting timeout read first byte elapsed
            {
                m_state = STATE_BEGIN;
                return setError(Status_BadSerialRead, QString("Error while reading serial port '%1' - timeout")
                                                         .arg(serialPortName()));
            }
            else
            {
                break;
            }
            m_timestamp = QDateTime::currentMSecsSinceEpoch();
            m_state = STATE_WAIT_FOR_READ_ALL;
            // no need break
        case STATE_WAIT_FOR_READ_ALL:
            // next bytes state
            if (m_serialPort->bytesAvailable())
            {
                qint64 c = m_serialPort->read(reinterpret_cast<char*>(&m_buff[m_sz]), c_buffSz-m_sz);
                if (c < 0)
                    return setError(Modbus::Status_BadSerialRead, QString("Error while reading serial port '%1' - %2")
                                                             .arg(serialPortName(), m_serialPort->errorString()));
                m_sz += static_cast<quint16>(c);
                if (m_sz > c_buffSz)
                    return setError(Modbus::Status_BadReadBufferOverflow, QString("Serial port's '%1' read-buffer overflow").arg(serialPortName()));
                m_timestamp = QDateTime::currentMSecsSinceEpoch();
            }
            else if (QDateTime::currentMSecsSinceEpoch() - m_timestamp >= m_timeoutIB) // waiting timeout read next byte elapsed
            {
                m_state = STATE_BEGIN;
                QByteArray tmp = QByteArray(reinterpret_cast<char*>(m_buff), m_sz);
                Q_EMIT signalRx(tmp);
                //Q_EMIT signalRx(QByteArray(reinterpret_cast<char*>(m_buff), m_sz));
                return Status_Good;
            }
            return Status_Processing;
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

} // namespace Modbus
