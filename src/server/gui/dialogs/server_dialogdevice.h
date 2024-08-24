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
#ifndef SERVER_DIALOGDEVICE_H
#define SERVER_DIALOGDEVICE_H

#include <gui/dialogs/core_dialogdevice.h>

namespace Ui {
class mbServerDialogDevice;
}

class mbServerDialogDevice : public mbCoreDialogDevice
{
    Q_OBJECT

public:
    enum Mode
    {
        EditDevice,
        EditDeviceRef,
        ShowDevices
    };

    struct Strings : public mbCoreDialogDevice::Strings
    {
        const QString mode;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogDevice(QWidget *parent = nullptr);
    ~mbServerDialogDevice();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &m) override;

private:
    void fillForm(const MBSETTINGS& settings) override;
    void fillData(MBSETTINGS& settings) const override;
    void fillFormShowDevices(const MBSETTINGS& settings);
    void fillDataShowDevices(MBSETTINGS& settings) const;
    void fillFormDevice(const MBSETTINGS& settings);
    void fillDataDevice(MBSETTINGS& settings) const;

private:
    void setEditEnabled(bool enabled);

private Q_SLOTS:
    void setCurrentDevice(int i);

private:
    Ui::mbServerDialogDevice *ui;
    Mode m_mode;
};

#endif // SERVER_DIALOGDEVICE_H
