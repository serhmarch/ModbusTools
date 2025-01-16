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
#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>
#include <QThread>
#include <QMutex>

#include <core.h>
#include <client_global.h>

class ModbusClient;
class mbClientItem;
class mbClientDevice;
class mbClientProject;
class mbClientUi;
class mbClientRuntime;

class mbClient : public mbCore
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString settings_application;
        const QString default_client;
        const QString default_conf_file;
        const QString GUID;
        Strings();
        static const Strings &instance();
    };

public:
    static inline mbClient* global() { return static_cast<mbClient*>(globalCore()); }

public:
    mbClient();
    ~mbClient();

public:
    inline mbClientUi* ui() const { return reinterpret_cast<mbClientUi*>(coreUi()); }
    inline mbClientProject* project() const { return reinterpret_cast<mbClientProject*>(projectCore()); }
    inline mbClientRuntime* runtime() const { return reinterpret_cast<mbClientRuntime*>(coreRuntime()); }
    inline void setProject(mbClientProject* project) { setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public:
    int columnTypeByName(const QString &name) const override;
    QString columnNameByIndex(int i) const override;

public:
    void sendMessage(mb::Client::DeviceHandle_t handle, const mbClientRunMessagePtr &message);
    void updateItem(mb::Client::ItemHandle_t handle, const QByteArray &data, Modbus::StatusCode status, mb::Timestamp_t timestamp);
    void writeItemData(mb::Client::ItemHandle_t handle, const QByteArray &data);

private:
    QString createGUID() override;
    mbCoreUi *createUi() override;
    mbCoreProject *createProject() override;
    mbCoreBuilder *createBuilder() override;
    mbCoreRuntime *createRuntime() override;
    QStringList availableDataViewColumns() const override;
};


#endif // CLIENT_H
