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
#include "server_project.h"

#include "server_simaction.h"
#include "server_scriptmodule.h"

mbServerProject::mbServerProject(QObject *parent) :
    mbCoreProject(parent)
{
}

mbServerProject::~mbServerProject()
{
    qDeleteAll(m_simActions);
}

int mbServerProject::simActionInsert(mbServerSimAction *simAction, int index)
{
    if (!hasSimAction(simAction))
    {
        if ((index >= 0) && (index < m_simActions.count()))
            m_simActions.insert(index, simAction);
        else
        {
            index = m_simActions.count();
            m_simActions.append(simAction);
        }
        Q_EMIT simActionAdded(simAction);
        connect(simAction, &mbServerSimAction::changed, this, &mbServerProject::slotSimActionChanged);
        return index;
    }
    return -1;
}

void mbServerProject::simActionsInsert(const QList<mbServerSimAction *> &simActions, int index)
{
    if (index < 0 || index >= simActionCount())
    {
        Q_FOREACH (mbServerSimAction *simAction, simActions)
            simActionAdd(simAction);
    }
    else
    {
        Q_FOREACH (mbServerSimAction *simAction, simActions)
            simActionInsert(simAction, index++);
    }
}

void mbServerProject::simActionsRemove(const QList<mbServerSimAction *> &simActions)
{
    Q_FOREACH (mbServerSimAction *simAction, simActions)
        simActionRemove(simAction);
}

int mbServerProject::simActionRemove(int index)
{
    if ((index >= 0) && (index < simActionCount()))
    {
        mbServerSimAction *simAction = simActionAt(index);
        Q_EMIT simActionRemoving(simAction);
        simAction->disconnect(this);
        m_simActions.removeAt(index);
        Q_EMIT simActionRemoved(simAction);
        return index;
    }
    return -1;
}

QString mbServerProject::freeScriptModuleName(const QString &s) const
{
    QString ret = s.trimmed();
    if (ret.isEmpty())
        ret = mbServerScriptModule::Defaults::instance().name;
    QRegExp re("^(.*)(\\d+)$");
    QString tn = ret;
    if (ret.contains(re))
        tn = re.cap(1);
    int c = 1;

    while (hasScriptModule(ret))
    {
        ret = tn + QString::number(c);
        c++;
    }
    return ret;
}

int mbServerProject::scriptModuleInsert(mbServerScriptModule *scriptModule, int index)
{
    if (!hasScriptModule(scriptModule))
    {
        QString scriptModuleName = scriptModule->name();
        if (scriptModuleName.isEmpty() || hasScriptModule(scriptModuleName))
            scriptModule->setName(freeScriptModuleName(scriptModuleName));
        if ((index >= 0) && (index < m_scriptModules.count()))
            m_scriptModules.insert(index, scriptModule);
        else
        {
            index = m_scriptModules.count();
            m_scriptModules.append(scriptModule);
        }
        m_hashScriptModules.insert(scriptModule->name(), scriptModule);
        //scriptModule->setProjectCore(this);
        Q_EMIT scriptModuleAdded(scriptModule);
        connect(scriptModule, &mbServerScriptModule::changed, this, &mbServerProject::slotScriptModuleChanged);
        return index;
    }
    return -1;
}

int mbServerProject::scriptModuleRemove(int index)
{
    if ((index >= 0) && (index < scriptModuleCount()))
    {
        mbServerScriptModule* d = scriptModuleAt(index);
        Q_EMIT scriptModuleRemoving(d);
        m_hashScriptModules.remove(d->name());
        m_scriptModules.removeAt(index);
        //d->setProjectCore(nullptr);
        Q_EMIT scriptModuleRemoved(d);
        return index;
    }
    return -1;
}

bool mbServerProject::scriptModuleRename(mbServerScriptModule *d, const QString &newName)
{
    if (scriptModule(d->name()) == d && !scriptModule(newName))
    {
        //Q_EMIT scriptModuleRenaming(d, newName);
        m_hashScriptModules.remove(d->name());
        m_hashScriptModules.insert(newName, d);
        return true;
    }
    return false;
}

void mbServerProject::slotSimActionChanged()
{
    mbServerSimAction *simAction = static_cast<mbServerSimAction*>(sender());
    Q_EMIT simActionChanged(simAction);
}

void mbServerProject::slotScriptModuleChanged()
{
    mbServerScriptModule *scriptModule = static_cast<mbServerScriptModule*>(sender());
    Q_EMIT scriptModuleChanged(scriptModule);
}

