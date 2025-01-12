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
#include "core.h"

#include <iostream>

#include <QApplication>
#include <QDateTime>

#include <Modbus.h>

#include "task/core_taskfactoryinfo.h"
#include "sdk/mbcore_taskfactory.h"
#include "core_filemanager.h"
#include "plugin/core_pluginmanager.h"
#include "project/core_project.h"
#include "project/core_builder.h"
//#include "project/core_taskinfo.h"
#include "gui/core_ui.h"
#include "runtime/core_runtime.h"

const int variantTypeId_LogFlag = qRegisterMetaType<mb::LogFlag>();

mbCore *mbCore::s_globalCore = nullptr;

void coreMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    mb::LogFlag flag;
    switch (type)
    {
    case QtDebugMsg:
        flag = mb::Log_QtDebug;
        break;
    case QtWarningMsg:
        flag = mb::Log_QtWarning;
        break;
    case QtCriticalMsg:
        flag = mb::Log_QtCritical;
        break;
    case QtFatalMsg:
        flag = mb::Log_QtFatal;
        break;
    case QtInfoMsg:
        flag = mb::Log_QtInfo;
        break;
    default:
        return;
    }
    QString text = QString("%1 (%2:%3, %4)\n")
                       .arg(msg, context.file)
                       .arg(context.line)
                       .arg(context.function);
    mbCore::LogMessage(flag, QStringLiteral("Qt"), text);
}

mbCore::Strings::Strings() :
    settings_organization   (QStringLiteral("ModbusTools"       )),
    settings_lastProject    (QStringLiteral("lastProject"       )),
    settings_logFlags       (QStringLiteral("Log.Flags"         )),
    settings_useTimestamp   (QStringLiteral("Log.UseTimestamp"  )),
    settings_formatDateTime (QStringLiteral("Log.FormatDateTime")),
    settings_addressNotation(QStringLiteral("AddressNotation"))
{
}

const mbCore::Strings &mbCore::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCore::Defaults::Defaults() :
    settings_logFlags       (mb::Log_Error|mb::Log_Warning|mb::Log_Info|mb::Log_Tx|mb::Log_Rx),
    settings_useTimestamp   (true),
    settings_formatDateTime (QStringLiteral("dd.MM.yyyy hh:mm:ss.zzz")),
    settings_addressNotation(mb::Address_Modbus),
    tray                    (false),
    availableBaudRate       (mb::availableBaudRate   ()),
    availableDataBits       (mb::availableDataBits   ()),
    availableParity         (mb::availableParity     ()),
    availableStopBits       (mb::availableStopBits   ()),
    availableFlowControl    (mb::availableFlowControl())
{
}

const mbCore::Defaults &mbCore::Defaults::instance()
{
    static const Defaults d;
    return d;
}

mbCore::mbCore(const QString &application, QObject *parent) : mbCoreBase(parent)
{
    s_globalCore = this;
    const Strings &s = Strings::instance();
    const Defaults &d = Defaults::instance();

    m_status = NoProject;
    m_fileManager = nullptr;
    m_pluginManager = nullptr;
    m_runtime = nullptr;
    m_ui = nullptr;
    m_project = nullptr;

    connect(this, &mbCore::signalLog   , this, &mbCore::logMessageThreadUnsafe);
    connect(this, &mbCore::signalOutput, this, &mbCore::outputMessageThreadUnsafe);

    m_settings.logFlags        = d.settings_logFlags       ;
    m_settings.useTimestamp    = d.settings_useTimestamp   ;
    m_settings.formatDateTime  = d.settings_formatDateTime ;
    m_settings.addressNotation = d.settings_addressNotation;
    m_config = new QSettings(s.settings_organization, application, this);
}

mbCore::~mbCore()
{
    delete m_ui;
    delete m_project;
    delete m_app;
}

void mbCore::setStatus(mbCore::Status status)
{
    m_status = status;
    Q_EMIT statusChanged(status);
}

void mbCore::setProjectCore(mbCoreProject *project)
{
    if (m_runtime->isRunning())
        return;
    if (m_project != project)
    {
        mbCoreProject *old = m_project;
        if (old)
        {
            QString absPath = old->absoluteFilePath();
            if (absPath.count())
                m_settings.lastProject = absPath;
        }
        m_project = project;
        Q_EMIT projectChanged(project);
        if (project)
            setStatus(Stopped);
        else
            setStatus(NoProject);
        delete old;
    }
}

