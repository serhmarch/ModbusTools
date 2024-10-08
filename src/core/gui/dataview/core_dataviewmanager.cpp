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
#include "core_dataviewmanager.h"

#include <QMdiArea>
#include <QMdiSubWindow>

#include <core.h>

#include "core_dataviewui.h"
#include <project/core_project.h>
#include <project/core_dataview.h>

mbCoreDataViewManager::mbCoreDataViewManager(QObject *parent) : QObject(parent)
{
    m_project = nullptr;
    m_activeDataViewUi = nullptr;

    mbCore *core = mbCore::globalCore();
    connect(core, &mbCore::projectChanged, this, &mbCoreDataViewManager::setProject);
}

mbCoreDataView *mbCoreDataViewManager::activeDataViewCore() const
{
    mbCoreDataViewUi *ui = activeDataViewUiCore();
    if (ui)
        return ui->dataViewCore();
    return nullptr;
}

mbCoreDataViewUi *mbCoreDataViewManager::dataViewUiCore(const QString &name) const
{
    if (m_project)
    {
        mbCoreDataView *v = m_project->dataViewCore(name);
        return m_hashDataViewUis.value(v, nullptr);
    }
    return nullptr;
}

void mbCoreDataViewManager::setActiveDataViewUi(mbCoreDataViewUi *ui)
{
    // TODO: ASSERT m_hashDataViewUis.contains(ui->dataView())
    if (m_activeDataViewUi != ui)
    {
        m_activeDataViewUi = ui;
        Q_EMIT dataViewUiActivated(ui);
    }
}

void mbCoreDataViewManager::setProject(mbCoreProject *p)
{
    mbCoreProject *project = static_cast<mbCoreProject*>(p);
    if (m_project != project)
    {
        if (m_project)
        {
            Q_FOREACH (mbCoreDataView* d, m_project->dataViewsCore())
                dataViewRemove(d);
            m_project->disconnect(this);
            m_activeDataViewUi = nullptr;
        }
        m_project = project;
        if (m_project)
        {
            connect(m_project, &mbCoreProject::dataViewAdded, this, &mbCoreDataViewManager::dataViewAdd);
            connect(m_project, &mbCoreProject::dataViewRemoving, this, &mbCoreDataViewManager::dataViewRemove);
            Q_FOREACH (mbCoreDataView* d, m_project->dataViewsCore())
                dataViewAdd(d);
        }
    }
}

void mbCoreDataViewManager::dataViewAdd(mbCoreDataView *dataView)
{
    // TODO: ASSERT !m_hashDataViewUis.contains(dataView)
    mbCoreDataViewUi *ui = createDataViewUi(dataView);
    m_dataViewUis.append(ui);
    m_hashDataViewUis.insert(dataView, ui);
    connect(ui, &mbCoreDataViewUi::customContextMenuRequested, this, &mbCoreDataViewManager::dataViewContextMenu);
    Q_EMIT dataViewUiAdd(ui);
}

void mbCoreDataViewManager::dataViewRemove(mbCoreDataView *dataView)
{
    // TODO: ASSERT ui != nullptr && m_hashDataViewUis.contains(ui->dataView())
    mbCoreDataViewUi *ui = dataViewUiCore(dataView);
    if (m_activeDataViewUi == ui)
        m_activeDataViewUi = nullptr;
    ui->disconnect(this);
    Q_EMIT dataViewUiRemove(ui);
    m_dataViewUis.removeOne(ui);
    m_hashDataViewUis.remove(dataView);
    ui->deleteLater(); // Note: need because 'ui' can have 'QMenu'-children located in stack which is trying to delete
}

void mbCoreDataViewManager::dataViewContextMenu(const QPoint & /*pos*/)
{
    mbCoreDataViewUi *ui = qobject_cast<mbCoreDataViewUi*>(sender());
    Q_EMIT dataViewUiContextMenu(ui);
}
