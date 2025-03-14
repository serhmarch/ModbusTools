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
#ifndef CLIENT_DIALOGSENDMESSAGE_H
#define CLIENT_DIALOGSENDMESSAGE_H

#include <gui/dialogs/core_dialogbase.h>

class mbCoreProject;
class mbClientProject;
class mbCoreDevice;
class mbClientDevice;

#include <client_global.h>

class mbCoreAddressWidget;

namespace Ui {
class mbClientSendMessageUi;
}

class mbClientSendMessageUi : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString prefix         ;
        const QString function       ;
        const QString readAdrType    ;
        const QString readAddress    ;
        const QString readFormat     ;
        const QString readCount      ;
        const QString writeAdrType   ;
        const QString writeAddress   ;
        const QString writeFormat    ;
        const QString writeCount     ;
        const QString writeData      ;
        const QString period         ;
        const QString writeMaskAnd   ;
        const QString writeMaskOr    ;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientSendMessageUi(QWidget *parent = nullptr);
    ~mbClientSendMessageUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

private Q_SLOTS:
    void setModbusAddresNotation(mb::AddressNotation notation);
    void setProject(mbCoreProject *p);
    void addDevice(mbCoreDevice *device);
    void removeDevice(mbCoreDevice *device);
    void renameDevice(mbCoreDevice *device, const QString newName);
    void setCurrentFuncIndex(int func);
    void sendOne();
    void sendPeriodically();
    void stopSending();

private Q_SLOTS:
    void slotBytesTx(const QByteArray &bytes);
    void slotBytesRx(const QByteArray &bytes);
    void slotAsciiTx(const QByteArray &bytes);
    void slotAsciiRx(const QByteArray &bytes);
    void messageCompleted();

private:
    void createMessage();
    mbClientDevice *currentDevice() const;
    void setEnableParams(bool v);
    int getReadAddress() const;
    void setReadAddress(int v);
    int getWriteAddress() const;
    void setWriteAddress(int v);

private:
    void timerEvent(QTimerEvent *event) override;

private:
    void fillForm(mbClientDevice *device, const mbClientRunMessagePtr &message);
    void fillData(mbClientDevice *device, mbClientRunMessagePtr &message);
    QStringList toStringListBits(const QByteArray &data, uint16_t count);
    QStringList toStringListNumbers(const QByteArray &data, mb::Format format, mbClientDevice *device);
    QByteArray fromStringListBits(const QStringList &ls);
    QByteArray fromStringListNumbers(const QStringList &ls, mb::Format format, mbClientDevice *device);
    bool fromStringNumber(mb::Format format, const QString &v, void *buff);
    QStringList params(const QString &s);
    uint8_t getCurrentFuncNum() const;
    void setCurrentFuncNum(uint8_t func);

private:
    Ui::mbClientSendMessageUi *ui;
    mbCoreAddressWidget *m_readAddress;
    mbCoreAddressWidget *m_writeAddress;

private:
    mbClientProject *m_project;
    mbClientRunMessagePtr m_message;
    QList<uint8_t> m_funcNums;
    int m_timer;
};

#endif // CLIENT_DIALOGSENDMESSAGE_H
