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
#ifndef SERVER_SIMACTIONSUI_H
#define SERVER_SIMACTIONSUI_H

#include <QModelIndex>
#include <QWidget>

class QTableView;

class mbServerSimAction;
class mbServerSimActionsModel;
class mbServerSimActionsDelegate;

class mbServerSimActionsUi : public QWidget
{
    Q_OBJECT
public:
    explicit mbServerSimActionsUi(QWidget *parent = nullptr);

public:
    mbServerSimActionsModel *model() const { return m_model; }
    mbServerSimActionsDelegate *delegate() const { return m_delegate; }

public:
    QModelIndex currentItemModelIndex() const;
    inline int currentItemIndex() const { return currentItemModelIndex().row(); }
    mbServerSimAction *currentItem() const;
    QList<mbServerSimAction*> selectedItems() const;
    void selectItem(mbServerSimAction *item);

public Q_SLOTS:
    void selectAll();

Q_SIGNALS:
    void simActionDoubleClick(mbServerSimAction *item);
    void simActionContextMenu(mbServerSimAction *item);

protected Q_SLOTS:
    void customContextMenu(const QPoint &pos);
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);

private:
    QTableView *m_view;
    mbServerSimActionsModel *m_model;
    mbServerSimActionsDelegate *m_delegate;
};

#endif // SERVER_SIMACTIONSUI_H
