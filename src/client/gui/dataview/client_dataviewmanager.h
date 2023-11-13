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
#ifndef CLIENT_DATAVIEWMANAGER_H
#define CLIENT_DATAVIEWMANAGER_H

#include <core/gui/dataview/core_dataviewmanager.h>

class mbClientProject;
class mbClientDataView;
class mbClientDataViewUi;

class mbClientDataViewManager : public mbCoreDataViewManager
{
    Q_OBJECT

public:
    explicit mbClientDataViewManager(QObject *parent = nullptr);

public: // project
    inline mbClientProject *project() const { return reinterpret_cast<mbClientProject*>(projectCore()); }
    inline QList<mbClientDataViewUi*> dataViewUis() const { return QList<mbClientDataViewUi*>(*reinterpret_cast<const QList<mbClientDataViewUi*>*>(&m_dataViewUis)); }
    inline mbClientDataViewUi *activeDataViewUi() const { return reinterpret_cast<mbClientDataViewUi*>(activeDataViewUiCore()); }
    inline mbClientDataView *activeDataView() const { return reinterpret_cast<mbClientDataView*>(activeDataViewCore()); }

public: // dataView ui
    inline mbClientDataViewUi *dataViewUi(mbClientDataView *dataView) const { return reinterpret_cast<mbClientDataViewUi*>(dataViewUiCore(reinterpret_cast<mbCoreDataView*>(dataView))); }

protected:
    void setProject(mbCoreProject *project) override;
    mbCoreDataViewUi *createDataViewUi(mbCoreDataView *dataView) override;

Q_SIGNALS:
    void maximizeDataViewUi();
};

#endif // CLIENT_DATAVIEWMANAGER_H
