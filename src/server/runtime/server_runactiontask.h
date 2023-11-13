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
#ifndef SERVER_RUNACTIONTASK_H
#define SERVER_RUNACTIONTASK_H

#include <mbcore_task.h>

class mbServerAction;
class mbServerRunAction;

class mbServerRunActionTask : public mbCoreTask
{
public:
    explicit mbServerRunActionTask(QObject *parent = nullptr);
    ~mbServerRunActionTask();

public:
    void setActions(const QList<mbServerAction*> &actions);

public: // task interface
    virtual int init() override;
    virtual int loop() override;
    virtual int final() override;

private:
    typedef QList<mbServerRunAction*> Actions_t;

    Actions_t m_actions;
};

#endif // SERVER_RUNACTIONTASK_H
