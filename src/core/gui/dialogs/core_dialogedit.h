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
#ifndef CORE_DIALOGEDIT_H
#define CORE_DIALOGEDIT_H

#include "core_dialogbase.h"

class MB_EXPORT mbCoreDialogEdit : public mbCoreDialogBase
{
public:
    using mbCoreDialogBase::mbCoreDialogBase;

public:
    virtual MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) = 0;
};

#endif // CORE_DIALOGEDIT_H
