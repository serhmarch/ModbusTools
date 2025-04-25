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
#ifndef SERVER_SCRIPTMODULESUI_H
#define SERVER_SCRIPTMODULESUI_H

#include <QModelIndex>
#include <QWidget>

class QTableView;

class mbServerScriptModule;
class mbServerScriptModulesModel;
class mbServerScriptModulesDelegate;

class mbServerScriptModulesUi : public QWidget
{
    Q_OBJECT
public:
    explicit mbServerScriptModulesUi(QWidget *parent = nullptr);

public:
    mbServerScriptModulesModel *model() const { return m_model; }
    mbServerScriptModulesDelegate *delegate() const { return m_delegate; }

public:
    QModelIndex currentItemModelIndex() const;
    inline int currentItemIndex() const { return currentItemModelIndex().row(); }
    mbServerScriptModule *currentItem() const;
    QList<mbServerScriptModule*> selectedItems() const;
    void selectItem(mbServerScriptModule *item);

public Q_SLOTS:
    void selectAll();

Q_SIGNALS:
    void scriptModuleDoubleClick(mbServerScriptModule *item);
    void scriptModuleContextMenu(mbServerScriptModule *item);

protected Q_SLOTS:
    void customContextMenu(const QPoint &pos);
    void doubleClick(const QModelIndex &index);
    void contextMenu(const QModelIndex &index);

private:
    QTableView *m_view;
    mbServerScriptModulesModel *m_model;
    mbServerScriptModulesDelegate *m_delegate;
};

#endif // SERVER_SCRIPTMODULESUI_H
