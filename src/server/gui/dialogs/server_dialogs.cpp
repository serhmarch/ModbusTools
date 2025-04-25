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
#include "server_dialogs.h"

#include <gui/dialogs/core_dialogprojectinfo.h>

#include <server.h>

#include "settings/server_dialogsettings.h"
#include "server_dialogport.h"
#include "server_dialogdevice.h"
#include "server_dialogdataviewitem.h"
#include "server_dialogsimaction.h"
#include "server_dialogscriptmodule.h".h"

mbServerDialogs::mbServerDialogs(QWidget *parent) : mbCoreDialogs (parent)
{
    m_projectFilter = QStringLiteral("Server Project (*.pjs)");
    m_settings = new mbServerDialogSettings(parent);
    m_port = new mbServerDialogPort(parent);
    m_device = new mbServerDialogDevice(parent);
    m_dataViewItem = new mbServerDialogDataViewItem(parent);
    m_simaction = new mbServerDialogSimAction(parent);
    m_scriptModule = new mbServerDialogScriptModule(parent);

    m_projectInfo->setProjectType(QStringLiteral("Server Project"));
}

mbServerDialogs::~mbServerDialogs()
{
}

MBSETTINGS mbServerDialogs::cachedSettings() const
{
    MBSETTINGS m = mbCoreDialogs::cachedSettings();
    mb::unite(m, m_simaction->cachedSettings());
    mb::unite(m, m_scriptModule->cachedSettings());
    return m;
}

void mbServerDialogs::setCachedSettings(const MBSETTINGS &settings)
{
    mbCoreDialogs::setCachedSettings(settings);
    m_simaction->setCachedSettings(settings);
    m_scriptModule->setCachedSettings(settings);
}

MBSETTINGS mbServerDialogs::getSimAction(const MBSETTINGS &settings, const QString &title)
{
    return m_simaction->getSettings(settings, title);
}

MBSETTINGS mbServerDialogs::getScriptModule(const MBSETTINGS &settings, const QString &title)
{
    return m_scriptModule->getSettings(settings, title);
}