int mbCore::exec(int argc, char **argv)
{
    m_shared.setKey(createGUID());
    int r;
    if ((r = parseArgs(argc, argv)))
        return r;
    //qInstallMessageHandler(coreMessageHandler);
    bool gui = m_args.value(Arg_Gui, true).toBool();
    m_fileManager = new mbCoreFileManager(this, this);
    m_pluginManager = createPluginManager();
    m_builder = createBuilder();
    m_runtime = createRuntime();
    if (gui)
        r = runGui();
    else
        r = runConsole();
    stop();
    return r;
}

QWidget* mbCore::topLevel() const
{
    return m_ui;
}

mbCorePluginManager* mbCore::createPluginManager()
{
    return new mbCorePluginManager(this, this);
}

void mbCore::loadProject()
{
    QString projectPath;
    if (m_args.contains(Arg_Project))
        projectPath = m_args.value(Arg_Project).toString();
    if (projectPath.isEmpty() && !m_settings.lastProject.isEmpty())
        projectPath = m_settings.lastProject;
    else
        return;
    QScopedPointer<mbCoreBuilder> b(createBuilder());
    if (mbCoreProject* p = b->loadCore(projectPath))
    {
        setProjectCore(p);
    }
}

int mbCore::parseArg(int /*argc*/, char ** /*argv*/, int & /*arg*/)
{
    return -1;
}

int mbCore::parseArgs(int &argc, char **argv)
{
    bool gui = true;
    for (int i = 1; i < argc; i++)
    {
        int r = parseArg(argc, argv, i);
        if (r)
        {
            if (r > 0)
                return r;
            // r == -1 means that's unknown argv[i]
            // and we can try proccess it
            if (!qstrcmp(argv[i], "-gui"))
            {
                gui = true;
                m_args[Arg_Gui] = gui;
                continue;
            }
            if (!qstrcmp(argv[i], "-no-gui"))
            {
                gui = false;
                m_args[Arg_Gui] = gui;
                continue;
            }
            if ((!qstrcmp(argv[i], "-project")) || (!qstrcmp(argv[i], "-p")))
            {
                if (++i < argc)
                    m_args[Arg_Project] = QString(argv[i]);
                continue;
            }
            if ((!qstrcmp(argv[i], "-singleton")) || (!qstrcmp(argv[i], "-s")))
            {
                m_args[Arg_Singleton] = true;
                continue;
            }
            if (!qstrcmp(argv[i], "-tray"))
            {
                m_args[Arg_Tray] = true;
                continue;
            }
            if (!qstrcmp(argv[i], "-no-tray"))
            {
                m_args[Arg_Tray] = false;
                continue;
            }
            std::cerr << "Unknown parameter " << argv[i];
            return 1;
        }
    }
    if (gui)
        m_app = new QApplication(argc, argv);
    else
        m_app = new QCoreApplication(argc, argv);
    if (!m_shared.create(1))
    {
        bool singleton = m_args.value(Arg_Singleton, false).toBool();
        if (m_shared.error() == QSharedMemory::AlreadyExists && singleton)
        {
            std::cerr << applicationName().toStdString() << " already running (singleton mode)";
            return 1;
        }
    }
    return 0;
}

int mbCore::runGui()
{
    m_ui = createUi();
    m_ui->initialize();
    loadCachedSettings();
    loadProject();
    if (!m_project)
    {
        mbCoreProject *p = createProject();
        p->setAuthor(mb::currentUser());
        p->setModified();
        setProjectCore(p);
    }
    m_ui->show();
    //qDebug("Test DEBUG");
    pluginManagerSync();
    int r = m_app->exec();
    saveCachedSettings();
    return r;
}

int mbCore::runConsole()
{
    loadCachedSettings();
    loadProject();
    int r = 1;
    if (m_project)
    {
        start();
        r = m_app->exec();
    }
    else
    {
        logMessageThreadUnsafe(mb::Log_Error, applicationName(), QStringLiteral("No project defined"));
    }
    saveCachedSettings();
    return r;
}

bool mbCore::isRunning()
{
    return m_runtime->isRunning();
}

void mbCore::start()
{
    if (m_runtime->isRunning() || !m_project)
        return;
    m_runtime->start();
    if (m_runtime->isRunning())
        setStatus(Running);
    else
        setStatus(Stopped);
}

void mbCore::stop()
{
    if (!m_runtime->isRunning() || m_status == Stopping)
        return;
    setStatus(Stopping);
    m_runtime->stop();
    setStatus(Stopped);
}

void mbCore::setAddressNotation(mb::AddressNotation notation)
{
    if (notation != mb::Address_IEC61131)
        notation = mb::Address_Modbus;
    if (m_settings.addressNotation != notation)
    {
        m_settings.addressNotation = notation;
        Q_EMIT addressNotationChanged(m_settings.addressNotation);
    }

}

