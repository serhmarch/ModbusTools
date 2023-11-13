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
#ifndef MODBUSPORT_H
#define MODBUSPORT_H

#include <QObject>

#include "Modbus.h"

namespace Modbus {

class MODBUS_EXPORT Port : public QObject
{
    Q_OBJECT

public:
    struct MODBUS_EXPORT Strings
    {
        const QString type;
        const QString server;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults
    {
        const Type type;

        Defaults();
        static const Defaults &instance();
    };

    enum State
    {
        STATE_UNKNOWN = 0,
        STATE_WAIT_FOR_OPEN,
        STATE_OPENED,
        STATE_BEGIN = STATE_OPENED,
        STATE_PREPARE_TO_READ,
        STATE_WAIT_FOR_READ,
        STATE_WAIT_FOR_READ_ALL,
        STATE_PREPARE_TO_WRITE,
        STATE_WAIT_FOR_WRITE,
        STATE_WAIT_FOR_WRITE_ALL,
        STATE_WAIT_FOR_CLOSE,
        STATE_CLOSED,
        STATE_END = STATE_CLOSED
    };

public:
    explicit Port(QObject *parent = nullptr);
    virtual ~Port();

public:
    virtual Type type() const = 0;
    virtual StatusCode open() = 0;
    virtual StatusCode close() = 0;
    virtual bool isOpen() const = 0;
    virtual Settings settings() const;
    virtual bool setSettings(const Settings& settings);
    virtual void setNextRequestRepeated(bool v);

public:
    inline bool isChanged() const { return m_changed; }
    inline bool isServerMode() const { return m_modeServer; }
    virtual void setServerMode(bool mode);

public: // errors
    inline QString lastErrorText() const { return m_lastErrorText; }

public:
    inline bool isWriteBufferBlocked() const { return m_block; }
    inline void freeWriteBuffer() { m_block = false; }
    virtual StatusCode writeBuffer(uint8_t unit, uint8_t func, uint8_t *buff, uint16_t szInBuff) = 0;
    virtual StatusCode readBuffer(uint8_t &unit, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) = 0;
    virtual StatusCode write() = 0;
    virtual StatusCode read() = 0;

Q_SIGNALS:
    void signalTx(const QByteArray& bytes);
    void signalRx(const QByteArray& bytes);
    void signalError(StatusCode status, const QString &text);
    void signalMessage(const QString &text);

protected:
    inline void setChanged(bool changed = true) { m_changed = changed; }
    inline void clearChanged() { setChanged(false); }
    inline StatusCode setError(StatusCode status, const QString &text) { m_lastErrorText = text; return status; }
    inline void setMessage(const QString &text) { Q_EMIT signalMessage(text); }

protected:
    State m_state;
    uint8_t m_unit;
    uint8_t m_func;
    QString m_lastErrorText;
    bool m_block;
    bool m_modeServer;
    bool m_changed;
};

} // namespace Modbus

#endif // MODBUSPORT_H
