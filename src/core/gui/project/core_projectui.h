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
#ifndef CORE_PROJECTUI_H
#define CORE_PROJECTUI_H

#include <QWidget>

#include <mbcore.h>

class QTreeView;

class mbCorePort;
class mbCoreProjectModel;
class mbCoreProjectDelegate;

class MB_EXPORT mbCoreProjectUi : public QWidget
{
    Q_OBJECT
public:
    explicit mbCoreProjectUi(mbCoreProjectModel *model, mbCoreProjectDelegate *delegate, QWidget *parent = nullptr);

public:
    virtual mbCorePort *currentPortCore() const;

public: // settings
    bool useNameWithSettings() const;
    void setUseNameWithSettings(bool use);

Q_SIGNALS:
    void portDoubleClick(mbCorePort *port);
    void portContextMenu(mbCorePort *port);

protected Q_SLOTS:
    void customContextMenu(const QPoint &pos);
    virtual void doubleClick(const QModelIndex &index);
    virtual void contextMenu(const QModelIndex &index);

protected:
    QTreeView *m_view;
    mbCoreProjectModel *m_model;
    mbCoreProjectDelegate *m_delegate;
};

#endif // CORE_PROJECTUI_H
