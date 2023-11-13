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
#include "core_taskinfo.h"

#include <mbcore_taskfactory.h>
#include "core_project.h"

const QString DEVICE_DEFAULT_NAME = QStringLiteral("task");

mbCoreTaskInfo::mbCoreTaskInfo(const QString &type, mbCoreTaskFactory *factory)
{
    m_type = type;
    m_project = nullptr;
    m_factory = factory;
}

void mbCoreTaskInfo::setProject(mbCoreProject *project)
{
    m_project = project;
    setName(name());
    if (project && (project->taskIndex(this) < 0))
        project->taskAdd(this);
}

void mbCoreTaskInfo::setName(const QString &name)
{
    QString tn = name.trimmed();
    if (tn.isEmpty())
        tn = DEVICE_DEFAULT_NAME;
    if (m_project && m_project->task(tn) != this)
    {
        if (m_project->hasTask(tn))
            tn = m_project->freeTaskName(tn);
        m_project->taskRename(this, tn);
    }
    m_name = (tn);
}

void mbCoreTaskInfo::setParamsString(const QString &params)
{
    if (m_factory)
    {
        m_params = m_factory->stringToParams(params);
        m_paramsString = m_factory->paramsToString(m_params);
    }
}

bool mbCoreTaskInfo::canConvertStringToParams() const
{
    return m_factory && m_factory->canConvertStringToParams();
}

bool mbCoreTaskInfo::getTaskParams(const QString &title)
{
    if (m_factory)
    {
        bool r = m_factory->getTaskParams(m_params, title);
        m_paramsString = m_factory->paramsToString(m_params);
        return r;
    }
    return false;
}

