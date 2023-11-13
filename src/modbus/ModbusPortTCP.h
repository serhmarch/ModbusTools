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
#ifndef MODBUSPORTTCP_H
#define MODBUSPORTTCP_H

#include "ModbusPort.h"

class QTcpSocket;

#define MBCLIENTTCP_BUFF_SZ MB_TCP_IO_BUFF_SZ

namespace Modbus {

class MODBUS_EXPORT PortTCP : public Port
{
public:
    struct MODBUS_EXPORT Strings : public Port::Strings
    {
        const QString host;
        const QString port;
        const QString timeout;

        Strings();
        static const Strings &instance();
    };

    struct MODBUS_EXPORT Defaults : public Port::Defaults
    {
        const QString host;
        const uint16_t port;
        const uint32_t timeout;

        Defaults();
        static const Defaults &instance();
    };

public:
    PortTCP(QTcpSocket* socket, QObject* parent = nullptr);
    PortTCP(QObject* parent = nullptr);
    ~PortTCP();

public:
    Type type() const  override { return TCP; }
    StatusCode open() override;
    StatusCode close() override;
    bool isOpen() const override;

public:
    inline QTcpSocket* socket() const { return m_socket; }
    inline QString host() const { return m_host; }
    void setHost(const QString &host);
    inline uint16_t port() const { return m_port; }
    void setPort(uint16_t port);
    inline uint32_t timeout() const { return m_timeout; }
    inline void setTimeout(uint32_t timeout) { m_timeout = timeout; }
    // settings
    Settings settings() const override;
    bool setSettings(const Settings &settings) override;
    void setNextRequestRepeated(bool v) override;
    // autoincrement transaction id
    inline bool autoIncrement() const { return m_autoIncrement; }

protected:
    StatusCode write() override;
    StatusCode read() override;
    StatusCode writeBuffer(uint8_t slave, uint8_t func, uint8_t *buff, uint16_t szInBuff) override;
    StatusCode readBuffer(uint8_t &slave, uint8_t &func, uint8_t *buff, uint16_t maxSzBuff, uint16_t *szOutBuff) override;

private:
    QTcpSocket *m_socket;
    QString m_host;
    uint16_t m_port;
    uint16_t m_transaction;
    bool m_autoIncrement;
    uint32_t m_timeout;
    qint64 m_timestamp;
    uint8_t m_buff[MBCLIENTTCP_BUFF_SZ];
    uint16_t m_sz;
    uint16_t m_packetSz;
};

} // namespace Modbus

#endif // MODBUSPORTTCP_H
