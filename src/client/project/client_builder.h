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
#ifndef CLIENT_BUILDER_H
#define CLIENT_BUILDER_H

#include <project/core_builder.h>
#include <project/client_project.h>

class QIODevice;
class mbClientProject;
class mbClientPort;
class mbClientDevice;
class mbClientDomProject;
class mbClientDomPort;
class mbClientDomDevice;
class mbClientDomItem;

class mbClientBuilder : public mbCoreBuilder
{
public:
    struct Strings : public mbCoreBuilder::Strings
    {
        const QChar sep;
        //----------------
        Strings();
        static const Strings &instance();
    };

public:
    mbClientBuilder(QObject* parent = nullptr);

public:
    inline mbClientProject *project() const { return static_cast<mbClientProject*>(projectCore()); }

public:
    inline mbClientProject* load(const QString& file) { return reinterpret_cast<mbClientProject*>(loadCore(file)); }
    inline bool save(mbClientProject* project) { return saveCore(reinterpret_cast<mbCoreProject*>(project)); }

public:
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
    mbCoreProject *toProject(mbCoreDomProject *dom) override;
};

#endif // CLIENT_BUILDER_H
