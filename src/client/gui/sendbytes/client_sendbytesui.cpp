#include "client_sendbytesui.h"
#include "ui_client_sendbytesui.h"

#include <QTextStream>

#include <client.h>

#include <project/client_project.h>
#include <project/client_port.h>

#include <runtime/client_runmessage.h>

#include <gui/client_ui.h>
#include <gui/dialogs/client_dialogs.h>

#include "client_sendbyteslistmodel.h"

mbClientSendBytesUi::Strings::Strings() :
    prefix        (QStringLiteral("Ui.SendBytes.")),
    port          (QStringLiteral("port")),
    byteFormat    (QStringLiteral("byteFormat")),
    isAddChecksum (QStringLiteral("isAddChecksum")),
    checksum      (QStringLiteral("checksum")),
    text          (QStringLiteral("text")),
    list          (QStringLiteral("list")),
    period        (QStringLiteral("period")),
    isLoop        (QStringLiteral("isLoop")),
    CRC           (QStringLiteral("CRC")),
    LRC           (QStringLiteral("LRC"))
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
    m_timer = 0;

    m_list = new mbClientSendBytesListModel(this);
    ui->lsList->setModel(m_list);

    mbClient *core = mbClient::global();

    QComboBox *cmb;
    QSpinBox *sp;

    cmb = ui->cmbByteFormat;
    mb::fillDigitalFormatComboBox(ui->cmbByteFormat);
    cmb->setCurrentText(mb::enumDigitalFormatKey(mb::Hex));

    sp = ui->spPeriod;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);
    sp->setValue(1000);

    ui->rdCRC->setChecked(true);
    ui->grChecksum->setEnabled(false);
    connect(ui->chbAddCheck, &QCheckBox::toggled, ui->grChecksum, &QGroupBox::setEnabled);

    connect(ui->btnListShowHide, &QPushButton::clicked, this, &mbClientSendBytesUi::slotListShowHide);
    connect(ui->btnListInsert  , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListInsert  );
    connect(ui->btnListEdit    , &QPushButton::clicked, this, &mbClientSendBytesUi::slotListEdit    );
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

    connect(ui->lsList, &QAbstractItemView::doubleClicked, this, &mbClientSendBytesUi::getListItem);

    connect(core, &mbClient::projectChanged, this, &mbClientSendBytesUi::setProject);
    connect(core, &mbClient::statusChanged , this, &mbClientSendBytesUi::setRunStatus);
    setProject(core->project());
}

mbClientSendBytesUi::~mbClientSendBytesUi()
{
    delete ui;
}

MBSETTINGS mbClientSendBytesUi::cachedSettings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS m = mbCoreDialogBase::cachedSettings();

    m[s.port         ] = ui->cmbPort        ->currentText();
    m[s.byteFormat   ] = ui->cmbByteFormat  ->currentText();
    m[s.isAddChecksum] = ui->chbAddCheck    ->isChecked();
    m[s.checksum     ] = ui->rdLRC          ->isChecked() ? s.LRC : s.CRC;
    m[s.text         ] = ui->txtData        ->toPlainText();
    m[s.list         ] = this               ->getListItems();
    m[s.period       ] = ui->spPeriod       ->value();
    m[s.isLoop       ] = ui->chbUseLoop     ->isChecked();
    m[s.wGeometry    ] = this               ->saveGeometry();

    return m;
}

void mbClientSendBytesUi::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);

    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(s.port         ); if (it != end) ui->cmbPort       ->setCurrentText (it.value().toString()         );
    it = m.find(s.byteFormat   ); if (it != end) ui->cmbByteFormat ->setCurrentText (it.value().toString()         );
    it = m.find(s.isAddChecksum); if (it != end) ui->chbAddCheck   ->setChecked     (it.value().toBool()           );
    it = m.find(s.checksum     ); if (it != end) ui->rdLRC         ->setChecked     (it.value().toString() == s.LRC);
    it = m.find(s.text         ); if (it != end) ui->txtData       ->setPlainText   (it.value().toString()         );
    it = m.find(s.list         ); if (it != end) this              ->setListItems   (it.value().toStringList()     );
    it = m.find(s.period       ); if (it != end) ui->spPeriod      ->setValue       (it.value().toInt()            );
    it = m.find(s.isLoop       ); if (it != end) ui->chbUseLoop    ->setChecked     (it.value().toBool()           );
    it = m.find(s.wGeometry    ); if (it != end) this              ->restoreGeometry(it.value().toByteArray()      );
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

void mbClientSendBytesUi::setRunStatus(int status)
{
    if (status == mbClient::Stopped)
        slotStop();
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
    QByteArray bytes = parseString(ui->txtData->toPlainText());
    if (bytes.count() == 0)
        return;
    int i = currentListIndex();
    m_list->insertMessage(i, bytes);
}

