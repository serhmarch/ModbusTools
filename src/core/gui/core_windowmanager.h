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
#ifndef CORE_WINDOWMANAGER_H
#define CORE_WINDOWMANAGER_H

#include <QHash>
#include <QMdiArea>

#include <mbcore.h>

class QMdiSubWindow;

class mbCoreDevice;
class mbCoreDataView;

class mbCoreUi;
class mbCoreDataViewManager;
class mbCoreDataViewUi;

class MB_EXPORT mbCoreWindowManager : public QObject
{
    Q_OBJECT
public:
    explicit mbCoreWindowManager(mbCoreUi *ui, mbCoreDataViewManager *dataViewManager);

public:
    inline QWidget *centralWidget() const { return m_area; }

public:
    inline mbCoreUi *uiCore() const { return m_ui; }
    inline mbCoreDataViewManager *dataViewManagerCore() { return m_dataViewManager; }
    mbCoreDataView *activeDataViewCore() const;
    void setActiveDataViewCore(mbCoreDataView *dataView);
    inline void setActiveDataView(mbCoreDataView *dataView) { setActiveDataViewCore(dataView); }
    
public Q_SLOTS:
    void showDataViewUi(const mbCoreDataViewUi *ui);
    void actionWindowDataViewShowAll();
    void actionWindowDataViewShowActive();
    void actionWindowDataViewCloseAll();
    void actionWindowDataViewCloseActive();
    virtual void actionWindowShowAll();
    void actionWindowShowActive();
    virtual void actionWindowCloseAll();
    void actionWindowCloseActive();
    void actionWindowCascade();
    void actionWindowTile();

Q_SIGNALS:

protected Q_SLOTS:
    void dataViewUiAdd(mbCoreDataViewUi *ui);
    void dataViewUiRemove(mbCoreDataViewUi *ui);

protected Q_SLOTS:
    virtual void subWindowActivated(QMdiSubWindow *sw);

protected:
    void showSubWindow(const QWidget *ui);
    void closeSubWindow(const QWidget *ui);

protected:
    QMdiArea *m_area;
    mbCoreUi *m_ui;
    mbCoreDataViewManager *m_dataViewManager;

    // DataViews
    typedef QList<QMdiSubWindow*> DataViews_t;
    DataViews_t m_dataViews;

    // Windows
    typedef QHash<const QWidget*, QMdiSubWindow*> HashWindows_t;
    HashWindows_t m_hashWindows;
};

#endif // CORE_WINDOWMANAGER_H
