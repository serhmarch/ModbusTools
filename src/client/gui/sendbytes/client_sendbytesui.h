#ifndef CLIENT_SENDBYTESUI_H
#define CLIENT_SENDBYTESUI_H

#include <gui/dialogs/core_dialogbase.h>
#include <client_global.h>

class mbCoreProject;
class mbCorePort;
class mbClientProject;
class mbClientPort;

namespace Ui {
class mbClientSendBytesUi;
}

class mbClientSendBytesUi : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString prefix;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientSendBytesUi(QWidget *parent = nullptr);
    ~mbClientSendBytesUi();

private Q_SLOTS:
    void setProject(mbCoreProject *p);
    void addPort(mbCorePort *port);
    void removePort(mbCorePort *port);
    void renamePort(mbCorePort *port, const QString newName);

private Q_SLOTS: // list
    void slotListShowHide();
    void slotListInsert  ();
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

private:
    mbClientPort *currentPort() const;
    void createMessage();
    void createMessageList();
    void sendMessages();
    QByteArray parseString(const QString &source);

private:
    Ui::mbClientSendBytesUi *ui;

private:
    mbClientProject *m_project;
    QList<mbClientRunMessagePtr> m_messageList;
    int m_messageIndex;
};

#endif // CLIENT_SENDBYTESUI_H
