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
#ifndef MODBUSCLIENTPORT_H
#define MODBUSCLIENTPORT_H

#include "ModbusPort.h"

namespace Modbus {

class Port;

class MODBUS_EXPORT ClientPort : public QObject
{
    Q_OBJECT

public:
    struct MODBUS_EXPORT Strings
    {
        const QString repeatCount;

        Strings();
        static const Strings& instance();
    };

    struct MODBUS_EXPORT Defaults
    {
        const uint32_t repeatCount;

        Defaults();
        static const Defaults& instance();
    };

public:
    enum RequestStatus
    {
        Enable,
        Disable,
        Process
    };

    enum State
    {
        STATE_BEGIN = 0,
        STATE_UNKNOWN = STATE_BEGIN,
        STATE_WAIT_FOR_OPEN,
        STATE_OPENED,
        STATE_BEGIN_WRITE,
        STATE_WRITE,
        STATE_BEGIN_READ,
        STATE_READ,
        STATE_WAIT_FOR_CLOSE,
        STATE_CLOSED,
        STATE_END = STATE_CLOSED
    };

public:
    ClientPort(Port *port, QObject *parent = nullptr);
    ~ClientPort();

public:
    Type type() const;
    StatusCode close();
    bool isOpen() const;
    uint32_t repeatCount() const { return m_settings.repeatCount; }
    void setRepeatCount(uint32_t v) { if (v > 0) m_settings.repeatCount = v; }
    Settings settings() const;
    bool setSettings(const Settings& settings);

public:
    inline Port *port() const { return m_port; }

public:
    inline StatusCode lastStatus() const { return m_lastStatus; }
    inline qint64 lastStatusTimestamp() const { return m_lastStatusTimestamp; }
    inline QString lastErrorText() const { return m_port->lastErrorText(); }

public:
    StatusCode request(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff, uint16_t maxSzBuff, uint16_t *szOutBuff);
    StatusCode process();

public:
    struct RequestParams;
    const void *currentClient() const;
    RequestParams *createRequestParams(void *obj, const QString &name);
    void renameRequestParams(RequestParams *rp, const QString &name);
    void deleteRequestParams(RequestParams *rp);
    RequestStatus getRequestStatus(RequestParams *rp);
    void cancelRequest(RequestParams* rp);

Q_SIGNALS:
    void signalTx(const QString& source, const QByteArray& bytes);
    void signalRx(const QString& source, const QByteArray& bytes);
    void signalError(const QString& source, StatusCode status, const QString& message);
    void signalMessage(const QString& source, const QString& message);

protected Q_SLOTS:
    void slotTx(const QByteArray& bytes);
    void slotRx(const QByteArray& bytes);
    StatusCode setError(StatusCode status, const QString& message);
    void setMessage(const QString& message);

protected:
    void setStatus(StatusCode s);

protected:
    Port *m_port;
    State m_state;
    RequestParams *m_currentRequestParams;
    uint32_t m_repeats;
    StatusCode m_lastStatus;
    qint64 m_lastStatusTimestamp;

    struct
    {
        uint32_t repeatCount;
    } m_settings;

};

} // namespace Modbus

#endif // MODBUSCLIENTPORT_H
