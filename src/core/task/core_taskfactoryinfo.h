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
#ifndef CORE_TASKFACTORYINFO_H
#define CORE_TASKFACTORYINFO_H

#include <QString>

class mbCore;
class mbCoreTaskFactory;

class mbCoreTaskFactoryInfo
{
public:
    mbCoreTaskFactoryInfo(mbCoreTaskFactory* taskFactory);
    ~mbCoreTaskFactoryInfo();
    
public:
    inline mbCoreTaskFactory* taskFactory() const { return m_taskFactory; }
    inline QString name() const { return m_name; }
    inline bool isPlugin() const { return m_isPlugin; }
    inline QString absoluteFilePath() const { return m_absoluteFilePath; }

private:
    inline void setName(const QString& name) { m_name = name; }
    inline void setIsPlugin(bool isPlugin) { m_isPlugin = isPlugin; }
    inline void setAbsoluteFilePath(const QString& absoluteFilePath) { m_absoluteFilePath = absoluteFilePath; }
    
private:
    friend class mbCore;

    mbCoreTaskFactory* m_taskFactory;
    QString m_name;
    QString m_absoluteFilePath;
    bool m_isPlugin;
};

#endif // CORE_TASKFACTORYINFO_H
