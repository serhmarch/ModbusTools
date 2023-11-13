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
#include "core_dataviewui.h"

#include <QAction>
#include <QTableView>
#include <QHeaderView>
#include <QComboBox>
#include <QMenu>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QFileInfo>
#include <QDir>
#include <QInputDialog>

#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>

#include <core.h>
#include <gui/core_ui.h>

#include <project/core_project.h>
#include <project/core_device.h>
#include <project/core_dataview.h>

#include "core_dataviewmodel.h"
#include "core_dataviewdelegate.h"

mbCoreDataViewUi::Strings::Strings() :
    name(QStringLiteral("dataView"))
{
}

const mbCoreDataViewUi::Strings &mbCoreDataViewUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDataViewUi::mbCoreDataViewUi(mbCoreDataView *dataView, mbCoreDataViewModel *model, mbCoreDataViewDelegate *delegate, QWidget *parent) : QWidget (parent)
{
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    m_dataView = dataView;

    m_model = model;
    m_model->setParent(this);

    m_delegate = delegate;
    m_delegate->setParent(this);
    connect(m_delegate, &mbCoreDataViewDelegate::doubleClick, this, &mbCoreDataViewUi::doubleClick);
    connect(m_delegate, &mbCoreDataViewDelegate::contextMenu, this, &mbCoreDataViewUi::contextMenu);

    m_view = new QTableView(this);
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(dataView, &mbCoreDataView::nameChanged, this, &mbCoreDataViewUi::nameChanged);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_view);
}

QString mbCoreDataViewUi::name() const
{
    return m_dataView->name();
}

QModelIndex mbCoreDataViewUi::currentItemModelIndex() const
{
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (ls.count())
        return ls.first();
    return QModelIndex();
}

mbCoreDataViewItem *mbCoreDataViewUi::currentItemCore() const
{
    return m_dataView->itemCore(currentItemIndex());
}

QList<mbCoreDataViewItem *> mbCoreDataViewUi::selectedItemsCore() const
{
    QList<mbCoreDataViewItem*> r;
    QModelIndexList ls = m_view->selectionModel()->selectedIndexes();
    if (!ls.count())
        return r;
    QSet<int> setIndexes;
    Q_FOREACH (QModelIndex i, ls)
        setIndexes.insert(i.row()); // get unique selected rows
    QList<int> indexes = setIndexes.values();
    std::sort(indexes.begin(), indexes.end());
    Q_FOREACH (int i, indexes)
        r.append(m_dataView->itemCore(i));
    return r;
}

void mbCoreDataViewUi::selectItem(mbCoreDataViewItem *item)
{
    QModelIndex index = m_model->itemIndex(item);
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QModelIndex firstColumnIndex = index.sibling(index.row(), 0);
    QModelIndex lastColumnIndex = index.sibling(index.row(), index.model()->columnCount() - 1);
    QItemSelection rowSelection(firstColumnIndex, lastColumnIndex);
    selectionModel->clearSelection();
    selectionModel->select(rowSelection, QItemSelectionModel::Select);
}

void mbCoreDataViewUi::selectAll()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QItemSelection selection;
    QModelIndex topLeft = m_model->index(0, 0);
    QModelIndex bottomRight = m_model->index(m_model->rowCount() - 1, m_model->columnCount() - 1);
    selection.select(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
}

void mbCoreDataViewUi::doubleClick(const QModelIndex &index)
{
    if (mbCoreDataViewItem *item = m_model->itemCore(index))
        Q_EMIT itemDoubleClick(item);
}

void mbCoreDataViewUi::contextMenu(const QModelIndex &index)
{
    if (mbCoreDataViewItem *item = m_model->itemCore(index))
        Q_EMIT itemContextMenu(item);
}

