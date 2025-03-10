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
#ifndef CORE_DIALOGPROJECT_H
#define CORE_DIALOGPROJECT_H

#include "core_dialogedit.h"

namespace Ui {
class mbCoreDialogProject;
}

class mbCoreProject;

class MB_EXPORT mbCoreDialogProject : public mbCoreDialogEdit
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings : public mbCoreDialogEdit::Strings
    {
        const QString title;
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogProject(QWidget *parent = nullptr);
    ~mbCoreDialogProject();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings, const QString &title = QString()) override;

private:
    void fillForm(const MBSETTINGS &settings);
    void fillData(MBSETTINGS &settings);

private:
    Ui::mbCoreDialogProject *ui;
};

#endif // CORE_DIALOGPROJECT_H
