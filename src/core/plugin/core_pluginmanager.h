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
#ifndef CORE_PLUGINMANAGER_H
#define CORE_PLUGINMANAGER_H

#include <QObject>

#include <mbcore.h>

class mbCore;

struct mbCorePluginInfo
{
    QObject* instance;
    QString name;
    QString absoluteFilePath;
    
    mbCorePluginInfo()
    {
        instance = nullptr;
    }
};

class MB_EXPORT mbCorePluginManager: public QObject
{
    Q_OBJECT

public:
    static QStringList findPlugins(const QString &path);

public:
    explicit mbCorePluginManager(mbCore *core, QObject *parent = nullptr);

public:
    mbCore *baseCore() const;

public: // plugin paths
    void updateRegisteredPlugins();
    inline QStringList defaultPluginPaths() const { return m_defaultPluginPaths; }
    inline QStringList customPluginPaths() const { return m_customPluginPaths; }
    void addCustomPluginPath(const QString& path);
    void removeCustomPluginPath(const QString& path);
    void setCustomPluginPaths(const QStringList &paths);
    inline QStringList pluginPaths() const { return m_defaultPluginPaths+m_customPluginPaths; }

public: // registered plugins
    inline int count() const { return m_plugins.count(); }
    inline QStringList pluginNames() const { return m_hashPlugins.keys(); }
    inline const mbCorePluginInfo* plugin(int i) const { return m_plugins.value(i); }
    inline const mbCorePluginInfo* plugin(const QString &plugin) const { return m_hashPlugins.value(plugin); }
    QList<const mbCorePluginInfo*> plugins() const;
    QObjectList instances() const;
    inline QObject *instance(int i) const { if (mbCorePluginInfo* info = m_plugins.value(i)) return info->instance; return nullptr; }
    inline QObject *instance(const QString &plugin) const { if (mbCorePluginInfo* info = m_hashPlugins.value(plugin)) return info->instance; return nullptr; }
    QObject* instanceAtPath(const QString& absoluteFilePath) const;

protected:
    virtual bool filterPlugin(const QObject *instance) const;
    
private:
    void registerPath(const QString &path);
    void registerPlugin(const QString &plugin);

private:
    struct Item
    {
        QObject* instance;
        QString absolutePath;
    };
    typedef QList<mbCorePluginInfo*> Plugins_t;
    typedef QHash<QString, mbCorePluginInfo*> HashPlugins_t;
    
    mbCore *m_core;
    Plugins_t m_plugins;
    HashPlugins_t m_hashPlugins;
    QStringList m_defaultPluginPaths;
    QStringList m_customPluginPaths;
};

#endif // CORE_PLUGINMANAGER_H
