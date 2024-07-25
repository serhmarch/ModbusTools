/*
    Modbus Tools

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
#ifndef SERVER_PORTRUNNABLE_H
#define SERVER_PORTRUNNABLE_H

#include <QObject>

#include <ModbusQt.h>

class mbServerRunDevice;

class mbServerPortRunnable : public QObject
{
    Q_OBJECT
public:
    explicit mbServerPortRunnable(const Modbus::Settings &settings, mbServerRunDevice *device, QObject *parent = nullptr);
    ~mbServerPortRunnable();

public:
    inline QString name() const { return objectName(); }
    void setName(const QString &name);
    
public:
    void run();
    void close();

private Q_SLOTS:
    void slotBytesTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotBytesRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotAsciiTx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotAsciiRx(const Modbus::Char *source, const uint8_t* buff, uint16_t size);
    void slotError(const Modbus::Char *source, Modbus::StatusCode status, const Modbus::Char *text);
    void slotNewConnection(const Modbus::Char *source);
    void slotCloseConnection(const Modbus::Char *source);

private:
    ModbusServerPort *m_port;

private:
    mbServerRunDevice *m_device;
};

#endif // SERVER_PORTRUNNABLE_H
