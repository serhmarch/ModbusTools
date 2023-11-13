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
#ifndef CLIENT_WINDOWMANAGER_H
#define CLIENT_WINDOWMANAGER_H

#include <core/gui/core_windowmanager.h>

class mbClientDevice;
class mbClientDataView;

class mbClientUi;
class mbClientDataViewManager;
class mbClientDataViewUi;

class mbClientWindowManager : public mbCoreWindowManager
{
    Q_OBJECT
public:
    explicit mbClientWindowManager(mbClientUi *ui, mbClientDataViewManager *dataViewManager);

public:
    inline mbClientUi *ui() const { return reinterpret_cast<mbClientUi*>(uiCore()); }
    inline mbClientDataViewManager *dataViewManager() { return reinterpret_cast<mbClientDataViewManager*>(dataViewManagerCore()); }
    inline mbClientDataView *activeDataView() const { return reinterpret_cast<mbClientDataView*>(activeDataViewCore()); }
    inline void setActiveDataView(mbClientDataView *dataView) { setActiveDataViewCore(reinterpret_cast<mbCoreDataView*>(dataView)); }

private Q_SLOTS:
    void maximizeDataViewUi();
};

#endif // CLIENT_WINDOWMANAGER_H
