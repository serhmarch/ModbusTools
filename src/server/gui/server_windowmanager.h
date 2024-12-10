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
#ifndef SERVER_WINDOWMANAGER_H
#define SERVER_WINDOWMANAGER_H

#include <core/gui/core_windowmanager.h>
#include <project/server_device.h>

class mbServerDevice;
class mbServerDataView;

class mbServerUi;
class mbServerDeviceManager;
class mbServerScriptManager;
class mbServerDeviceUi;
class mbServerDataViewManager;
class mbServerDataViewUi;
class mbServerDeviceScriptEditor;

class mbServerWindowManager : public mbCoreWindowManager
{
    Q_OBJECT

public:
    struct Strings : public mbCoreWindowManager::Strings
    {
        const QString prefixDevice;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerWindowManager(mbServerUi *ui, mbServerDeviceManager *deviceManager,
                                                   mbServerScriptManager *scriptManager,
                                                   mbServerDataViewManager *dataViewManager);

public: // 'mbCoreWindowManager'-interface
    inline mbServerUi *ui() const { return reinterpret_cast<mbServerUi*>(uiCore()); }
    inline mbServerDataViewManager *dataViewManager() { return reinterpret_cast<mbServerDataViewManager*>(dataViewManagerCore()); }
    inline mbServerDataView *activeDataView() const { return reinterpret_cast<mbServerDataView*>(activeDataViewCore()); }
    inline void setActiveDataView(mbServerDataView *dataView) { setActiveDataViewCore(reinterpret_cast<mbCoreDataView*>(dataView)); }
    QMdiSubWindow *getMdiSubWindowForNameWithPrefix(const QString &nameWithPrefix) const override;
    QString getMdiSubWindowNameWithPrefix(const QMdiSubWindow *sw) const override;

public:
    mbServerDevice *activeDevice() const;
    void setActiveDevice(mbServerDevice *device);

public:
    void showDeviceScript(mbServerDevice *device, mbServerDevice::ScriptType scriptType);
    void setActiveScriptEditor(mbServerDeviceScriptEditor *scriptEditor);

public Q_SLOTS:
    void showDeviceUi(const mbServerDeviceUi *ui);
    void actionWindowDeviceShowAll();
    void actionWindowDeviceShowActive();
    void actionWindowDeviceCloseAll();
    void actionWindowDeviceCloseActive();
    void actionWindowShowAll() override;
    void actionWindowCloseAll() override;

private Q_SLOTS:
    void deviceUiAdd(mbServerDeviceUi *ui);
    void deviceUiRemove(mbServerDeviceUi *ui);

private Q_SLOTS:
    void scriptEditorAdd(mbServerDeviceScriptEditor *ui);
    void scriptEditorRemove(mbServerDeviceScriptEditor *ui);

private Q_SLOTS:
    void subWindowActivated(QMdiSubWindow *sw) override;

private:
    mbServerDeviceManager *m_deviceManager;
    mbServerScriptManager *m_scriptManager;

    // Devices
    typedef QList<QMdiSubWindow*> Devices_t;
    Devices_t m_devices;

    typedef QList<QMdiSubWindow*> ScriptEditor_t;
    ScriptEditor_t m_scriptEditors;
};

#endif // SERVER_WINDOWMANAGER_H