MBSETTINGS mbCore::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    if (m_ui)
        r = m_ui->cachedSettings();
    QString absoluteFilePath;
    if (m_project)
        absoluteFilePath = m_project->absoluteFilePath();
    QString projectPath = absoluteFilePath.count() ? absoluteFilePath : m_settings.lastProject;
    r[s.settings_lastProject    ] = projectPath;
    r[s.settings_logFlags       ] = static_cast<uint>(logFlags());
    r[s.settings_useTimestamp   ] = useTimestamp  ();
    r[s.settings_formatDateTime ] = formatDateTime();
    r[s.settings_addressNotation] = mb::toString(addressNotation());
    return r;
}

void mbCore::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    bool ok;

    it = settings.find(s.settings_lastProject);
    if (it != end)
    {
        QString v = it.value().toString();
        m_settings.lastProject = v;
    }

    it = settings.find(s.settings_logFlags);
    if (it != end)
    {
        mb::LogFlag v = static_cast<mb::LogFlag>(it.value().toInt(&ok));
        if (ok)
            setLogFlags(v);
    }

    it = settings.find(s.settings_useTimestamp);
    if (it != end)
    {
        bool v = it.value().toBool();
        setUseTimestamp(v);
    }

    it = settings.find(s.settings_formatDateTime);
    if (it != end)
    {
        QString v = it.value().toString();
        setFormatDateTime(v);
    }

    it = settings.find(s.settings_addressNotation);
    if (it != end)
    {
        mb::AddressNotation v = mb::toAddressNotation(it.value());
        setAddressNotation(v);
    }

    if (m_ui)
        m_ui->setCachedSettings(settings);
}


void mbCore::logMessageThreadSafe(mb::LogFlag flag, const QString &source, const QString &text)
{
    if (thread() == QThread::currentThread())
        logMessageThreadUnsafe(flag, source, text);
    else
        Q_EMIT signalLog(flag, source, text);
}

void mbCore::outputMessageThreadSafe(const QString &text)
{
    if (thread() == QThread::currentThread())
        outputMessageThreadUnsafe(text);
    else
        Q_EMIT signalOutput(text);
}

void mbCore::logMessageThreadUnsafe(mb::LogFlag flag, const QString &source, const QString &text)
{
    if (m_ui)
        m_ui->logMessage(flag, source, text);
    else
    {
        QString msg = QString("%1 %2").arg(QDateTime::currentDateTime().toString(m_settings.formatDateTime), msg);
        std::cout << msg.toStdString();
    }
}

void mbCore::outputMessageThreadUnsafe(const QString &text)
{
    if (m_ui)
        m_ui->outputMessage(text);
    else
        std::cout << text.toStdString();
}

void mbCore::loadCachedSettings()
{
    QStringList keys = m_config->allKeys();
    MBSETTINGS settings;
    Q_FOREACH (const QString &key, keys)
    {
        QString name = key;
        settings.insert(name.replace('/', '.'), m_config->value(key));
    }

    this->setCachedSettings(settings);
}

void mbCore::saveCachedSettings()
{
    MBSETTINGS settings = this->cachedSettings();
    for (auto it = settings.begin(); it != settings.end(); it++)
    {
        QString key = it.key();
        m_config->setValue(key.replace('.', '/'), it.value());
    }
}

void mbCore::pluginManagerSync()
{
    //qDeleteAll(m_taskFactories);
    m_taskFactories.clear();
    m_hashTaskFactories.clear();
    m_pluginManager->updateRegisteredPlugins();
    QList<const mbCorePluginInfo*> plugins = m_pluginManager->plugins();
    Q_FOREACH (const mbCorePluginInfo* pluginInfo, plugins)
    {
        if (mbCoreTaskFactory* f = qobject_cast<mbCoreTaskFactory*>(pluginInfo->instance))
        {
            f->initialize(this);
            mbCoreTaskFactoryInfo* info = new mbCoreTaskFactoryInfo(f);
            QString name = pluginInfo->name;
            if (m_hashTaskFactories.contains(name))
            {
                int i = 0;
                QString n;
                do
                {
                    n = name;
                    name = name + QString::number(i);
                    i++;
                }
                while (m_hashTaskFactories.contains(n));
                name = n;
            }
            info->setName(name);
            info->setAbsoluteFilePath(pluginInfo->absoluteFilePath);
            m_taskFactories.append(info);
            m_hashTaskFactories.insert(name, info);
        }
    }
}
