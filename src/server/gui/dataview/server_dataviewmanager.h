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
#ifndef SERVER_DATAVIEWMANAGER_H
#define SERVER_DATAVIEWMANAGER_H

#include <core/gui/dataview/core_dataviewmanager.h>

class mbServerProject;
class mbServerDataView;
class mbServerDataViewUi;

class mbServerDataViewManager : public mbCoreDataViewManager
{
    Q_OBJECT

public:
    explicit mbServerDataViewManager(QObject *parent = nullptr);

public: // project
    inline mbServerProject *project() const { return reinterpret_cast<mbServerProject*>(projectCore()); }
    inline QList<mbServerDataViewUi*> dataViewUis() const { return QList<mbServerDataViewUi*>(*reinterpret_cast<const QList<mbServerDataViewUi*>*>(&m_dataViewUis)); }
    inline mbServerDataViewUi *activeDataViewUi() const { return reinterpret_cast<mbServerDataViewUi*>(activeDataViewUiCore()); }
    inline mbServerDataView *activeDataView() const { return reinterpret_cast<mbServerDataView*>(activeDataViewCore()); }

public: // dataView ui
    inline mbServerDataViewUi *dataViewUi(mbServerDataView *dataView) const { return reinterpret_cast<mbServerDataViewUi*>(dataViewUiCore(reinterpret_cast<mbCoreDataView*>(dataView))); }

private:
    mbCoreDataViewUi *createDataViewUi(mbCoreDataView *dataView) override;
};

#endif // SERVER_DATAVIEWMANAGER_H