void mbClientSendBytesUi::slotListEdit()
{
    int i = currentListIndex();
    if (i < 0)
        return;
    QByteArray bytes = parseString(ui->txtData->toPlainText());
    if (bytes.count() == 0)
        return;
    m_list->editMessage(i, bytes);
}

void mbClientSendBytesUi::slotListRemove()
{
    int i = currentListIndex();
    m_list->removeMessage(i);
}

void mbClientSendBytesUi::slotListClear()
{
    m_list->clear();
}

void mbClientSendBytesUi::slotListMoveUp()
{
    int i = currentListIndex();
    if (m_list->moveUp(i))
        setCurrentListIndex(i-1);
}

void mbClientSendBytesUi::slotListMoveDown()
{
    int i = currentListIndex();
    if (m_list->moveDown(i))
        setCurrentListIndex(i+1);
}

void mbClientSendBytesUi::slotListImport()
{
    mbClientDialogs *dialogs = mbClient::global()->ui()->dialogs();
    QString file = dialogs->getOpenFileName(this,
                                            QString("Import Bytes"),
                                            QString(),
                                            dialogs->getFilterString(mbCoreDialogs::Filter_AllFiles));
    if (!file.isEmpty())
    {
        QFile qf(file);
        if (qf.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            m_list->clear();
            QTextStream in(&qf);
            while (!in.atEnd())
            {
                QString line = in.readLine();
                QByteArray bytes = parseString(line);
                if (bytes.count())
                    m_list->addMessage(bytes);
            }
            qf.close();
        }
    }
}

void mbClientSendBytesUi::slotListExport()
{
    mbClientDialogs *dialogs = mbClient::global()->ui()->dialogs();
    QString file = dialogs->getSaveFileName(this,
                                              QString("Export Bytes"),
                                              QString(),
                                              dialogs->getFilterString(mbCoreDialogs::Filter_AllFiles));
    if (!file.isEmpty())
    {
        QFile qf(file);
        if (qf.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&qf);
            for (int i = 0; i < m_list->rowCount(); ++i)
            {
                QString line = m_list->messageRepr(i);
                out << line << '\n';
            }
            qf.close();
        }
    }
}

void mbClientSendBytesUi::slotSendOne()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    createMessage();
    if (m_messageList.count())
    {
        startSendMessages();
    }
}

void mbClientSendBytesUi::slotSendList()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    createMessageList();
    if (m_messageList.count())
    {
        startSendMessages();
    }
}

void mbClientSendBytesUi::slotStop()
{
    stopSendMessages();
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
    mbClientRunMessagePtr msg = qobject_cast<mbClientRunMessage*>(sender());
    if (msg)
    {
        ui->lnStatus   ->setText(mb::toString(msg->status   ()));
        ui->lnTimestamp->setText(mb::toString(msg->timestamp()));
        if (isTimerStopped())
            stopSendMessages();
    }
}

void mbClientSendBytesUi::getListItem(const QModelIndex &index)
{
    int i = index.row();
    if (i < 0)
        return;
    // TODO: check if txt already has data and ask user to confirm
    QString s = m_list->messageRepr(i);
    ui->txtData->setPlainText(s);
}

void mbClientSendBytesUi::timerEvent(QTimerEvent */*event*/)
{
    mbClientRunMessagePtr msg = m_messageList.value(m_messageIndex);
    if (msg && msg->isCompleted())
    {
        msg->clearCompleted();
        ++m_messageIndex;
        if (m_messageIndex >= m_messageList.count())
        {
            if (ui->chbUseLoop->isChecked())
                m_messageIndex = 0;
            else
            {
                stopSendMessages();
                return;
            }
        }
        this->sendMessage();
    }
}

QStringList mbClientSendBytesUi::getListItems() const
{
    QStringList res;
    for (int i = 0; i < m_list->rowCount(); ++i)
    {
        QString line = m_list->messageRepr(i);
        res.append(line);
    }
    return res;
}

void mbClientSendBytesUi::setListItems(const QStringList &list)
{
    Q_FOREACH(const QString &line, list)
    {
        QByteArray bytes = parseString(line);
        if (bytes.count())
            m_list->addMessage(bytes);
    }
}

int mbClientSendBytesUi::currentListIndex() const
{
    auto indexes = ui->lsList->selectionModel()->selectedIndexes();
    if (indexes.count())
        return indexes.first().row();
    return -1;
}

