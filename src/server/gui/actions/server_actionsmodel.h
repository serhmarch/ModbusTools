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
#ifndef SERVER_ACTIONSMODEL_H
#define SERVER_ACTIONSMODEL_H

#include <QAbstractItemModel>
#include <QObject>

class mbCoreProject;
class mbServerProject;
class mbServerAction;

class mbServerActionsModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum Column
    {
        Column_Device,
        Column_Address,
        Column_DataType,
        Column_Period,
        Column_Comment,
        Column_ActionType,
        Column_ExtendedSettings,
        ColumnCount
    };

public:
    explicit mbServerActionsModel(QObject *parent = nullptr);

public: // table model interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

public:
    mbServerProject *project() const { return m_project; }
    QModelIndex actionIndex(mbServerAction *item) const;
    inline QModelIndex itemIndex(mbServerAction *item) const { return actionIndex(item); }
    mbServerAction *action(const QModelIndex &index) const;
    inline mbServerAction *item(const QModelIndex &index) const { return action(index); }

private Q_SLOTS:
    void setProject(mbCoreProject *project);
    void actionAdd(mbServerAction *action);
    void actionRemove(mbServerAction *action);
    void actionChange(mbServerAction *action);

private:
    mbServerProject* m_project;
};

#endif // SERVER_ACTIONSMODEL_H
