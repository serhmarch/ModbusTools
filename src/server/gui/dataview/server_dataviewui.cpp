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
#include "server_dataviewui.h"

#include <QTimerEvent>

#include <project/server_project.h>
#include <project/server_device.h>
#include <project/server_dataview.h>

#include "server_dataviewmodel.h"
#include "server_dataviewdelegate.h"

mbServerDataViewUi::mbServerDataViewUi(mbServerDataView *dataView, QWidget *parent) :
    mbCoreDataViewUi(dataView, new mbServerDataViewModel(dataView), new mbServerDataViewDelegate(), parent)
{
    m_timerId = 0;
    connect(dataView, &mbServerDataView::periodChanged, this, &mbServerDataViewUi::changePeriod);

}

void mbServerDataViewUi::changePeriod(int period)
{
    if (isVisible())
    {
        stopScanning();
        startScanning(period);
    }
}

bool mbServerDataViewUi::event(QEvent *event)
{
    if (event->type() == QEvent::Show)
    {
        startScanning(m_dataView->period());
    }
    else if (event->type() == QEvent::Hide)
    {
        stopScanning();
    }
    else if (event->type() == QEvent::Timer)
    {
        model()->refreshValues();
    }
    return QWidget::event(event);
}

void mbServerDataViewUi::startScanning(int period)
{
    if (!isScanning())
    {
        m_timerId = startTimer(period);
    }
}

void mbServerDataViewUi::stopScanning()
{
    if (isScanning())
    {
        killTimer(m_timerId);
        m_timerId = 0;
    }
}

QList<mbServerDataViewItem *> mbServerDataViewUi::selectedItems() const
{
    QList<mbCoreDataViewItem*> ls = selectedItemsCore();
    return QList<mbServerDataViewItem*>(*(reinterpret_cast<const QList<mbServerDataViewItem*>*>(&ls)));
}

