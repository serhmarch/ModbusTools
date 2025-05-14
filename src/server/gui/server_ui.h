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
#ifndef SERVER_UI_H
#define SERVER_UI_H

#include <gui/core_ui.h>
#include <server.h>

class QLabel;
class QLineEdit;
class QComboBox;
class QDockWidget;
class QMdiArea;

class mbCoreDataViewUi;

class mbServerPort;
class mbServerDevice;
class mbServerDeviceRef;
class mbServerSimAction;
class mbServerOutputView;

class mbServer;
class mbServerBuilder;
class mbServerWindowManager;
class mbServerDialogs;
class mbServerProjectUi;
class mbServerSimActionsUi;
class mbServerDeviceManager;
class mbServerScriptManager;
class mbServerScriptModulesUi;
class mbServerScriptModule;
class mbServerDataViewManager;
class mbServerDeviceUi;
class mbServerDataViewUi;

namespace Ui {
class mbServerUi;
}

class mbServerUi : public mbCoreUi
{
    Q_OBJECT

public:
    struct Strings : public mbCoreUi::Strings
    {
        const QString cacheFormat;
        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbCoreUi::Defaults
    {
        mb::DigitalFormat cacheFormat;
        int filterFileActions;
        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbServerUi(mbServer* core, QWidget *parent = nullptr);
    ~mbServerUi();

public:
    inline mbServer *core() const { return static_cast<mbServer*>(baseCore()); }
    inline mbServerBuilder* builder() const { return reinterpret_cast<mbServerBuilder*>(builderCore()); }
    inline mbServerDialogs *dialogs() const { return reinterpret_cast<mbServerDialogs*>(dialogsCore()); }
    inline mbServerProjectUi* projectUi() const { return reinterpret_cast<mbServerProjectUi*>(projectUiCore()); }
    inline mbServerWindowManager *windowManager() const { return reinterpret_cast<mbServerWindowManager*>(windowManagerCore()); }
    inline mbServerDeviceManager *deviceManager() const { return m_deviceManager; }
    inline mbServerScriptManager *scriptManager() const { return m_scriptManager; }
    inline mbServerDataViewManager *dataViewManager() const { return reinterpret_cast<mbServerDataViewManager*>(dataViewManagerCore()); }
    inline mbServerProject *project() const { return reinterpret_cast<mbServerProject*>(projectCore()); }

public:
    QWidget *outputView() const;

public:
    void initialize() override;

public: // settings
    inline mb::DigitalFormat format() const { return m_format; }

    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public Q_SLOTS:
    void outputMessage(const QString& message) override;

Q_SIGNALS:
    void formatChanged(int format);

private Q_SLOTS:
    // ----------------------------
    // ------------VIEW------------
    // ----------------------------
    void menuSlotViewOutput();
    // ----------------------------
    // ------------EDIT------------
    // ----------------------------
    void menuSlotEditCopy     () override;
    void menuSlotEditPaste    () override;
    void menuSlotEditInsert   () override;
    void menuSlotEditEdit     () override;
    void menuSlotEditDelete   () override;
    void menuSlotEditSelectAll() override;
    // ----------------------------
    // ------------VIEW------------
    // ----------------------------
    void menuSlotViewSimulation   ();
    void menuSlotViewScriptModules();
    // ----------------------------
    // ------------PORT------------
    // ----------------------------
    void menuSlotPortNew         () override;
    void menuSlotPortEdit        () override;
    void menuSlotPortDelete      () override;
    void menuSlotPortDeviceNew   ();
    void menuSlotPortDeviceAdd   ();
    void menuSlotPortDeviceEdit  ();
    void menuSlotPortDeviceDelete();
    // ----------------------------
    // -----------DEVICE-----------
    // ----------------------------
    void menuSlotDeviceNew           () override;
    void menuSlotDeviceEdit          () override;
    void menuSlotDeviceDelete        () override;
    void menuSlotDeviceImport        () override;
    void menuSlotDeviceExport        () override;
    void menuSlotDeviceMemoryZerro   ();
    void menuSlotDeviceMemoryZerroAll();
    void menuSlotDeviceMemoryImport  ();
    void menuSlotDeviceMemoryExport  ();
    void menuSlotDeviceScriptInit    ();
    void menuSlotDeviceScriptLoop    ();
    void menuSlotDeviceScriptFinal   ();
    // ----------------------------
    // ----------SIMACTION---------
    // ----------------------------
    void menuSlotSimActionNew   ();
    void menuSlotSimActionEdit  ();
    void menuSlotSimActionInsert();
    void menuSlotSimActionDelete();
    void menuSlotSimActionImport();
    void menuSlotSimActionExport();
    // ----------------------------
    // --------SCRIPTMODULE--------
    // ----------------------------
    void menuSlotScriptModuleNew       ();
    void menuSlotScriptModuleOpen      ();
    void menuSlotScriptModuleDelete    ();
    void menuSlotScriptModuleEditParams();
    void menuSlotScriptModuleImport    ();
    void menuSlotScriptModuleExport    ();
    // ----------------------------
    // -----------WINDOW-----------
    // ----------------------------
    void menuSlotWindowDeviceShowAll    ();
    void menuSlotWindowDeviceShowActive ();
    void menuSlotWindowDeviceCloseAll   ();
    void menuSlotWindowDeviceCloseActive();

protected Q_SLOTS: // non menu slots
    void slotSimActionCopy();
    void slotSimActionPaste();
    void slotSimActionSelectAll();
    void setFormat(int format);

private Q_SLOTS:
    void editPort(mbCorePort *port);
    void editDeviceRef(mbServerDeviceRef *device);
    void editDevice(mbServerDevice *device);
    void editAction(mbServerSimAction *action);
    void editActions(const QList<mbServerSimAction*> &actions);
    void contextMenuDevice(mbServerDeviceUi *deviceUi);
    void contextMenuDeviceRef(mbServerDeviceRef *device);
    void contextMenuSimAction(mbServerSimAction *action);
    void contextMenuScriptModule(mbServerScriptModule *scriptModule);
private:
    void editPortPrivate(mbServerPort *port);
    void editDeviceRefPrivate(mbServerDeviceRef *device);
    void editDevicePrivate(mbServerDevice *device);
    void saveProjectInner() override;
    mbServerDeviceUi *activeDeviceUi() const;

private:
    Ui::mbServerUi *ui;
    mb::DigitalFormat m_format;
    QComboBox *m_cmbFormat;
    // Output
    QDockWidget *m_dockOutput;
    mbServerOutputView *m_outputView;
    // SimAction
    mbServerSimActionsUi *m_simActionsUi;
    QDockWidget *m_dockSimActions;
    // ScriptModules
    mbServerScriptModulesUi *m_scriptModulesUi;
    QDockWidget *m_dockScriptModules;
    // Device
    mbServerDeviceManager *m_deviceManager;
    // Script
    mbServerScriptManager *m_scriptManager;
};

#endif // SERVER_UI_H
