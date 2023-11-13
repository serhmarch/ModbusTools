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
#include "core_windowmanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include "core_ui.h"
#include "dataview/core_dataviewmanager.h"
#include "dataview/core_dataviewui.h"

mbCoreWindowManager::mbCoreWindowManager(mbCoreUi *ui, mbCoreDataViewManager *dataViewManager) :
    QObject(ui)
{
    m_ui = ui;
    m_dataViewManager = dataViewManager;

    m_area = new QMdiArea(ui);
    m_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_area->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    connect(m_area, &QMdiArea::subWindowActivated, this, &mbCoreWindowManager::subWindowActivated);

    connect(m_dataViewManager, &mbCoreDataViewManager::dataViewUiAdd, this, &mbCoreWindowManager::dataViewUiAdd);
    connect(m_dataViewManager, &mbCoreDataViewManager::dataViewUiRemove, this, &mbCoreWindowManager::dataViewUiRemove);
    Q_FOREACH (mbCoreDataViewUi *ui, m_dataViewManager->dataViewUisCore())
        dataViewUiAdd(ui);
}

mbCoreDataView *mbCoreWindowManager::activeDataViewCore() const
{
    return m_dataViewManager->activeDataViewCore();
}

void mbCoreWindowManager::setActiveDataViewCore(mbCoreDataView *dataView)
{
    mbCoreDataViewUi *ui = m_dataViewManager->dataViewUiCore(dataView);
    if (ui)
    {
        QMdiSubWindow *sw = m_hashWindows.value(ui);
        if (sw)
            m_area->setActiveSubWindow(sw);
    }
}

void mbCoreWindowManager::showDataViewUi(const mbCoreDataViewUi *ui)
{
    showSubWindow(ui);
}

void mbCoreWindowManager::actionWindowDataViewShowAll()
{
    Q_FOREACH(QMdiSubWindow *sw, m_dataViews)
    {
        sw->show();
        sw->widget()->show();
    }
}

void mbCoreWindowManager::actionWindowDataViewShowActive()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
        showSubWindow(ui);
}

void mbCoreWindowManager::actionWindowDataViewCloseAll()
{
    Q_FOREACH(QMdiSubWindow *sw, m_dataViews)
    {
        sw->close();
    }
}

void mbCoreWindowManager::actionWindowDataViewCloseActive()
{
    mbCoreDataViewUi *ui = m_dataViewManager->activeDataViewUiCore();
    if (ui)
        closeSubWindow(ui);
}

void mbCoreWindowManager::actionWindowShowAll()
{
    actionWindowDataViewShowAll();
}

void mbCoreWindowManager::actionWindowShowActive()
{
    QMdiSubWindow *sw = m_area->activeSubWindow();
    if (sw)
    {
        sw->show();
        sw->widget()->show();
    }
}

void mbCoreWindowManager::actionWindowCloseActive()
{
    m_area->closeActiveSubWindow();
}

void mbCoreWindowManager::actionWindowCloseAll()
{
    actionWindowDataViewCloseAll();
}

void mbCoreWindowManager::actionWindowCascade()
{
    m_area->cascadeSubWindows();
}

void mbCoreWindowManager::actionWindowTile()
{
    Q_FOREACH(QMdiSubWindow *sw, m_area->subWindowList())
    {
        sw->show();
        sw->widget()->show();
    }
    m_area->tileSubWindows();
}

void mbCoreWindowManager::dataViewUiAdd(mbCoreDataViewUi *ui)
{
    QMdiSubWindow* sw = new QMdiSubWindow(m_area);
    sw->setWidget(ui);
    //sw->setAttribute(Qt::WA_DeleteOnClose, false);
    m_dataViews.append(sw);
    m_hashWindows.insert(ui, sw);
    connect(ui, &mbCoreDataViewUi::nameChanged, sw, &QWidget::setWindowTitle);
    sw->setWindowTitle(ui->name());
    m_area->addSubWindow(sw);
    sw->show();
}

void mbCoreWindowManager::dataViewUiRemove(mbCoreDataViewUi *ui)
{
    ui->disconnect(this);
    QMdiSubWindow* sw = m_hashWindows.value(ui, nullptr);
    if (sw)
    {
        m_dataViews.removeOne(sw);
        m_hashWindows.remove(ui);
        m_area->removeSubWindow(sw);
        sw->setWidget(nullptr);
        ui->setParent(nullptr);
        delete sw;
    }
}

void mbCoreWindowManager::subWindowActivated(QMdiSubWindow *sw)
{
    if (sw)
    {
        QWidget *w = sw->widget();
        if (mbCoreDataViewUi *ui = qobject_cast<mbCoreDataViewUi*>(w))
        {
            ui->blockSignals(true);
            m_dataViewManager->setActiveDataViewUi(ui);
            ui->blockSignals(false);
            return;
        }
    }
}

void mbCoreWindowManager::showSubWindow(const QWidget *ui)
{
    QMdiSubWindow *sw = m_hashWindows.value(ui);
    if (sw)
    {
        sw->show();
        sw->widget()->show();
    }
}

void mbCoreWindowManager::closeSubWindow(const QWidget *ui)
{
    QMdiSubWindow *sw = m_hashWindows.value(ui);
    if (sw)
        sw->close();
}
