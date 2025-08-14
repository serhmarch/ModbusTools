#include "client_sendbytesui.h"
#include "ui_client_sendbytesui.h"

#include <client.h>

#include <project/client_project.h>
#include <project/client_port.h>

#include <runtime/client_runmessage.h>

mbClientSendBytesUi::Strings::Strings() :
    prefix(QStringLiteral("Ui.SendBytes."))
{
}

const mbClientSendBytesUi::Strings &mbClientSendBytesUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientSendBytesUi::mbClientSendBytesUi(QWidget *parent)
    : mbCoreDialogBase(Strings::instance().prefix, parent)
    , ui(new Ui::mbClientSendBytesUi)
{
    ui->setupUi(this);
    m_project = nullptr;

    mbClient *core = mbClient::global();

    QComboBox *cmb = ui->cmbByteFormat;
    mb::fillDigitalFormatComboBox(ui->cmbByteFormat);
    cmb->setCurrentText(mb::enumDigitalFormatKey(mb::Hex));

    ui->rdCRC->setChecked(true);
    ui->grChecksum->setEnabled(false);
    connect(ui->chbAddCheck, &QCheckBox::clicked, ui->grChecksum, &QGroupBox::setEnabled);

    connect(ui->btnListShowHide, &QPushButton::clicked, this, &mbClientSendBytesUi::slotListShowHide);
    connect(ui->btnListInsert  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListInsert  );
    connect(ui->btnListRemove  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListRemove  );
    connect(ui->btnListClear   , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListClear   );
    connect(ui->btnListMoveUp  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListMoveUp  );
    connect(ui->btnListMoveDown, &QPushButton::clicked, this, &mbClientSendBytesUi::slotListMoveDown);
    connect(ui->btnListImport  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListImport  );
    connect(ui->btnListExport  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListExport  );

    ui->btnListShowHide->click();

    connect(ui->btnSendOne , &QPushButton::clicked, this, &mbClientSendBytesUi::slotSendOne );
    connect(ui->btnSendList, &QPushButton::clicked, this, &mbClientSendBytesUi::slotSendList);
    connect(ui->btnStop    , &QPushButton::clicked, this, &mbClientSendBytesUi::slotStop    );
    connect(ui->btnClose   , &QPushButton::clicked, this, &QDialog::close);

    connect(core, &mbClient::projectChanged, this, &mbClientSendBytesUi::setProject);
    setProject(core->project());
}

mbClientSendBytesUi::~mbClientSendBytesUi()
{
    delete ui;
}

void mbClientSendBytesUi::setProject(mbCoreProject *p)
{
    mbClientProject *project = static_cast<mbClientProject*>(p);
    if (m_project != project)
    {
        if (m_project)
        {
            m_project->disconnect(this);
            ui->cmbPort->clear();
        }
        m_project = project;
        if (m_project)
        {
            QList<mbClientPort*> ports = m_project->ports();
            connect(m_project, &mbClientProject::portAdded   , this, &mbClientSendBytesUi::addPort   );
            connect(m_project, &mbClientProject::portRemoving, this, &mbClientSendBytesUi::removePort);
            connect(m_project, &mbClientProject::portRenaming, this, &mbClientSendBytesUi::renamePort);
            Q_FOREACH (mbClientPort *d, ports)
                addPort(d);
        }
    }
}

void mbClientSendBytesUi::addPort(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->insertItem(i, port->name());
}

void mbClientSendBytesUi::removePort(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->removeItem(i);
}

void mbClientSendBytesUi::renamePort(mbCorePort *port, const QString newName)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->setItemText(i, newName);
}

void mbClientSendBytesUi::slotListShowHide()
{
    bool isVisible = ui->grList->isVisible();
    if (isVisible)
        ui->btnListShowHide->setText("Show");
    else
        ui->btnListShowHide->setText("Hide");
    ui->grList->setVisible(!isVisible);
}

