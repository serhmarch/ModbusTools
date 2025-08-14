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
#ifndef CLIENT_UI_H
#define CLIENT_UI_H

#include <gui/core_ui.h>

class QUndoStack;
class QLabel;

class mbCoreDataViewUi;

class mbClient;
class mbClientBuilder;
class mbClientProject;
class mbClientPort;
class mbClientDevice;
class mbClientDataView;
class mbClientDialogs;
class mbClientProjectUi;
class mbClientWindowManager;
class mbClientDataViewManager;
class mbClientSendMessageUi;
class mbClientSendBytesUi;
class mbClientScannerUi;

namespace Ui {
class mbClientUi;
}

class mbClientUi : public mbCoreUi
{
    Q_OBJECT

public:
    explicit mbClientUi(mbClient* core, QWidget *parent = nullptr);
    ~mbClientUi();

public:
    inline mbClient* core() const { return reinterpret_cast<mbClient*>(baseCore()); }
    inline mbClientBuilder* builder() const { return reinterpret_cast<mbClientBuilder*>(builderCore()); }
    inline mbClientDialogs* dialogs() const { return reinterpret_cast<mbClientDialogs*>(dialogsCore()); }
    inline mbClientProjectUi* projectUi() const { return reinterpret_cast<mbClientProjectUi*>(projectUiCore()); }
    inline mbClientWindowManager *windowManager() const { return reinterpret_cast<mbClientWindowManager*>(windowManagerCore()); }
    inline mbClientDataViewManager *dataViewManager() const { return reinterpret_cast<mbClientDataViewManager*>(dataViewManagerCore()); }

public:
    void initialize() override;

public: // settings
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

private Q_SLOTS:
    // ----------------------------
    // ------------EDIT------------
    // ----------------------------
    void menuSlotEditPaste () override;
    void menuSlotEditInsert() override;
    void menuSlotEditEdit  () override;
    void menuSlotEditDelete() override;
    // ----------------------------
    // ------------PORT------------
    // ----------------------------
    void menuSlotPortNew   () override;
    void menuSlotPortEdit  () override;
    void menuSlotPortDelete() override;
    void menuSlotPortNewDevice();
    void menuSlotPortClearAllDevice();
    // ----------------------------
    // -----------DEVICE-----------
    // ----------------------------
    void menuSlotDeviceNew   () override;
    void menuSlotDeviceEdit  () override;
    void menuSlotDeviceDelete() override;
    void menuSlotDeviceImport() override;
    void menuSlotDeviceExport() override;
    // ----------------------------
    // ----------DATA VIEW---------
    // ----------------------------
    void menuSlotDataViewItemNew    () override;
    void menuSlotDataViewItemEdit   () override;
    void menuSlotDataViewItemInsert () override;
    void menuSlotDataViewItemDelete () override;
    void menuSlotDataViewImportItems() override;
    void menuSlotDataViewExportItems() override;
    void menuSlotDataViewNew        () override;
    void menuSlotDataViewEdit       () override;
    void menuSlotDataViewInsert     () override;
    void menuSlotDataViewDelete     () override;
    void menuSlotDataViewImport     () override;
    void menuSlotDataViewExport     () override;
    // ----------------------------
    // ------------TOOLS-----------
    // ----------------------------
    void menuSlotToolsSendMessage();
    void menuSlotToolsSendBytes();
    void menuSlotToolsScanner();

private Q_SLOTS:
    void contextMenuDevice(mbClientDevice *device);

private:
    void editPort(mbCorePort *port);
    void editDevice(mbClientDevice *device);

private:
    MBSETTINGS getDataViewItemCreateSettings() override;

private:
    Ui::mbClientUi *ui;
    // status bar labels
    mbClientSendMessageUi *m_sendMessageUi;
    mbClientSendBytesUi *m_sendBytesUi;
    mbClientScannerUi *m_scannerUi;
};


#endif // CLIENT_UI_H
