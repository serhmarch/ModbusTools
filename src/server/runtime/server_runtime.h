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
#ifndef SERVER_RUNTIME_H
#define SERVER_RUNTIME_H

#include <server_global.h>
#include <project/server_project.h>
#include <runtime/core_runtime.h>

class mbServerProject;
class mbServerPort;
class mbServerDevice;
class mbServerRunThread;
class mbServerRunScriptThread;

class mbServerRuntime : public mbCoreRuntime
{
public:
    explicit mbServerRuntime(QObject *parent = nullptr);

public:
    inline mbServerProject *project() const { return static_cast<mbServerProject*>(projectCore()); }

public:
    bool isRunning();
    void start();
    void stop();

private:
    void createComponents() override;
    void startComponents() override;
    void beginStopComponents() override;
    bool tryStopComponents() override;
    void clearComponents() override;

private:
    mbServerRunThread *createRunThread(mbServerPort *port);
    mbServerRunScriptThread *createScriptThread(mbServerDevice *device);

private:
    mbServerProject *m_project;

private: // threads
    typedef QHash<mbServerPort*, mbServerRunThread*> Threads_t;
    Threads_t m_threads;

    typedef QHash<mbServerDevice*, mbServerRunScriptThread*> ScriptThreads_t;
    ScriptThreads_t m_scriptThreads;
};

#endif // SERVER_RUNTIME_H