void mbClientSendBytesUi::slotListInsert()
{

}

void mbClientSendBytesUi::slotListRemove()
{

}

void mbClientSendBytesUi::slotListClear()
{

}

void mbClientSendBytesUi::slotListMoveUp()
{

}

void mbClientSendBytesUi::slotListMoveDown()
{

}

void mbClientSendBytesUi::slotListImport()
{

}

void mbClientSendBytesUi::slotListExport()
{

}

void mbClientSendBytesUi::slotSendOne()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    createMessage();
    sendMessages();
}

void mbClientSendBytesUi::slotSendList()
{

}

void mbClientSendBytesUi::slotStop()
{

}

void mbClientSendBytesUi::slotBytesTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientSendBytesUi::slotBytesRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientSendBytesUi::slotAsciiTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientSendBytesUi::slotAsciiRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientSendBytesUi::messageCompleted()
{
    mbClientRunMessagePtr message = qobject_cast<mbClientRunMessage*>(sender());
    if (message)
    {
        ui->lnStatus   ->setText(mb::toString(message->status   ()));
        ui->lnTimestamp->setText(mb::toString(message->timestamp()));
    }
}

mbClientPort *mbClientSendBytesUi::currentPort() const
{
    if (m_project)
    {
        int i = ui->cmbPort->currentIndex();
        return m_project->port(i);
    }
    return nullptr;
}

void mbClientSendBytesUi::createMessage()
{
    m_messageList.clear();
    QByteArray bytes = parseString(ui->txtData->toPlainText());
    if (bytes.count() == 0)
        return;
    mbClientRunMessageRaw *msg = new mbClientRunMessageRaw();
    int count = bytes.count();
    if (count > msg->innerBufferSize())
        count = msg->innerBufferSize();
    memcpy(msg->innerBuffer(), bytes.data(), count);
    msg->setCount(count);
    connect(msg, &mbClientRunMessage::signalBytesTx, this, &mbClientSendBytesUi::slotBytesTx     );
    connect(msg, &mbClientRunMessage::signalBytesRx, this, &mbClientSendBytesUi::slotBytesRx     );
    connect(msg, &mbClientRunMessage::signalAsciiTx, this, &mbClientSendBytesUi::slotAsciiTx     );
    connect(msg, &mbClientRunMessage::signalAsciiRx, this, &mbClientSendBytesUi::slotAsciiRx     );
    connect(msg, &mbClientRunMessage::completed    , this, &mbClientSendBytesUi::messageCompleted);
    m_messageList.append(msg);
    m_messageIndex = 0;
}

void mbClientSendBytesUi::createMessageList()
{

}

void mbClientSendBytesUi::sendMessages()
{
    mbClientPort *port = currentPort();
    if (!port)
        return;
    mbClientRunMessagePtr message = m_messageList.at(m_messageIndex);
    m_messageIndex = (m_messageIndex+1) % m_messageList.count();
    mbClient::global()->sendPortMessage(port->handle(), message);
}

QByteArray mbClientSendBytesUi::parseString(const QString &source)
{
    QByteArray res;
    QString s;
    //mb::DigitalFormat format = mb::enumDigitalFormatValue(ui->cmbByteFormat->currentText());
    for (int i = 0; i < source.length(); i++)
    {
        ushort c = source.at(i).unicode();
        switch (c)
        {
        case '0':case '1':case '2':case '3':case '4':case '5':case '6':case '7':case '8':case '9':
        case 'A':case 'B':case 'C':case 'D':case 'E':case 'F':
        case 'a':case 'b':case 'c':case 'd':case 'e':case 'f':
            s += c;
            if (s.length() < 2)
                break;
            // no need break;
        default:
            if (s.length())
            {
                res.append(static_cast<char>(s.toInt(nullptr, 16)));
                s.clear();
            }
            break;
        }
    }
    if (s.length())
        res.append(static_cast<char>(s.toInt(nullptr, 16)));
    return res;
}
