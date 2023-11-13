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
#ifndef CORE_TASKINFO_H
#define CORE_TASKINFO_H

#include <QString>

#include <mbcore.h>

class mbCoreProject;
class mbCoreTaskFactory;

class MB_EXPORT mbCoreTaskInfo
{
public:
    mbCoreTaskInfo(const QString &type, mbCoreTaskFactory *factory);

public: 
    inline mbCoreTaskFactory *factory() const { return m_factory; }
    inline mbCoreProject *project() const { return m_project; }
    void setProject(mbCoreProject* project);
    inline QString name() const { return m_name; }
    void setName(const QString& name);
    inline QString type() const { return m_type; }
    inline MBSETTINGS params() const { return m_params; }
    inline void setParams(const MBSETTINGS& params) { m_params = params; }
    inline QString paramsString() const { return m_paramsString; }
    void setParamsString(const QString& params);
    bool canConvertStringToParams() const;
    bool getTaskParams(const QString& title = QString());

private:
    mbCoreTaskFactory *m_factory;
    mbCoreProject *m_project;
    QString m_name;
    QString m_type;
    MBSETTINGS m_params;
    QString m_paramsString;
};

#endif // CORE_TASKINFO_H
