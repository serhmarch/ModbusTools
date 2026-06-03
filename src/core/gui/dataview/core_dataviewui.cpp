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
#include <QSortFilterProxyModel>

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
    m_view->setSortingEnabled(true);
    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_model);
    m_view->setModel(m_proxyModel);
    //m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_view->setAlternatingRowColors(true);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    //header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);

    setEnableProcessing(m_dataView->isEnableProcessing());
    connect(m_dataView, &mbCoreDataView::enableProcessingChanged, this, &mbCoreDataViewUi::setEnableProcessing);

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

int mbCoreDataViewUi::currentItemIndex() const
{
    QModelIndex index = currentItemModelIndex();
    if (index.isValid())
        return m_proxyModel->mapToSource(index).row();
    return -1;
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
        setIndexes.insert(m_proxyModel->mapToSource(i).row()); // get unique selected rows
    QList<int> indexes = setIndexes.values();
    std::sort(indexes.begin(), indexes.end());
    Q_FOREACH (int i, indexes)
        r.append(m_dataView->itemCore(i));
    return r;
}

void mbCoreDataViewUi::selectItem(mbCoreDataViewItem *item)
{
    QModelIndex index = m_proxyModel->mapFromSource(m_model->itemIndex(item));
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QModelIndex firstColumnIndex = index.sibling(index.row(), 0);
    QModelIndex lastColumnIndex = index.sibling(index.row(), index.model()->columnCount() - 1);
    QItemSelection rowSelection(firstColumnIndex, lastColumnIndex);
    selectionModel->clearSelection();
    selectionModel->select(rowSelection, QItemSelectionModel::Select);
}

int mbCoreDataViewUi::dataIndex(const QModelIndex &index) const
{
    auto idx = m_proxyModel->mapToSource(index);
    return idx.row();
}

int mbCoreDataViewUi::getColumnTypeByIndex(const QModelIndex &index) const
{
    auto idx = m_proxyModel->mapToSource(index);
    return m_dataView->getColumnTypeByIndex(idx.column());
}

void mbCoreDataViewUi::selectAll()
{
    QItemSelectionModel* selectionModel = m_view->selectionModel();
    QItemSelection selection;
    QModelIndex topLeft = m_proxyModel->index(0, 0);
    QModelIndex bottomRight = m_proxyModel->index(m_proxyModel->rowCount() - 1, m_proxyModel->columnCount() - 1);
    selection.select(topLeft, bottomRight);
    selectionModel->select(selection, QItemSelectionModel::Select);
}

void mbCoreDataViewUi::doubleClick(const QModelIndex &index)
{
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    if (mbCoreDataViewItem *item = m_model->itemCore(sourceIndex))
        Q_EMIT itemDoubleClick(item);
}

void mbCoreDataViewUi::contextMenu(const QModelIndex &index)
{
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);
    if (mbCoreDataViewItem *item = m_model->itemCore(sourceIndex))
        Q_EMIT itemContextMenu(item);
}

void mbCoreDataViewUi::setEnableProcessing(bool enable)
{
    if (enable)
    {
        QString headerStyleSheet = R"(
        QHeaderView::section {
            background-color: #f0f0f0;        /* Light gray background */
            color: #2c3e50;                   /* Dark gray text color */
            border: 1px solid #dcdcdc;        /* Subtle light gray border around sections */
            font-size: 11px;                  /* Font size */
            font-weight: normal;              /* Normal text weight for a clean look */
            text-align: left;                 /* Align text to the left */
        }

        QHeaderView::section:pressed {
            background-color: #d0d0d0;        /* Darker background when pressed */
            border: 1px solid #bcbcbc;        /* Darker border when pressed */
        }

        )";

        m_view->setStyleSheet(headerStyleSheet);
    }
    else
    {
        QString headerStyleSheet = R"(
        QHeaderView::section {
            background-color: #e0e0e0;        /* Darker gray background for disabled */
            color: #a0a0a0;                   /* Muted gray text color */
            border: 1px solid #c0c0c0;        /* Darker border for disabled state */
            font-size: 11px;                  /* Font size */
            font-weight: normal;              /* Normal text weight */
            text-align: left;                 /* Align text to the left */
        }

        QHeaderView::section:pressed {
            background-color: #e0e0e0;        /* Same as normal disabled state */
            border: 1px solid #c0c0c0;        /* Same border when pressed in disabled */
        }

        )";

        m_view->setStyleSheet(headerStyleSheet);
    }
    QPalette p = this->palette();
    QPalette newPalette = p;
    QPalette::ColorGroup cg = enable ? QPalette::Active : QPalette::Disabled;
    // Copy how disabled text, backgrounds, buttons normally look
    newPalette.setColor(QPalette::Window,    p.color(cg, QPalette::Window));
    newPalette.setColor(QPalette::WindowText,p.color(cg, QPalette::WindowText));
    newPalette.setColor(QPalette::Base,      p.color(cg, QPalette::Base));
    newPalette.setColor(QPalette::Text,      p.color(cg, QPalette::Text));
    newPalette.setColor(QPalette::Button,    p.color(cg, QPalette::Button));
    newPalette.setColor(QPalette::ButtonText,p.color(cg, QPalette::ButtonText));

    m_view->setPalette(newPalette);
    m_view->setAutoFillBackground(true);
}

