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
#ifndef CORE_RUNTIME_H
#define CORE_RUNTIME_H

#include <QObject>

#include <mbcore_base.h>

class mbCoreProject;
class mbCoreRunTaskThread;

class MB_EXPORT mbCoreRuntime : public QObject
{
    Q_OBJECT
public:
    explicit mbCoreRuntime(QObject *parent = nullptr);

public:
    inline mbCoreProject *projectCore() const { return m_project; }

public:
    bool isRunning();
    void start();
    void stop();

Q_SIGNALS:

public:
    virtual void createComponents();
    virtual void startComponents();
    virtual void beginStopComponents();
    virtual bool tryStopComponents();
    virtual void clearComponents();

protected:
    mbCoreProject *m_project;

protected: //  task threads
    typedef QList<mbCoreRunTaskThread*> TaskThreads_t;
    TaskThreads_t m_taskThreads;
};

#endif // CORE_RUNTIME_H
