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
#ifndef SERVER_DIALOGACTION_H
#define SERVER_DIALOGACTION_H

#include <QDialog>

#include <mbcore.h>
#include <project/server_action.h>

class mbServerDevice;

namespace Ui {
class mbServerDialogAction;
}

class mbServerDialogAction : public QDialog
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString title;
        const QString count;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogAction(QWidget *parent = nullptr);
    ~mbServerDialogAction();

public:
    MBSETTINGS getSettings(const MBSETTINGS &data, const QString &title = QString());

private:
    void fillForm(const MBSETTINGS &settings);
    void fillFormActionType(const MBSETTINGS &settings);
    void fillFormByteOrder(mb::DataOrder e);
    void fillFormRegisterOrder(mb::DataOrder e);

    void fillData(MBSETTINGS &settings);
    void fillDataActionType(MBSETTINGS &settings);
    void fillDataByteOrder(MBSETTINGS &settings);
    void fillDataRegisterOrder(MBSETTINGS &settings);

private Q_SLOTS:
    void setActionType(int i);

private:
    Ui::mbServerDialogAction *ui;
};

#endif // SERVER_DIALOGACTION_H
