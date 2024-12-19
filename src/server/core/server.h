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
#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QMutexLocker>

#include <Modbus.h>

#include <core.h>
#include <server_global.h>

class ModbusSlave;
class mbServerProject;
class mbServerUi;
class mbServerRuntime;

class mbServer : public mbCore
{
    Q_OBJECT

public:
    struct Strings : mbCore::Strings
    {
        const QString GUID;
        const QString settings_application;
        const QString default_server;

        const QString settings_scriptEnable ;
        const QString settings_scriptManual ;
        const QString settings_scriptDefault;
        Strings();
        static const Strings &instance();
    };

public:
    static inline mbServer* global() { return static_cast<mbServer*>(globalCore()); }
    static QStringList findPythonExecutables();

public:
    mbServer();
    ~mbServer();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    inline mbServerUi* ui() const { return reinterpret_cast<mbServerUi*>(coreUi()); }
    inline mbServerProject* project() const { return reinterpret_cast<mbServerProject*>(projectCore()); }
    inline mbServerRuntime* runtime() const { return reinterpret_cast<mbServerRuntime*>(coreRuntime()); }
    inline void setProject(mbServerProject* project) { setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public:
    inline bool scriptEnable() const { return m_scriptEnable; }
    inline void setScriptEnable(bool use) { m_scriptEnable = use; }
    inline QStringList scriptAutoDetectedExecutables() const { return m_autoDetectedExec; }
    inline QStringList scriptManualExecutables() const { return m_manualExec; }
    inline void scriptSetManualExecutables(const QStringList &exec) { m_manualExec = exec; }
    inline void scriptAddExecutable(const QString exec) { m_manualExec.append(exec); }
    QString scriptDefaultExecutable() const;
    void scriptSetDefaultExecutable(const QString exec);

private:
    QString createGUID() override;
    mbCoreUi* createUi() override;
    mbCoreProject* createProject() override;
    mbCoreBuilder* createBuilder() override;
    mbCoreRuntime *createRuntime() override;

private:
    bool m_scriptEnable;
    QStringList m_autoDetectedExec;
    QStringList m_manualExec;
    mutable QString m_defaultExec;
};

#endif // SERVER_H
