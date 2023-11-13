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
#ifndef SERVER_DATAVIEWUI_H
#define SERVER_DATAVIEWUI_H

#include <core/gui/dataview/core_dataviewui.h>

class QTableView;
class mbServerDataView;
class mbServerDataViewItem;
class mbServerDataViewModel;
class mbServerDataViewDelegate;

class mbServerDataViewUi : public mbCoreDataViewUi
{
    Q_OBJECT
public:
    mbServerDataViewUi(mbServerDataView *dataView, QWidget *parent = nullptr);

public: // QWidget

public:
    inline bool isScanning() const { return m_timerId > 0; }
    inline mbServerDataView *dataView() const { return reinterpret_cast<mbServerDataView *>(dataViewCore()); }
    inline mbServerDataViewModel *model() const { return reinterpret_cast<mbServerDataViewModel*>(modelCore()); }
    QList<mbServerDataViewItem*> selectedItems() const;

protected Q_SLOTS:
    void changePeriod(int period);

private:
    bool event(QEvent *event) override;

private:
    void startScanning(int period);
    void stopScanning();

private:
    int m_timerId;
};

#endif // SERVER_DATAVIEWUI_H