void mbClientSendBytesUi::setCurrentListIndex(int i)
{
    QModelIndex index = m_list->index(i, 0);
    ui->lsList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
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
    m_messageIndex = 0;
    m_messageList.clear();
    QByteArray bytes = parseString(ui->txtData->toPlainText());
    if (bytes.count() == 0)
        return;
    mbClientRunMessageRaw *msg = m_list->createMessage(bytes);
    m_messageList.append(msg);
}

void mbClientSendBytesUi::createMessageList()
{
    m_messageIndex = 0;
    m_messageList.clear();
    auto msgList = m_list->messages();
    if (msgList.count() == 0)
        return;
    Q_FOREACH (auto ptr, msgList)
    {
        mbClientRunMessageRaw *msg = ptr.data();
        m_messageList.append(msg);
    }
}

void mbClientSendBytesUi::sendMessage()
{
    if (m_messageList.count() == 0)
        return;
    mbClientPort *port = currentPort();
    if (!port)
        return;
    mbClientRunMessagePtr msg = m_messageList.value(m_messageIndex);
    if (msg)
    {
        mbClient::global()->sendPortMessage(port->handle(), msg);
    }
}

void mbClientSendBytesUi::prepareToSend(mbClientRunMessageRaw *msg)
{
    uint8_t *buff = reinterpret_cast<uint8_t*>(msg->inputBuffer());
    uint16_t sz = msg->inputBaseCount();
    if (ui->chbAddCheck->isChecked())
    {
        if (ui->rdCRC->isChecked())
        {
            uint16_t crc = Modbus::crc16(buff, sz);
            // TODO: check set array bounds
            buff[sz  ] = reinterpret_cast<uint8_t*>(&crc)[0];
            buff[sz+1] = reinterpret_cast<uint8_t*>(&crc)[1];
            msg->setInputCount(sz+2);
        }
        else if (ui->rdLRC->isChecked())
        {
            uint8_t lrc = Modbus::lrc(buff, sz);
            // TODO: check set array bounds
            buff[sz  ] = lrc;
            buff[sz+1] = '\r'; // CR
            buff[sz+2] = '\n'; // LF
            msg->setInputCount(sz+3);
        }
    }
    else
        msg->setInputCount(sz);
    connect(msg, &mbClientRunMessage::signalBytesTx, this, &mbClientSendBytesUi::slotBytesTx     );
    connect(msg, &mbClientRunMessage::signalBytesRx, this, &mbClientSendBytesUi::slotBytesRx     );
    connect(msg, &mbClientRunMessage::signalAsciiTx, this, &mbClientSendBytesUi::slotAsciiTx     );
    connect(msg, &mbClientRunMessage::signalAsciiRx, this, &mbClientSendBytesUi::slotAsciiRx     );
    connect(msg, &mbClientRunMessage::completed    , this, &mbClientSendBytesUi::messageCompleted);
}

void mbClientSendBytesUi::clearAfterSend(mbClientRunMessage *msg)
{
    msg->disconnect(this);
}

void mbClientSendBytesUi::startSendMessages()
{
    setEnableParams(false);
    ui->txtModbusTx->clear();
    ui->txtModbusRx->clear();
    Q_FOREACH (auto msg, m_messageList)
    {
        prepareToSend(reinterpret_cast<mbClientRunMessageRaw*>(msg.data()));
    }
    sendMessage();
    if ((m_messageList.count() > 1 || ui->chbUseLoop->isChecked()) && isTimerStopped())
    {
        m_timer = startTimer(ui->spPeriod->value());;
    }
}

void mbClientSendBytesUi::stopSendMessages()
{
    if (isTimerRunning())
    {
        killTimer(m_timer);
        m_timer = 0;
    }
    Q_FOREACH (auto msg, m_messageList)
    {
        clearAfterSend(msg);
    }
    m_messageList.clear();
    setEnableParams(true);
}

void mbClientSendBytesUi::setEnableParams(bool enable)
{
    ui->cmbPort        ->setEnabled(enable);
    ui->cmbByteFormat  ->setEnabled(enable);
    ui->chbAddCheck    ->setEnabled(enable);
    ui->txtData        ->setEnabled(enable);
    ui->lsList         ->setEnabled(enable);
    ui->btnListInsert  ->setEnabled(enable);
    ui->btnListEdit    ->setEnabled(enable);
    ui->btnListImport  ->setEnabled(enable);
    ui->btnListMoveUp  ->setEnabled(enable);
    ui->btnListMoveDown->setEnabled(enable);
    ui->btnListRemove  ->setEnabled(enable);
    ui->btnListClear   ->setEnabled(enable);
    ui->spPeriod       ->setEnabled(enable);
    ui->chbUseLoop     ->setEnabled(enable);
    ui->btnSendOne     ->setEnabled(enable);
    ui->btnSendList    ->setEnabled(enable);
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
