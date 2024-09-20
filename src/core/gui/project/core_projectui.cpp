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
#include "core_projectui.h"

#include <QVBoxLayout>
#include <QTreeView>

#include <core.h>
#include <project/core_project.h>
#include <project/core_port.h>

#include "core_projectmodel.h"
#include "core_projectdelegate.h"

mbCoreProjectUi::mbCoreProjectUi(mbCoreProjectModel *model, mbCoreProjectDelegate *delegate, QWidget *parent)
    : QWidget{parent}
{
    m_currentPort = nullptr;

    mbCore *core = mbCore::globalCore();
    connect(core, &mbCore::projectChanged, this, &mbCoreProjectUi::setProject);

    m_view = new QTreeView(this);
    m_view->setHeaderHidden(true);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QAbstractItemView::customContextMenuRequested, this, &mbCoreProjectUi::customContextMenu);

    m_model = model;
    m_model->setParent(this);
    m_view->setModel(m_model);

    m_delegate = delegate;
    m_delegate->setParent(this);
    connect(m_delegate, SIGNAL(doubleClick(QModelIndex)), this, SLOT(doubleClick(QModelIndex)));
    connect(m_delegate, SIGNAL(contextMenu(QModelIndex)), this, SLOT(contextMenu(QModelIndex)));
    m_view->setItemDelegate(m_delegate);

    QItemSelectionModel *sm = m_view->selectionModel();
    connect(sm, &QItemSelectionModel::currentChanged, this, &mbCoreProjectUi::selectionChanged);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);
}

mbCorePort *mbCoreProjectUi::selectedPortCore() const
{
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (!ls.isEmpty())
    {
        const QModelIndex index = ls.first();
        mbCorePort *port = m_model->getPortByIndex(index);
        if (port)
            return port;
    }
    return nullptr;
}

bool mbCoreProjectUi::useNameWithSettings() const
{
    return m_model->useNameWithSettings();
}

void mbCoreProjectUi::setUseNameWithSettings(bool use)
{
    m_model->setUseNameWithSettings(use);
}

void mbCoreProjectUi::customContextMenu(const QPoint &pos)
{
    contextMenu(m_view->indexAt(pos));
}

void mbCoreProjectUi::doubleClick(const QModelIndex &index)
{
    if (mbCorePort *d = m_model->portCore(index))
    {
        Q_EMIT portDoubleClick(d);
        return;
    }
}

void mbCoreProjectUi::contextMenu(const QModelIndex &index)
{
    mbCorePort *d = m_model->portCore(index);
    Q_EMIT portContextMenu(d);
}

void mbCoreProjectUi::selectionChanged(const QModelIndex &current, const QModelIndex &)
{
    setCurrentPort(m_model->getPortByIndex(current));
}

void mbCoreProjectUi::setProject(mbCoreProject *project)
{
    mbCoreProject *old = m_model->projectCore();
    if (old)
    {
        Q_FOREACH (mbCorePort* p, old->portsCore())
            m_model->portRemove(p);

        old->disconnect(this);
    }
    m_model->setProject(project);
    if (project)
    {
        setCurrentPort(project->portCore(0));
        connect(project, &mbCoreProject::portAdded   , this, &mbCoreProjectUi::portAdd   );
        connect(project, &mbCoreProject::portRemoving, this, &mbCoreProjectUi::portRemove);
        Q_FOREACH (mbCorePort* p, project->portsCore())
            portAdd(p);
    }
    else
        setCurrentPort(nullptr);
}

void mbCoreProjectUi::portAdd(mbCorePort *port)
{
    setCurrentPort(port);
    m_model->portAdd(port);
    QItemSelectionModel *sm = m_view->selectionModel();
    sm->clearSelection();
    sm->select(m_model->portIndex(port), QItemSelectionModel::Select);
}

void mbCoreProjectUi::portRemove(mbCorePort *port)
{
    m_model->portRemove(port);
    if (m_currentPort == port)
    {
        mbCoreProject *project = m_model->projectCore();
        if (project->portCount() > 1)
        {
            mbCorePort *next = project->portCoreAt(0);
            if (next == port)
                next = project->portCoreAt(1);
            setCurrentPort(next);
        }
        else
            setCurrentPort(nullptr);
    }
}

void mbCoreProjectUi::setCurrentPort(mbCorePort *port)
{
    if (m_currentPort != port)
    {
        m_currentPort = port;
        Q_EMIT currentPortChanged(port);
    }
}
