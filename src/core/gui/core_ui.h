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
#ifndef CORE_UI_H
#define CORE_UI_H

#include <QUndoCommand>
#include <QMainWindow>
#include <QSystemTrayIcon>

#include <core.h>

class QLabel;
class mbCore;
class mbCoreDialogs;
class mbCoreWindowManager;
class mbCoreDataViewManager;
class mbCoreBuilder;
class mbCoreProject;
class mbCorePort;
class mbCoreDevice;
class mbCoreDataView;
class mbCoreProjectUi;
class mbCoreLogView;
class mbCoreHelpUi;

namespace Ui {
class mbCoreUi;
}

class MB_EXPORT mbCoreUi : public QMainWindow
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString settings_useNameWithSettings;
        Strings();
        static const Strings &instance();
    };

    struct MB_EXPORT Defaults
    {
        const bool settings_useNameWithSettings;
        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbCoreUi(mbCore* core, QWidget *parent = nullptr);
    ~mbCoreUi();

public:
    QWidget *logView() const;

public:
    inline mbCore* baseCore() const { return m_core; }
    inline mbCoreBuilder *builderCore() const { return m_builder; }
    inline mbCoreDialogs* dialogsCore() const { return m_dialogs; }
    inline mbCoreProjectUi *projectUiCore() const { return m_projectUi; }
    inline mbCoreWindowManager *windowManagerCore() const { return m_windowManager; }
    inline mbCoreDataViewManager *dataViewManagerCore() const { return m_dataViewManager; }

public:
    virtual void initialize();

public: // settings
    bool useNameWithSettings() const;
    void setUseNameWithSettings(bool use);

    virtual MBSETTINGS cachedSettings() const;
    virtual void setCachedSettings(const MBSETTINGS &settings);

public Q_SLOTS:
    void showMessage(const QString& message);

protected Q_SLOTS:
    // ----------------------------
    // ------------FILE------------
    // ----------------------------
    virtual void menuSlotFileNew   ();
    virtual void menuSlotFileOpen  ();
    virtual void menuSlotFileSave  ();
    virtual void menuSlotFileSaveAs();
    virtual void menuSlotFileEdit  ();
    virtual void menuSlotFileQuit  ();
    // ----------------------------
    // ------------EDIT------------
    // ----------------------------
    virtual void menuSlotEditUndo     ();
    virtual void menuSlotEditRedo     ();
    virtual void menuSlotEditCut      ();
    virtual void menuSlotEditCopy     ();
    virtual void menuSlotEditPaste    ();
    virtual void menuSlotEditInsert   ();
    virtual void menuSlotEditEdit     ();
    virtual void menuSlotEditDelete   ();
    virtual void menuSlotEditSelectAll();
    // ----------------------------
    // ------------VIEW------------
    // ----------------------------
    virtual void menuSlotViewProject();
    virtual void menuSlotViewLogView();
    // ----------------------------
    // ------------PORT------------
    // ----------------------------
    virtual void menuSlotPortNew   ();
    virtual void menuSlotPortEdit  ();
    virtual void menuSlotPortDelete();
    virtual void menuSlotPortImport();
    virtual void menuSlotPortExport();
    // ----------------------------
    // -----------DEVICE-----------
    // ----------------------------
    virtual void menuSlotDeviceNew   ();
    virtual void menuSlotDeviceEdit  ();
    virtual void menuSlotDeviceDelete();
    virtual void menuSlotDeviceImport();
    virtual void menuSlotDeviceExport();
    // ----------------------------
    // ---------WATCH LIST---------
    // ----------------------------
    virtual void menuSlotDataViewItemNew    ();
    virtual void menuSlotDataViewItemEdit   ();
    virtual void menuSlotDataViewItemInsert ();
    virtual void menuSlotDataViewItemDelete ();
    virtual void menuSlotDataViewImportItems();
    virtual void menuSlotDataViewExportItems();
    virtual void menuSlotDataViewNew        ();
    virtual void menuSlotDataViewEdit       ();
    virtual void menuSlotDataViewInsert     ();
    virtual void menuSlotDataViewDelete     ();
    virtual void menuSlotDataViewImport     ();
    virtual void menuSlotDataViewExport     ();
    // ----------------------------
    // ------------TOOLS-----------
    // ----------------------------
    virtual void menuSlotToolsSettings();
    // ----------------------------
    // -----------RUNTIME----------
    // ----------------------------
    virtual void menuSlotRuntimeStartStop();
    // ----------------------------
    // -----------WINDOW-----------
    // ----------------------------
    virtual void menuSlotWindowDataViewShowAll    ();
    virtual void menuSlotWindowDataViewShowActive ();
    virtual void menuSlotWindowDataViewCloseAll   ();
    virtual void menuSlotWindowDataViewCloseActive();
    virtual void menuSlotWindowShowAll            ();
    virtual void menuSlotWindowShowActive         ();
    virtual void menuSlotWindowCloseAll           ();
    virtual void menuSlotWindowCloseActive        ();
    virtual void menuSlotWindowCascade            ();
    virtual void menuSlotWindowTile               ();
    // ----------------------------
    // ------------HELP------------
    // ----------------------------
    virtual void menuSlotHelpAbout   ();
    virtual void menuSlotHelpAboutQt ();
    virtual void menuSlotHelpContents();
    //------------------------------
    virtual void trayActivated(QSystemTrayIcon::ActivationReason reason);


protected:
    mbCore *m_core;
    mbCoreBuilder *m_builder;
    mbCoreDialogs* m_dialogs;
    mbCoreProjectUi *m_projectUi;
    mbCoreWindowManager *m_windowManager;
    mbCoreDataViewManager *m_dataViewManager;

protected:
    QSystemTrayIcon* m_tray;
    QString m_projectFileFilter;
    QString m_helpFile;
    mbCoreLogView *m_logView;
    mbCoreHelpUi *m_help;
};

#endif // CORE_UI_H
