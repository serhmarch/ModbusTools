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
#ifndef SERVER_DEVICEREF_H
#define SERVER_DEVICEREF_H

#include <QObject>

#include <server_global.h>

#include "server_device.h"

class mbServerPort;

class mbServerDeviceRef : public QObject
{
    Q_OBJECT
public:
    struct Strings : public mbServerDevice::Strings
    {
        const QString units;

        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbServerDevice::Defaults
    {
        const QString units;

        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbServerDeviceRef(mbServerDevice *device, QObject *parent = nullptr);

public:
    inline QString name() const { return m_device->name(); }
    inline mbServerDevice *device() const { return m_device; }
    inline mbServerPort *port() const { return m_port; }
    void setPort(mbServerPort *port);

public:
    inline QList<quint8> units() const { return m_units; }
    void setUnits(const QList<quint8> &units);
    void addUnits(const QList<quint8> &units);
    QString unitsStr() const;
    void setUnitsStr(const QString &units);
    void addUnitsStr(const QString &units);

    Modbus::Settings settings() const;
    bool setSettings(const Modbus::Settings& settings);

public:
    void setName(const QString &name);

Q_SIGNALS:
    void nameChanged(const QString &name);
    void changed();

private:
    mbServerDevice *m_device;
    mbServerPort *m_port;
    QList<quint8> m_units;
};

#endif // SERVER_DEVICEREF_H
