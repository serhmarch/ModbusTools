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
#ifndef CORE_H
#define CORE_H

#include <QThread>
#include <QSettings>
#include <QSharedMemory>

#include <mbcore_base.h>
#include "core_global.h"

class QCoreApplication;

class mbCoreTask;
class mbCoreTaskInfo;
class mbCoreTaskFactoryInfo;
class mbCoreFileManager;
class mbCorePluginManager;
class mbCoreUi;
class mbCoreProject;
class mbCoreBuilder;
class mbCoreRuntime;

Q_DECLARE_METATYPE(mb::LogFlag)

class MB_EXPORT mbCore : public mbCoreBase
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString settings_organization   ;
        const QString settings_lastProject    ;
        const QString settings_logFlags       ;
        const QString settings_useTimestamp   ;
        const QString settings_formatDateTime ;
        const QString settings_addressNotation;
        const QString settings_columns        ;
        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const mb::LogFlags        settings_logFlags       ;
        const bool                settings_useTimestamp   ;
        const QString             settings_formatDateTime ;
        const mb::AddressNotation settings_addressNotation;
        const bool                tray                    ;
        const QVariantList        availableBaudRate       ;
        const QVariantList        availableDataBits       ;
        const QVariantList        availableParity         ;
        const QVariantList        availableStopBits       ;
        const QVariantList        availableFlowControl    ;

        Defaults();
        static const Defaults &instance();
    };

    enum Args
    {
        Arg_Gui,
        Arg_Project,
        Arg_Singleton,
        Arg_Tray,
        ArgCount
    };

    enum Status
    {
        NoProject,
        Stopping,
        Stopped,
        Running
    };
    Q_ENUM(Status)

public:
    static inline mbCore* globalCore() { return s_globalCore; }
    static inline void LogMessage(mb::LogFlag flag, const QString &source, const QString &text) { s_globalCore->logMessage(flag, source, text); }
    static inline void LogError  (const QString &source, const QString &text) { s_globalCore->logError  (source, text); }
    static inline void LogWarning(const QString &source, const QString &text) { s_globalCore->logWarning(source, text); }
    static inline void LogInfo   (const QString &source, const QString &text) { s_globalCore->logInfo   (source, text); }
    static inline void LogTx     (const QString &source, const QString &text) { s_globalCore->logTx     (source, text); }
    static inline void LogRx     (const QString &source, const QString &text) { s_globalCore->logRx     (source, text); }
    static inline void LogDebug  (const QString &source, const QString &text) { s_globalCore->logDebug  (source, text); }
    static inline void OutputMessage(const QString &text) { s_globalCore->outputMessage(text); }

public:
    explicit mbCore(const QString& application, QObject *parent = nullptr);
    ~mbCore();

public:
    inline mbCoreUi* coreUi() const { return m_ui; }

public:
    inline Status status() const { return m_status; }
    void setStatus(Status status);
    inline QCoreApplication *application() const { return m_app; }
    inline QString applicationName() { return m_config->applicationName(); }
    inline MBPARAMS args() const { return m_args; }
    inline mbCoreBuilder *builderCore() const { return m_builder; }
    inline mbCoreProject *projectCore() const { return m_project; }
    void setProjectCore(mbCoreProject* project);
    inline mbCoreRuntime *coreRuntime() const { return m_runtime; }
    int exec(int argc, char** argv);
    bool isRunning();
    void start();
    void stop();

public:
    inline mb::LogFlags logFlags() const { return m_settings.logFlags; }
    inline void setLogFlags(mb::LogFlags logFlags) { m_settings.logFlags = logFlags; }
    inline bool useTimestamp() const { return m_settings.useTimestamp; }
    inline void setUseTimestamp(bool useTimestamp) { m_settings.useTimestamp = useTimestamp; }
    inline QString formatDateTime() const { return m_settings.formatDateTime; }
    inline void setFormatDateTime(const QString& formatDateTime) { m_settings.formatDateTime = formatDateTime; }
    inline mb::AddressNotation addressNotation() const { return m_settings.addressNotation; }
    void setAddressNotation(mb::AddressNotation notation);

    inline int columnCount() const { return m_settings.columns.count(); }
    inline QList<int> columns() const { return m_settings.columns; }
    void setColumns(const QList<int> columns);
    QStringList columnNames() const;
    void setColumnNames(const QStringList &columns);
    int columnTypeByIndex(int i) const;
    virtual int columnTypeByName(const QString &name) const;
    virtual QString columnNameByIndex(int i) const;
    int columnIndexByType(int type);

    virtual MBSETTINGS cachedSettings() const;
    virtual void setCachedSettings(const MBSETTINGS &settings);

