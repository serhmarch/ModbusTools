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
#ifndef MODBUSSERVERPORT_H
#define MODBUSSERVERPORT_H

#include "ModbusPort.h"

#define MBSLAVE_SZ_VALUE_BUFF MB_VALUE_BUFF_SZ

namespace Modbus {

class MODBUS_EXPORT ServerPort : public QObject
{
    Q_OBJECT
    
public:
    struct MODBUS_EXPORT Strings
    {
        const QString type;
        const QString name;
        const QString unit;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults
    {
        Modbus::Type type;
        const uint8_t unit;

        Defaults();
        static const Defaults &instance();
    };

    enum State
    {
        STATE_BEGIN                 = 0,
        STATE_UNKNOWN               = STATE_BEGIN,
        STATE_WAIT_FOR_OPEN         ,
        STATE_OPENED                ,
        STATE_BEGIN_READ            ,
        STATE_READ                  ,
        STATE_PROCESS_DEVICE        ,
        STATE_WRITE                 ,
        STATE_BEGIN_WRITE           ,
        STATE_WAIT_FOR_CLOSE        ,
        STATE_CLOSED                ,
        STATE_END                   = STATE_CLOSED
    };
    
    enum Status
    {
        Unknown,
        WaitForFinalize,
        Finalized,
        WaitForInitialize,
        Initialized
    };
    Q_ENUM(Status)

public:
    explicit ServerPort(Port *port, Interface *device, QObject* parent = nullptr);

public:
    inline Interface *device() const { return m_device; }

public:
    virtual Modbus::Type type() const;
    virtual StatusCode open();
    virtual StatusCode close();
    virtual bool isOpen() const;
    inline bool isStateClosed() const { return m_state == STATE_CLOSED; }
    // name
    inline QString name() const { return objectName(); }
    inline void setName(const QString& name) { setObjectName(name); }
    // error
    // state
    inline State state() const { return m_state; }
    // status
    Status status() const;
    // settings
    virtual Settings settings();
    virtual bool setSettings(const Settings &settings);

public:
    virtual StatusCode process();

Q_SIGNALS:
    void signalTx(const QString& source, const QByteArray& bytes);
    void signalRx(const QString& source, const QByteArray& bytes);
    void signalError(const QString& source, int code, const QString& message);
    void signalMessage(const QString& source, const QString& message);

protected Q_SLOTS:
    void slotTx(const QByteArray& bytes);
    void slotRx(const QByteArray& bytes);
    StatusCode setError(StatusCode status, const QString& message);
    void setMessage(const QString& message);

protected:
    virtual StatusCode processInputData(const uint8_t *buff, uint16_t sz);
    virtual StatusCode processDevice();
    virtual StatusCode processOutputData(uint8_t *buff, uint16_t &sz);

protected:
    State m_state;
    uint8_t m_unit;
    uint8_t m_func;
    uint16_t m_offset;
    uint16_t m_count;
    uint8_t m_valueBuff[MBSLAVE_SZ_VALUE_BUFF];
    bool m_cmdClose;
    Port *m_port;
    Interface *m_device;
};

} // namespace Modbus

#endif // MODBUSSERVERPORT_H

