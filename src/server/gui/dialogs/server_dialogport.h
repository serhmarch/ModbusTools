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
#ifndef SERVER_DIALOGPORT_H
#define SERVER_DIALOGPORT_H

#include <gui/dialogs/core_dialogport.h>

class mbServerPort;

namespace Ui {
class mbServerDialogPort;
}

class mbServerDialogPort : public mbCoreDialogPort
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString title;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogPort(QWidget *parent = nullptr);
    ~mbServerDialogPort();

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

private:
    void fillForm(const Modbus::Settings& params);
    void fillData(Modbus::Settings& params);

private Q_SLOTS:
    void setType(int type);

private:
    Ui::mbServerDialogPort *ui;
};

#endif // SERVER_DIALOGPORT_H