public: // core base interface
    virtual QWidget* topLevel() const;

public: // log interface
    inline void logMessage(mb::LogFlag flag, const QString &source, const QString &text) { if (m_settings.logFlags & flag) logMessageThreadSafe(flag, source, text); }
    inline void logError  (const QString &source, const QString &text) { logMessage(mb::Log_Error  , source, text); }
    inline void logWarning(const QString &source, const QString &text) { logMessage(mb::Log_Warning, source, text); }
    inline void logInfo   (const QString &source, const QString &text) { logMessage(mb::Log_Info   , source, text); }
    inline void logTx     (const QString &source, const QString &text) { logMessage(mb::Log_Tx     , source, text); }
    inline void logRx     (const QString &source, const QString &text) { logMessage(mb::Log_Rx     , source, text); }
    inline void logDebug  (const QString &source, const QString &text) { logMessage(mb::Log_Debug  , source, text); }

public: // output
    inline void outputMessage(const QString &text) { outputMessageThreadSafe(text); }

public:
    inline mbCoreFileManager *fileManager() const { return m_fileManager; }
    inline mbCorePluginManager *pluginManager() const { return m_pluginManager; }

public: // task factory interface
    inline bool hasTaskFactory(const QString& name) const { return m_hashTaskFactories.contains(name); }
    inline int taskFactoryIndex(mbCoreTaskFactoryInfo* f) const { return m_taskFactories.indexOf(f); }
    inline int taskFactoryIndex(const QString& name) const { return taskFactoryIndex(taskFactory(name)); }
    inline mbCoreTaskFactoryInfo* taskFactory(int index) const { return m_taskFactories.value(index); }
    inline mbCoreTaskFactoryInfo* taskFactory(const QString& name) const { return m_hashTaskFactories.value(name); }
    inline mbCoreTaskFactoryInfo* taskFactoryAt(int index) const { return m_taskFactories.at(index); }
    inline int taskFactoryCount() const { return m_taskFactories.count(); }
    inline QList<mbCoreTaskFactoryInfo*> taskFactories() const { return m_taskFactories.toList(); }
    
Q_SIGNALS:
    void statusChanged(int status);
    void projectChanged(mbCoreProject* project);
    void addressNotationChanged(mb::AddressNotation addressNotation);
    void columnsChanged();

Q_SIGNALS:
    void signalLog(mb::LogFlag flag, const QString &source, const QString &text);
    void signalOutput(const QString &text);

public:
    virtual QString createGUID() = 0;
    virtual mbCoreProject* createProject() = 0;
    virtual QStringList availableDataViewColumns() const;

protected:
    virtual mbCoreBuilder* createBuilder() = 0;
    virtual mbCorePluginManager* createPluginManager();
    virtual mbCoreUi* createUi() = 0;
    virtual mbCoreRuntime* createRuntime() = 0;
    virtual void loadProject();

protected:
    virtual int parseArg(int c, char **argc, int& arg);
    virtual int parseArgs(int &argc, char **argv);
    virtual int runGui();
    virtual int runConsole();

private:
    void logMessageThreadSafe(mb::LogFlag flag, const QString &source, const QString &text);
    void outputMessageThreadSafe(const QString &text);

private Q_SLOTS:
    void logMessageThreadUnsafe(mb::LogFlag flag, const QString &source, const QString &text);
    void outputMessageThreadUnsafe(const QString &text);

private:
    void loadCachedSettings();
    void saveCachedSettings();
    void pluginManagerSync();

protected:
    static mbCore *s_globalCore;
    Status m_status;
    
    mbCoreFileManager *m_fileManager;
    mbCorePluginManager* m_pluginManager;
    mbCoreRuntime *m_runtime;
    mbCoreBuilder *m_builder;
    mbCoreProject *m_project;
    mbCoreUi *m_ui;
    QCoreApplication* m_app;
    QSettings* m_config;
    QSharedMemory m_shared;

protected:
    MBPARAMS m_args;
    struct
    {
        QString             lastProject    ;
        mb::LogFlags        logFlags       ;
        bool                useTimestamp   ;
        QString             formatDateTime ;
        mb::AddressNotation addressNotation;
        QList<int>          columns        ;
    } m_settings;

private:
    typedef QVector<mbCoreTaskFactoryInfo*> TaskFactories_t;
    typedef QHash<QString,mbCoreTaskFactoryInfo*> HashTaskFactories_t;
    
    TaskFactories_t m_taskFactories;
    HashTaskFactories_t m_hashTaskFactories;
};

#endif // CORE_H
