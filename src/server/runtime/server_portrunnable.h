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

#include <Modbus.h>

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
    void slotBytesTx(const QString& source, const QByteArray &bytes);
    void slotBytesRx(const QString& source, const QByteArray &bytes);
    void slotAsciiTx(const QString& source, const QByteArray &bytes);
    void slotAsciiRx(const QString& source, const QByteArray &bytes);
    void slotError  (const QString& source, int code, const QString& message);
    void slotMessage(const QString& source, const QString& message);

private:
    Modbus::ServerPort *m_port;

private:
    mbServerRunDevice *m_device;
};

#endif // SERVER_PORTRUNNABLE_H
