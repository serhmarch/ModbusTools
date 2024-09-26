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
#ifndef CORE_DIALOGSYSTEMSETTINGS_H
#define CORE_DIALOGSYSTEMSETTINGS_H

#include "core_dialogbase.h"

namespace Ui {
class mbCoreDialogSystemSettings;
}

class MB_EXPORT mbCoreDialogSystemSettings : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings : public mbCoreDialogBase::Strings
    {
        const QString title;
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreDialogSystemSettings(QWidget *parent = nullptr);
    ~mbCoreDialogSystemSettings();

public:
    bool editSystemSettings(const QString& title = QString());

private:
    void fillForm();
    void fillFormLogFlags(mb::LogFlags flags);
    void fillData();
    void fillDataLogFlags(mb::LogFlags &flags);

private:
    Ui::mbCoreDialogSystemSettings *ui;
    mb::LogFlags m_flags;
};

#endif // CORE_DIALOGSYSTEMSETTINGS_H
