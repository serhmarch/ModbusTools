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
#ifndef SERVER_BUILDER_H
#define SERVER_BUILDER_H

#include <project/core_builder.h>
#include <project/server_project.h>

class QIODevice;

class mbServer;
class mbServerProject;
class mbServerPort;
class mbServerDevice;
class mbServerSimAction;
class mbServerDataView;
class mbServerDataViewItem;

class mbServerDomPort;
class mbServerDomDevice;
class mbServerDomSimAction;
class mbServerDomScriptModule;
class mbServerDomDataView;
class mbServerDomDataViewItem;

class mbServerBuilder : public mbCoreBuilder
{

public:
    struct Strings : public mbCoreBuilder::Strings
    {
        const QChar sep;
        QString scriptModuleName   ;
        QString scriptModuleAuthor ;
        QString scriptModuleComment;
        //----------------
        Strings();
        static const Strings &instance();
    };

    typedef QByteArray BoolData_t;
    typedef QByteArray UInt16Data_t;

public:
    mbServerBuilder(QObject *parent = nullptr);

public:
    inline mbServerProject *project() const { return static_cast<mbServerProject*>(projectCore()); }

public:
    inline mbServerProject* load(const QString& file) { return reinterpret_cast<mbServerProject*>(loadCore(file)); }
    inline bool save(mbServerProject* project) { return saveCore(reinterpret_cast<mbCoreProject*>(project)); }

public:
    QStringList csvSimActionAttributes() const;

public: // 'mbCoreBuilder'-interface
    mbCoreProject         *newProject        () const override;
    mbCorePort            *newPort           () const override;
    mbCoreDevice          *newDevice         () const override;
    mbCoreDataView        *newDataView       () const override;
    mbCoreDataViewItem    *newDataViewItem   () const override;

    mbCoreDomProject      *newDomProject     () const override;
    mbCoreDomPort         *newDomPort        () const override;
    mbCoreDomDevice       *newDomDevice      () const override;
    mbCoreDomDataView     *newDomDataView    () const override;
    mbCoreDomDataViewItem *newDomDataViewItem() const override;

public:
    mbServerSimAction    *newSimAction   () const;
    mbServerSimAction    *newSimAction   (mbServerSimAction *prev) const;
    mbServerDomSimAction *newDomSimAction() const;

public:
    mbServerScriptModule    *newScriptModule() const;
    mbServerDomScriptModule *newDomScriptModule() const;

public: // 'mbCoreBuilder'-interface
    void fillProject(mbCoreProject *obj, const mbCoreDomProject *dom) override;
    void fillPort   (mbCorePort    *obj, const mbCoreDomPort    *dom) override;
    void fillDevice (mbCoreDevice  *obj, const mbCoreDomDevice  *dom) override;

    void fillDomProject(mbCoreDomProject *dom, const mbCoreProject *obj) override;
    void fillDomPort   (mbCoreDomPort    *dom, const mbCorePort    *obj) override;
    void fillDomDevice (mbCoreDomDevice  *dom, const mbCoreDevice  *obj) override;

public:
    mbServerSimAction        *toSimAction (mbServerDomSimAction *dom);
    mbServerSimAction        *toSimAction (const MBSETTINGS &settings);
    QList<mbServerSimAction*> toSimActions(const QList<mbServerDomSimAction*> &dom);

    mbServerDomSimAction *toDomSimAction (mbServerSimAction    *cfg);
    MBSETTINGS toSettings(const mbServerSimAction *item);
    QList<mbServerDomSimAction*> toDomSimActions(const QList<mbServerSimAction*> &cfg);

public:
    mbServerScriptModule *toScriptModule(mbServerDomScriptModule *dom) const;
    void fillScriptModule(mbServerScriptModule *obj, const mbServerDomScriptModule *dom) const;

    mbServerDomScriptModule *toDomScriptModule(mbServerScriptModule *cfg) const;
    void fillDomScriptModule(mbServerDomScriptModule *dom, const mbServerScriptModule *obj) const;

    QList<mbServerDomScriptModule*> toDomScriptModules(const QList<mbServerScriptModule*> &cfg) const;

public:
    QList<mbServerSimAction*> importSimActions(const QString &file);
    QList<mbServerSimAction*> importSimActionsXml(const QString &file);
    QList<mbServerSimAction*> importSimActionsCsv(const QString &file);
    QList<mbServerSimAction*> importSimActionsXml(QIODevice *io);
    QList<mbServerSimAction*> importSimActionsCsv(QIODevice *io);

    bool exportSimActions(const QString &file, const QList<mbServerSimAction*> &cfg);
    bool exportSimActionsXml(const QString &file, const QList<mbServerSimAction*> &cfg);
    bool exportSimActionsCsv(const QString &file, const QList<mbServerSimAction*> &cfg);
    bool exportSimActionsXml(QIODevice *io, const QList<mbServerSimAction*> &cfg);
    bool exportSimActionsCsv(QIODevice *io, const QList<mbServerSimAction*> &cfg);

public:
    mbServerScriptModule* importScriptModule(const QString &file);
    mbServerScriptModule* importScriptModuleXml(const QString &file);
    mbServerScriptModule* importScriptModuleTxt(const QString &file);
    mbServerScriptModule* importScriptModuleXml(QIODevice *io);
    mbServerScriptModule* importScriptModuleTxt(QIODevice *io);

    bool exportScriptModule(const QString &file, const mbServerScriptModule* obj);
    bool exportScriptModuleXml(const QString &file, const mbServerScriptModule* obj);
    bool exportScriptModuleTxt(const QString &file, const mbServerScriptModule* obj);
    bool exportScriptModuleXml(QIODevice *io, const mbServerScriptModule* obj);
    bool exportScriptModuleTxt(QIODevice *io, const mbServerScriptModule* obj);

public:
    bool importBoolData(const QString& file, QByteArray &data, const QChar& sep = Strings::instance().sep);
    bool importBoolData(QIODevice* buff, QByteArray &data, const QChar& sep = Strings::instance().sep);
    bool exportBoolData(const QString& file, const QByteArray &data, int columns, const QChar& sep = Strings::instance().sep);
    bool exportBoolData(QIODevice* buff, const QByteArray &data, int columns, const QChar& sep = Strings::instance().sep);

    bool importUInt16Data(const QString& file, QByteArray &data, const QChar& sep = Strings::instance().sep);
    bool importUInt16Data(QIODevice* buff, QByteArray &data, const QChar& sep = Strings::instance().sep);
    bool exportUInt16Data(const QString& file, const QByteArray &data, int columns, const QChar& sep = Strings::instance().sep);
    bool exportUInt16Data(QIODevice* buff, const QByteArray &data, int columns, const QChar& sep = Strings::instance().sep);

private:
    void importDomProject(mbCoreDomProject *dom) override;
    BoolData_t toBoolData(const QString &str, int reserve = MB_MEMORY_MAX_COUNT);
    UInt16Data_t toUInt16Data(const QString &str, int reserve = MB_MEMORY_MAX_COUNT);
    QString fromBoolData(const BoolData_t &data);
    QString fromUInt16Data(const UInt16Data_t &data);

};

#endif // SERVER_BUILDER_H
