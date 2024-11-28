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

#include <QMessageBox>
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
class mbCoreDataViewUi;
class mbCoreProjectUi;
class mbCoreLogView;
class mbCoreOutputView;
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
        const QString settings_recentProjects;
        const QString wGeometry;
        const QString wState;
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
    QWidget *outputView() const;

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
    void logMessage(const QString& message);
    void outputMessage(const QString& message);

protected Q_SLOTS:
    // ----------------------------
    // ------------FILE------------
    // ----------------------------
    virtual void menuSlotFileNew   ();
    virtual void menuSlotFileOpen  ();
    virtual void menuSlotFileClose ();
    virtual void menuSlotFileSave  ();
    virtual void menuSlotFileSaveAs();
    virtual void menuSlotFileEdit  ();
    virtual void menuSlotFileInfo  ();
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
    virtual void menuSlotViewOutput();
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
    // --------- DATA VIEW --------
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

protected Q_SLOTS: // non menu slots
    virtual void slotDataViewItemCopy();
    virtual void slotDataViewItemPaste();
    virtual void slotDataViewItemSelectAll();
    virtual void slotTrayActivated(QSystemTrayIcon::ActivationReason reason);

protected Q_SLOTS:
    virtual void contextMenuPort(mbCorePort *port);
    virtual void contextMenuDataViewUi(mbCoreDataViewUi *ui);

protected Q_SLOTS:
    virtual void setProject(mbCoreProject *project);
    virtual void setProjectName(const QString &name);
    void currentPortChanged(mbCorePort *port);
    void refreshCurrentPortName();
    void setStatTx(quint32 count);
    void setStatRx(quint32 count);
    void statusChange(int status);
    void menuRecentTriggered(QAction *a);

protected:
    QMessageBox::StandardButton checkProjectModifiedAndSave(const QString &title,
                                                            const QString &action,
                                                            QMessageBox::StandardButtons buttons = QMessageBox::StandardButtons(QMessageBox::Yes | QMessageBox::No));
    void openProject(const QString &file);
    void closeProject();
    void addRecentFile(const QString &absPath);
    void removeRecentFile(const QString &absPath);
    void recentClear();

    QVariantList cachedSettingsRecentProjects() const;
    void setCachedSettingsRecentProjects(const QVariantList &ls);

protected:
    void closeEvent(QCloseEvent *e) override;

protected:
    mbCore *m_core;
    mbCoreProject *m_project;
    mbCoreBuilder *m_builder;
    mbCoreDialogs* m_dialogs;
    mbCoreProjectUi *m_projectUi;
    mbCoreWindowManager *m_windowManager;
    mbCoreDataViewManager *m_dataViewManager;
    mbCorePort *m_currentPort;

protected:
    QSystemTrayIcon* m_tray;
    QString m_helpFile;
    mbCoreLogView *m_logView;
    mbCoreHelpUi *m_help;

protected:
    struct // ui defined in derived classes
    {
        QMenu       *menuFile                       ;
        QMenu       *menuEdit                       ;
        QMenu       *menuView                       ;
        QMenu       *menuPort                       ;
        QMenu       *menuDevice                     ;
        QMenu       *menuDataView                   ;
        QMenu       *menuTools                      ;
        QMenu       *menuRuntime                    ;
        QMenu       *menuWindow                     ;
        QMenu       *menuHelp                       ;
        QAction     *actionFileNew                  ;
        QAction     *actionFileOpen                 ;
        QAction     *actionFileRecent               ;
        QAction     *actionFileClose                ;
        QAction     *actionFileSave                 ;
        QAction     *actionFileSaveAs               ;
        QAction     *actionFileEdit                 ;
        QAction     *actionFileInfo                 ;
        QAction     *actionFileQuit                 ;
        QAction     *actionEditCut                  ;
        QAction     *actionEditCopy                 ;
        QAction     *actionEditPaste                ;
        QAction     *actionEditInsert               ;
        QAction     *actionEditEdit                 ;
        QAction     *actionEditDelete               ;
        QAction     *actionEditSelectAll            ;
        QAction     *actionViewProject              ;
        QAction     *actionViewLogView              ;
        QAction     *actionViewOutput               ;
        QAction     *actionPortNew                  ;
        QAction     *actionPortEdit                 ;
        QAction     *actionPortDelete               ;
        QAction     *actionPortImport               ;
        QAction     *actionPortExport               ;
        QAction     *actionDeviceNew                ;
        QAction     *actionDeviceEdit               ;
        QAction     *actionDeviceDelete             ;
        QAction     *actionDeviceImport             ;
        QAction     *actionDeviceExport             ;
        QAction     *actionDataViewItemNew          ;
        QAction     *actionDataViewItemEdit         ;
        QAction     *actionDataViewItemInsert       ;
        QAction     *actionDataViewItemDelete       ;
        QAction     *actionDataViewImportItems      ;
        QAction     *actionDataViewExportItems      ;
        QAction     *actionDataViewNew              ;
        QAction     *actionDataViewEdit             ;
        QAction     *actionDataViewInsert           ;
        QAction     *actionDataViewDelete           ;
        QAction     *actionDataViewImport           ;
        QAction     *actionDataViewExport           ;
        QAction     *actionWindowShowAll            ;
        QAction     *actionWindowShowActive         ;
        QAction     *actionWindowCloseAll           ;
        QAction     *actionWindowCloseActive        ;
        QAction     *actionWindowCascade            ;
        QAction     *actionWindowTile               ;
        QAction     *actionHelpAbout                ;
        QAction     *actionHelpAboutQt              ;
        QAction     *actionHelpContents             ;
        QAction     *actionToolsSettings            ;
        QAction     *actionRuntimeStartStop         ;
        QDockWidget *dockProject                    ;
        QDockWidget *dockLogView                    ;
        QStatusBar  *statusbar                      ;
    } m_ui;

    QMenu *m_menuRecent;
    QAction *m_actionFileRecentClear;
    // Output
    QDockWidget *m_dockOutput;
    mbCoreOutputView *m_outputView;

    QLabel *m_lbSystemName;
    QLabel *m_lbSystemStatus;
    QLabel *m_lbPortName;
    QLabel *m_lbPortStatTx;
    QLabel *m_lbPortStatRx;

    typedef QHash<QString, QAction*> RecentProjectActions_t;
    RecentProjectActions_t m_recentProjectActions;
};

#endif // CORE_UI_H
