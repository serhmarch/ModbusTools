#ifndef CLIENT_SENDBYTESUI_H
#define CLIENT_SENDBYTESUI_H

#include <gui/dialogs/core_dialogbase.h>
#include <client_global.h>

class mbCoreProject;
class mbCorePort;
class mbClientProject;
class mbClientPort;
class mbClientSendBytesListModel;

namespace Ui {
class mbClientSendBytesUi;
}

class mbClientSendBytesUi : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString prefix        ;
        const QString port          ;
        const QString byteFormat    ;
        const QString isAddChecksum ;
        const QString checksum      ;
        const QString text          ;
        const QString list          ;
        const QString period        ;
        const QString isLoop        ;
        const QString CRC           ;
        const QString LRC           ;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientSendBytesUi(QWidget *parent = nullptr);
    ~mbClientSendBytesUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

private:
    void timerEvent(QTimerEvent *event) override;

private Q_SLOTS:
    void setProject(mbCoreProject *p);
    void addPort(mbCorePort *port);
    void removePort(mbCorePort *port);
    void renamePort(mbCorePort *port, const QString newName);

private Q_SLOTS: // list
    void slotListShowHide();
    void slotListInsert  ();
    void slotListEdit    ();
    void slotListRemove  ();
    void slotListClear   ();
    void slotListMoveUp  ();
    void slotListMoveDown();
    void slotListImport  ();
    void slotListExport  ();

private Q_SLOTS: // send
    void slotSendOne ();
    void slotSendList();
    void slotStop    ();

private Q_SLOTS:
    void slotBytesTx(const QByteArray &bytes);
    void slotBytesRx(const QByteArray &bytes);
    void slotAsciiTx(const QByteArray &bytes);
    void slotAsciiRx(const QByteArray &bytes);
    void messageCompleted();
    void getListItem(const QModelIndex &index);

private:
    QStringList getListItems() const;
    void setListItems(const QStringList& list);
    int currentListIndex() const;
    void setCurrentListIndex(int i);
    mbClientPort *currentPort() const;
    void createMessage();
    void createMessageList();
    void sendMessage();
    void prepareToSend(mbClientRunMessageRaw *msg);
    void startSendMessages();
    void stopSendMessages();
    void setEnableParams(bool enable);
    QByteArray parseString(const QString &source);

private:
    Ui::mbClientSendBytesUi *ui;

private:
    mbClientProject *m_project;
    QList<mbClientRunMessagePtr> m_messageList;
    int m_messageIndex;
    mbClientSendBytesListModel *m_list;
    int m_timer;
};

#endif // CLIENT_SENDBYTESUI_H
