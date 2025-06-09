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
#ifndef SERVER_DIALOGS_H
#define SERVER_DIALOGS_H

#include <gui/dialogs/core_dialogs.h>
#include <server_global.h>

class mbServer;
class mbServerDialogProject;
class mbServerDialogPort;
class mbServerDialogDevice;
class mbServerDialogDataView;
class mbServerDialogDataViewItem;
class mbServerDialogSimAction;
class mbServerDialogScriptModule;
class mbServerDialogFindReplace;

class mbServerDialogs : public mbCoreDialogs
{
public:
    mbServerDialogs(QWidget *parent = nullptr);
    ~mbServerDialogs();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    MBSETTINGS getSimAction(const MBSETTINGS &settings, const QString &title = QString());
    MBSETTINGS getScriptModule(const MBSETTINGS &settings, const QString &title = QString());
    void execFindReplace(bool replace = false);

private:
    mbServerDialogSimAction *m_simaction;
    mbServerDialogScriptModule *m_scriptModule;
    mbServerDialogFindReplace *m_findReplace;
};

#endif // SERVER_DIALOGS_H
