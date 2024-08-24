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
#ifndef CLIENT_DIALOGDEVICE_H
#define CLIENT_DIALOGDEVICE_H

#include <gui/dialogs/core_dialogdevice.h>

namespace Ui {
class mbClientDialogDevice;
}

class mbClientDialogDevice : public mbCoreDialogDevice
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogDevice::Strings
    {
        const QString createDeviceForPort;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientDialogDevice(QWidget *parent = nullptr);
    ~mbClientDialogDevice();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &m) override;

private:
    void fillForm(const MBSETTINGS& params) override;
    void fillData(MBSETTINGS& params) const override;

private:
    void fillPortNames();
    void setPortName(const QString &portName);
    void fillPortForm(const Modbus::Settings& params);
    void fillPortData(Modbus::Settings& params) const;

private Q_SLOTS:
    void setPort(int port);
    void setPortType(int type);
    void setPortEnable(bool enable);

private:
    Ui::mbClientDialogDevice *ui;
};

#endif // CLIENT_DIALOGDEVICE_H
