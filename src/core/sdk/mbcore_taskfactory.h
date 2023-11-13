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
#ifndef MBCORE_TASKFACTORY_H
#define MBCORE_TASKFACTORY_H

#include <QObject>

#include "mbcore.h"

class mbCoreBase;
class mbCoreTask;

class mbCoreTaskFactory
{
public:
    virtual ~mbCoreTaskFactory() {}

public:
    virtual void initialize(mbCoreBase *core) = 0;
    virtual mbCoreTask *createTask(const MBSETTINGS& params, QObject *parent = nullptr) = 0;
    virtual bool getTaskParams(MBSETTINGS &params, const QString &title = QString()) = 0;
    virtual QString paramsToString(const MBSETTINGS &params) const = 0;
    virtual MBSETTINGS stringToParams(const QString &paramsString) const = 0;
    virtual bool canConvertStringToParams() const = 0;
};
Q_DECLARE_INTERFACE(mbCoreTaskFactory, "mbtools.Core.TaskFactory.Interface")

#endif // MBCORE_TASKFACTORY_H
