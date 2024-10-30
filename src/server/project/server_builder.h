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
class mbServerAction;
class mbServerDataView;
class mbServerDataViewItem;

class mbServerDomPort;
class mbServerDomDevice;
class mbServerDomAction;
class mbServerDomAction;
class mbServerDomDataView;
class mbServerDomDataViewItem;

class mbServerBuilder : public mbCoreBuilder
{

public:
    struct Strings : public mbCoreBuilder::Strings
    {
        const QChar sep;
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
    QStringList csvActionAttributes() const;

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
    mbServerAction    *newAction   () const;
    mbServerAction    *newAction   (mbServerAction *prev) const;
    mbServerDomAction *newDomAction() const;

public: // 'mbCoreBuilder'-interface
    mbCoreProject     *toProject   (mbCoreDomProject *dom) override;
    mbCorePort        *toPort      (mbCoreDomPort    *dom) override;
    mbCoreDevice      *toDevice    (mbCoreDomDevice  *dom) override;

    mbCoreDomProject  *toDomProject(mbCoreProject    *cfg) override;
    mbCoreDomPort     *toDomPort   (mbCorePort       *cfg) override;
    mbCoreDomDevice   *toDomDevice (mbCoreDevice     *cfg) override;

public:
    mbServerAction        *toAction (mbServerDomAction *dom);
    mbServerAction        *toAction (const MBSETTINGS &settings);
    QList<mbServerAction*> toActions(const QList<mbServerDomAction*> &dom);

    mbServerDomAction *toDomAction (mbServerAction    *cfg);
    MBSETTINGS toSettings(const mbServerAction *item);
    QList<mbServerDomAction*> toDomActions(const QList<mbServerAction*> &cfg);

public:
    QList<mbServerAction*> importActions(const QString &file);
    QList<mbServerAction*> importActionsXml(const QString &file);
    QList<mbServerAction*> importActionsCsv(const QString &file);
    QList<mbServerAction*> importActionsXml(QIODevice *io);
    QList<mbServerAction*> importActionsCsv(QIODevice *io);

    bool exportActions(const QString &file, const QList<mbServerAction*> &cfg);
    bool exportActionsXml(const QString &file, const QList<mbServerAction*> &cfg);
    bool exportActionsCsv(const QString &file, const QList<mbServerAction*> &cfg);
    bool exportActionsXml(QIODevice *io, const QList<mbServerAction*> &cfg);
    bool exportActionsCsv(QIODevice *io, const QList<mbServerAction*> &cfg);

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
    BoolData_t toBoolData(const QString &str, int reserve = MB_MEMORY_MAX_COUNT);
    UInt16Data_t toUInt16Data(const QString &str, int reserve = MB_MEMORY_MAX_COUNT);
    QString fromBoolData(const BoolData_t &data);
    QString fromUInt16Data(const UInt16Data_t &data);

};

#endif // SERVER_BUILDER_H
