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
#include "core_pluginmanager.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSet>
#include <QPluginLoader>
#include <QLibrary>
#include <QLibraryInfo>
#include <QCoreApplication>

#include <core.h>

#include <mbcore_taskfactory.h>


QStringList mbCorePluginManager::findPlugins(const QString &path)
{
    const QDir dir(path);
    if (!dir.exists())
        return QStringList();

    const QFileInfoList infoList = dir.entryInfoList(QDir::Files);
    if (infoList.empty())
        return QStringList();

    // Load symbolic links but make sure all file names are unique as not
    // to fall for something like 'libplugin.so.1 -> libplugin.so'
    QStringList result;
    const QFileInfoList::const_iterator icend = infoList.constEnd();
    for (QFileInfoList::const_iterator it = infoList.constBegin(); it != icend; ++it)
    {
        QString fileName;
        if (it->isSymLink()) 
        {
            const QFileInfo linkTarget = QFileInfo(it->symLinkTarget());
            if (linkTarget.exists() && linkTarget.isFile())
                fileName = linkTarget.absoluteFilePath();
        } 
        else
            fileName = it->absoluteFilePath();

        if (!fileName.isEmpty() && QLibrary::isLibrary(fileName) && !result.contains(fileName))
            result += fileName;
    }
    return result;
}

mbCorePluginManager::mbCorePluginManager(mbCore* core, QObject* parent) : QObject(parent)
{
    m_core = core;

    QString execPath = QCoreApplication::applicationDirPath();
    execPath += QDir::separator();
    execPath += QStringLiteral("plugins");
    m_defaultPluginPaths.append(execPath);
}

void mbCorePluginManager::updateRegisteredPlugins()
{
    qDeleteAll(m_plugins);
    m_plugins.clear();
    m_hashPlugins.clear();
    Q_FOREACH (const QString &path, m_defaultPluginPaths)
        registerPath(path);
    Q_FOREACH (const QString &path, m_customPluginPaths)
        registerPath(path);
}

void mbCorePluginManager::addCustomPluginPath(const QString& /*path*/)
{
}

void mbCorePluginManager::removeCustomPluginPath(const QString& /*path*/)
{
}

void mbCorePluginManager::setCustomPluginPaths(const QStringList& /*paths*/)
{
}

QList<const mbCorePluginInfo *> mbCorePluginManager::plugins() const
{
    QList<const mbCorePluginInfo *> r;
    Q_FOREACH (const mbCorePluginInfo *info, m_plugins)
        r.append(info);
    return r;
}

QObjectList mbCorePluginManager::instances() const
{
    QObjectList r;
    Q_FOREACH (mbCorePluginInfo* info, m_plugins)
        r.append(info->instance);
    return r;
}

QObject *mbCorePluginManager::instanceAtPath(const QString &absoluteFilePath) const
{
    Q_FOREACH (const mbCorePluginInfo* info, m_plugins)
    {
        if (info->absoluteFilePath == absoluteFilePath)
            return info->instance;
    }
    return nullptr;
}

bool mbCorePluginManager::filterPlugin(const QObject* /*instance*/) const
{
    //return static_cast<bool>(qobject_cast<mbCoreTaskFactory*>(instance));
    return false;
}

void mbCorePluginManager::registerPath(const QString &path)
{
    QStringList candidates = findPlugins(path);

    Q_FOREACH (const QString &absoluteFilePath, candidates)
        registerPlugin(absoluteFilePath);
}

void mbCorePluginManager::registerPlugin(const QString &absoluteFilePath)
{
    if (instanceAtPath(absoluteFilePath))
        return;

    QLibrary lib(absoluteFilePath);
    //bool r = lib.load();
    lib.load();
    //QString e = lib.errorString();
    lib.unload();
    QPluginLoader loader(absoluteFilePath);
    QObject* o = loader.instance();
    if (o)
    {
        if (filterPlugin(o))
        {
            QFileInfo fi(absoluteFilePath);
            QString name = fi.baseName();
            if (m_hashPlugins.contains(name))
                return;
            mbCorePluginInfo* info = new mbCorePluginInfo();
            info->instance = o;
            info->name = name;
            info->absoluteFilePath = absoluteFilePath;
            m_plugins.append(info);
            m_hashPlugins.insert(name, info);
        }
    }
    else
    {
        m_core->logError(QStringLiteral("Plugin manager"), loader.errorString());
    }
}
