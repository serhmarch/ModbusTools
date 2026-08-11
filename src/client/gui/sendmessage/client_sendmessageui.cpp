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
#include "client_sendmessageui.h"
#include "ui_client_sendmessageui.h"

#include <QTextStream>

#include <client.h>

#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

#include <runtime/client_runmessage.h>

#include <gui/widgets/core_addresswidget.h>

#include <gui/client_ui.h>
#include <gui/dialogs/client_dialogs.h>

#include "client_sendmessagedefaultwidget.h"
#include "client_sendmessagewritesinglecoilwidget.h"
#include "client_sendmessagewritesingleregisterwidget.h"
#include "client_sendmessagereadexceptionstatuswidget.h"
#include "client_sendmessagediagnosticswidget.h"
#include "client_sendmessagepggetcommeventcounterwidget.h"
#include "client_sendmessagepggetcommeventlogwidget.h"
#include "client_sendmessagereportserveridwidget.h"
#include "client_sendmessagefilerecordswidget.h"
#include "client_sendmessagemaskwriteregisterwidget.h"
#include "client_sendmessagereadwritemultipleregisterswidget.h"
#include "client_sendmessagereadfifoqueuewidget.h"
#include "client_sendmessagereaddeviceidwidget.h"

#include "client_sendmessagelistmodel.h"

mbClientSendMessageUi::Strings::Strings() :
    prefix  (QStringLiteral("Ui.SendMessage.")),
    sendTo  (prefix+QStringLiteral("sendTo")),
    unit    (prefix+QStringLiteral("unit")),
    function(prefix+QStringLiteral("function")),
    list    (prefix+QStringLiteral("list")),
    period  (prefix+QStringLiteral("period"))
{
}

const mbClientSendMessageUi::Strings &mbClientSendMessageUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientSendMessageUi::mbClientSendMessageUi(QWidget *parent) : mbCoreDialogBase(Strings::instance().prefix, parent),
                                                                ui(new Ui::mbClientSendMessageUi)
{
    ui->setupUi(this);
    m_list = new mbClientSendMessageListModel(&m_converter, this);
    ui->lsList->setModel(m_list);
    m_project = nullptr;
    m_sendTo = -1;
    m_timer = 0;

    QStringList ls;
    QComboBox *cmb;
    QSpinBox *sp;

    mbClient *core = mbClient::global();

    // -----------------------------------------------------------------------
    // Unit
    sp = ui->spUnit;
    sp->setMinimum(0);
    sp->setMaximum(255);

    sp = ui->spPeriod;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);
    sp->setValue(1000);

    cmb = ui->cmbFunction;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentFuncIndex(int)));

    addFunctionWidget(new mbClientSendMessageReadCoilsWidget                 (this, this));
    addFunctionWidget(new mbClientSendMessageReadDiscreteInputsWidget        (this, this));
    addFunctionWidget(new mbClientSendMessageReadHoldingRegistersWidget      (this, this));
    addFunctionWidget(new mbClientSendMessageReadInputRegistersWidget        (this, this));
    addFunctionWidget(new mbClientSendMessageWriteSingleCoilWidget           (this, this));
    addFunctionWidget(new mbClientSendMessageWriteSingleRegisterWidget       (this, this));
    addFunctionWidget(new mbClientSendMessageReadExceptionStatusWidget       (this, this));
    addFunctionWidget(new mbClientSendMessageDiagnosticsWidget               (this, this));
    addFunctionWidget(new mbClientSendMessageGetCommEventCounterWidget       (this, this));
    addFunctionWidget(new mbClientSendMessageGetCommEventLogWidget           (this, this));
    addFunctionWidget(new mbClientSendMessageWriteMultipleCoilsWidget        (this, this));
    addFunctionWidget(new mbClientSendMessageWriteMultipleRegistersWidget    (this, this));
    addFunctionWidget(new mbClientSendMessageReportServerIdWidget            (this, this));
    addFunctionWidget(new mbClientSendMessageReadFileRecordsWidget           (this, this));
    addFunctionWidget(new mbClientSendMessageWriteFileRecordsWidget          (this, this));
    addFunctionWidget(new mbClientSendMessageMaskWriteRegisterWidget         (this, this));
    addFunctionWidget(new mbClientSendMessageReadWriteMultipleRegistersWidget(this, this));
    addFunctionWidget(new mbClientSendMessageReadFIFOQueueWidget             (this, this));

    Q_FOREACH (uint8_t funcNum, m_funcNums)
    {
        cmb->addItem(QString("%1 - %2")
                         .arg(funcNum, 2, 10, QChar('0'))
                         .arg(mb::ModbusFunctionString(funcNum))
                     );
    }
    addFunctionWidget(new mbClientSendMessageReadDeviceIdWidget(this, this));
    cmb->addItem(QString("%1/%2 - ReadDeviceIdentification")
                    .arg(MBF_ENCAPSULATED_INTERFACE_TRANSPORT, 2, 10, QChar('0'))
                    .arg(MBF_MEI_READ_DEVICE_ID, 2, 10, QChar('0')));
    cmb->setCurrentIndex(2);

    setSendTo(SendToDevice);

    // Connect RadioButtons SendTo
    connect(ui->rdDevice, &QRadioButton::clicked, this, [this]() {
        this->setSendTo(SendToDevice);
    });
    connect(ui->rdPortUnit, &QRadioButton::clicked, this, [this]() {
        this->setSendTo(SendToPortUnit);
    });

    connect(ui->btnListShowHide, &QPushButton::clicked, this, &mbClientSendMessageUi::slotListShowHide);
    connect(ui->btnListInsert  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListInsert  );
    connect(ui->btnListEdit    , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListEdit    );
    connect(ui->btnListRemove  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListRemove  );
    connect(ui->btnListClear   , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListClear   );
    connect(ui->btnListMoveUp  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListMoveUp  );
    connect(ui->btnListMoveDown, &QPushButton::clicked, this, &mbClientSendMessageUi::slotListMoveDown);
    connect(ui->btnListImport  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListImport  );
    connect(ui->btnListExport  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotListExport  );

    ui->btnListShowHide->click();
    ui->btnSendOne->setDefault(true);

    connect(ui->btnSendOne  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotSendOne );
    connect(ui->btnSendList , &QPushButton::clicked, this, &mbClientSendMessageUi::slotSendList);
    connect(ui->btnStop     , &QPushButton::clicked, this, &mbClientSendMessageUi::slotStop    );
    connect(ui->btnClose    , &QPushButton::clicked, this, &QDialog::close);

    // Set keyboard shortcuts
    ui->btnSendOne ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Return));
    ui->btnSendList->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Return));
    ui->btnStop    ->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    connect(ui->lsList, &QAbstractItemView::doubleClicked, this, &mbClientSendMessageUi::getListItem);

    connect(core, &mbClient::projectChanged, this, &mbClientSendMessageUi::setProject);
    connect(core, &mbClient::statusChanged , this, &mbClientSendMessageUi::setRunStatus);
    setProject(core->project());
}

mbClientSendMessageUi::~mbClientSendMessageUi()
{
    delete ui;
}

MBSETTINGS mbClientSendMessageUi::cachedSettings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    m[s.sendTo   ] = QMetaEnum::fromType<SendTo>().valueToKey(m_sendTo);
    m[s.unit     ] = ui->spUnit->value();
    m[s.function ] = getCurrentFuncNum();
    m[s.list     ] = this                         ->getListItems();
    m[s.period   ] = ui->spPeriod                 ->value      ();
    m[s.wGeometry] = this->saveGeometry();

    for (int i = 0; i < ui->swFunctionData->count(); ++i)
    {
        mbClientSendMessageWidget *w = static_cast<mbClientSendMessageWidget*>(ui->swFunctionData->widget(i));
        auto t = w->cachedSettings();
        mb::unite(m, t);
    }
    return m;
}

void mbClientSendMessageUi::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);

    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(s.sendTo   ); if (it != end) setSendTo(mb::enumValue<SendTo>(it.value()));
    it = m.find(s.unit     ); if (it != end) ui->spUnit->setValue(it.value().toInt());
    it = m.find(s.function ); if (it != end) setCurrentFuncNum(static_cast<uint8_t> (it.value().toInt       ()));
    it = m.find(s.list     ); if (it != end) this                  ->setListItems   (it.value().toStringList());
    it = m.find(s.period   ); if (it != end) ui->spPeriod          ->setValue       (it.value().toInt       ());
    it = m.find(s.wGeometry); if (it != end) this                  ->restoreGeometry(it.value().toByteArray ());

    for (int i = 0; i < ui->swFunctionData->count(); ++i)
    {
        mbClientSendMessageWidget *w = static_cast<mbClientSendMessageWidget*>(ui->swFunctionData->widget(i));
        w->setCachedSettings(m);
    }
}

void mbClientSendMessageUi::setProject(mbCoreProject *p)
{
    mbClientProject *project = static_cast<mbClientProject*>(p);
    if (m_project != project)
    {
        if (m_project)
        {
            m_project->disconnect(this);
            ui->cmbPort->clear();
            ui->cmbDevice->clear();
        }
        m_project = project;
        if (m_project)
        {
            QList<mbClientPort*> ports = m_project->ports();
            connect(m_project, &mbClientProject::portAdded   , this, &mbClientSendMessageUi::addPort   );
            connect(m_project, &mbClientProject::portRemoving, this, &mbClientSendMessageUi::removePort);
            connect(m_project, &mbClientProject::portRenaming, this, &mbClientSendMessageUi::renamePort);
            Q_FOREACH (mbClientPort *d, ports)
                addPort(d);

            QList<mbClientDevice*> devices = m_project->devices();
            connect(m_project, &mbClientProject::deviceAdded   , this, &mbClientSendMessageUi::addDevice   );
            connect(m_project, &mbClientProject::deviceRemoving, this, &mbClientSendMessageUi::removeDevice);
            connect(m_project, &mbClientProject::deviceRenaming, this, &mbClientSendMessageUi::renameDevice);
            Q_FOREACH (mbClientDevice *d, devices)
                addDevice(d);
        }
    }
}

void mbClientSendMessageUi::addPort(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->insertItem(i, port->name());
}

void mbClientSendMessageUi::removePort(mbCorePort *port)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->removeItem(i);
}

void mbClientSendMessageUi::renamePort(mbCorePort *port, const QString newName)
{
    int i = m_project->portIndex(port);
    ui->cmbPort->setItemText(i, newName);
}

void mbClientSendMessageUi::addDevice(mbCoreDevice *device)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->insertItem(i, device->name());
}

void mbClientSendMessageUi::removeDevice(mbCoreDevice *device)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->removeItem(i);
}

void mbClientSendMessageUi::renameDevice(mbCoreDevice *device, const QString newName)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->setItemText(i, newName);
}

void mbClientSendMessageUi::setCurrentFuncIndex(int funcIndex)
{
    uint8_t funcNum = m_funcNums.value(funcIndex);
    setCurrentFuncNum(funcNum);
}

void mbClientSendMessageUi::setRunStatus(int status)
{
    if (status == mbClient::Stopped)
        slotStop();
}

void mbClientSendMessageUi::addFunctionWidget(mbClientSendMessageWidget *w)
{
    ui->swFunctionData->addWidget(w);
    m_funcWidget[w->function()] = w;
    m_funcNums.append(w->function());
}

mbClientSendMessageWidget *mbClientSendMessageUi::currentFunctionWidget() const
{
    return static_cast<mbClientSendMessageWidget*>(ui->swFunctionData->currentWidget());
}

void mbClientSendMessageUi::slotListShowHide()
{
    bool isVisible = ui->grList->isVisible();
    if (isVisible)
        ui->btnListShowHide->setText("Show");
    else
        ui->btnListShowHide->setText("Hide");
    ui->grList->setVisible(!isVisible);
}

void mbClientSendMessageUi::slotListInsert()
{
    mbClientMessageParams params;
    fillParams(params);
    int i = currentListIndex();
    m_list->insertMessage(i, params);
}

void mbClientSendMessageUi::slotListEdit()
{
    int i = currentListIndex();
    if (i < 0)
        return;
    mbClientMessageParams params;
    fillParams(params);
    m_list->editMessage(i, params);
}

void mbClientSendMessageUi::slotListRemove()
{
    int i = currentListIndex();
    m_list->removeMessage(i);
}

void mbClientSendMessageUi::slotListClear()
{
    m_list->clear();
}

void mbClientSendMessageUi::slotListMoveUp()
{
    int i = currentListIndex();
    if (m_list->moveUp(i))
        setCurrentListIndex(i-1);
}

void mbClientSendMessageUi::slotListMoveDown()
{
    int i = currentListIndex();
    if (m_list->moveDown(i))
        setCurrentListIndex(i+1);
}

void mbClientSendMessageUi::slotListImport()
{
    mbClientDialogs *dialogs = mbClient::global()->ui()->dialogs();
    QString file = dialogs->getOpenFileName(this,
                                            QString("Import Message"),
                                            QString(),
                                            dialogs->getFilterString(mbCoreDialogs::Filter_AllFiles));
    if (!file.isEmpty())
    {
        QFile qf(file);
        if (qf.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QTextStream in(&qf);
            QList<mbClientMessageParams> messages;
            bool ok = false;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                mbClientMessageParams m = m_converter.restoreClientMessageParams(line, &ok);
                if (ok)
                    messages.append(m);
            }
            qf.close();
            m_list->setMessages(messages);
        }
    }
}

void mbClientSendMessageUi::slotListExport()
{
    mbClientDialogs *dialogs = mbClient::global()->ui()->dialogs();
    QString file = dialogs->getSaveFileName(this,
                                            QString("Export Message"),
                                            QString(),
                                            dialogs->getFilterString(mbCoreDialogs::Filter_AllFiles));
    if (!file.isEmpty())
    {
        QFile qf(file);
        if (qf.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&qf);
            QList<mbClientMessageParams> messages = m_list->messages();
            Q_FOREACH (const mbClientMessageParams &m, messages)
            {
                QString line = m_converter.saveClientMessageParams(m);
                out << line << '\n';
            }
            qf.close();
        }
    }
}

void mbClientSendMessageUi::slotSendOne()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    if (!prepareSendParams())
        return;
    createMessage();
    if (m_messageList.count())
    {
        startSendMessages();
    }
}

void mbClientSendMessageUi::slotSendList()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    if (!prepareSendParams())
        return;
    createMessageList();
    if (m_messageList.count())
    {
        startSendMessages();
    }
}

void mbClientSendMessageUi::slotStop()
{
    stopSendMessages();
}

void mbClientSendMessageUi::slotBytesTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientSendMessageUi::slotBytesRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientSendMessageUi::slotAsciiTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientSendMessageUi::slotAsciiRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientSendMessageUi::messageCompleted()
{
    mbClientRunMessagePtr message = qobject_cast<mbClientRunMessage*>(sender());
    if (message)
    {
        ui->lnStatus   ->setText(mb::toString(message->status   ()));
        ui->lnTimestamp->setText(mb::toString(message->timestamp()));
        fillForm(message);
        if (isTimerStopped())
            stopSendMessages();
    }
}

void mbClientSendMessageUi::getListItem(const QModelIndex &index)
{
    int i = index.row();
    if (i < 0)
        return;
    // TODO: check if txt already has data and ask user to confirm
    mbClientMessageParams p = m_list->message(i);
    fillForm(p);
}

void mbClientSendMessageUi::timerEvent(QTimerEvent */*event*/)
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

QStringList mbClientSendMessageUi::getListItems() const
{
    return m_converter.saveClientMessages(m_list->messages());
}

void mbClientSendMessageUi::setListItems(const QStringList &list)
{
    m_list->setMessages(m_converter.restoreClientMessages(list));
}

int mbClientSendMessageUi::currentListIndex() const
{
    auto indexes = ui->lsList->selectionModel()->selectedIndexes();
    if (indexes.count())
        return indexes.first().row();
    return -1;
}

void mbClientSendMessageUi::setCurrentListIndex(int i)
{
    QModelIndex index = m_list->index(i, 0);
    ui->lsList->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
}

void mbClientSendMessageUi::createMessage()
{
    m_messageIndex = 0;
    m_messageList.clear();
    mbClientMessageParams params;
    fillParams(params);
    mbClientRunMessage *msg = this->createMessage(params);
    m_messageList.append(msg);
}

void mbClientSendMessageUi::createMessageList()
{
    m_messageIndex = 0;
    m_messageList.clear();
    auto msgList = m_list->messages();
    if (msgList.count() == 0)
        return;
    Q_FOREACH (const auto& params, msgList)
    {
        mbClientRunMessage *msg = this->createMessage(params);
        m_messageList.append(msg);
    }
}

mbClientRunMessage *mbClientSendMessageUi::createMessage(const mbClientMessageParams &params)
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return nullptr;
    mbClientRunMessage *msg;
    switch(params.function())
    {
    case MBF_READ_COILS:
        return new mbClientRunMessageReadCoils(params.offset(),
                                               params.count(),
                                               m_dataParams.maxReadCoils,
                                               this);
    case MBF_READ_DISCRETE_INPUTS:
        return new mbClientRunMessageReadDiscreteInputs(params.offset(),
                                                        params.count(),
                                                        m_dataParams.maxReadDiscreteInputs,
                                                        this);
    case MBF_READ_HOLDING_REGISTERS:
        return new mbClientRunMessageReadHoldingRegisters(params.offset(),
                                                          params.count(),
                                                          m_dataParams.maxReadHoldingRegisters,
                                                          this);
    case MBF_READ_INPUT_REGISTERS:
        return new mbClientRunMessageReadInputRegisters(params.offset(),
                                                        params.count(),
                                                        m_dataParams.maxReadInputRegisters,
                                                        this);
    case MBF_WRITE_SINGLE_COIL:
        msg = new mbClientRunMessageWriteSingleCoil(params.offset(), this);
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        msg = new mbClientRunMessageWriteSingleRegister(params.offset(), this);
        break;
    case MBF_READ_EXCEPTION_STATUS:
        return new mbClientRunMessageReadExceptionStatus(this);
    case MBF_DIAGNOSTICS:
        msg = new mbClientRunMessageDiagnostics(params.subfunction(), params.count(), this);
        break;
    case MBF_GET_COMM_EVENT_COUNTER:
        return new mbClientRunMessageGetCommEventCounter(this);
    case MBF_GET_COMM_EVENT_LOG:
        return new mbClientRunMessageGetCommEventLog(this);
    case MBF_WRITE_MULTIPLE_COILS:
    {
        msg = new mbClientRunMessageWriteMultipleCoils(params.offset(),
                                                       params.count(),
                                                       m_dataParams.maxWriteMultipleCoils,
                                                       this);
        break;
    }
    case MBF_WRITE_MULTIPLE_REGISTERS:
        msg = new mbClientRunMessageWriteMultipleRegisters(params.offset(),
                                                           params.count(),
                                                           m_dataParams.maxWriteMultipleRegisters,
                                                           this);
        break;
    case MBF_REPORT_SERVER_ID:
        return new mbClientRunMessageReportServerID(this);
    case MBF_READ_FILE_RECORD:
        return new mbClientRunMessageReadFileRecord(params.fileRecords().count(), params.fileRecords().data(), this);
    case MBF_WRITE_FILE_RECORD:
        msg = new mbClientRunMessageWriteFileRecord(params.fileRecords().count(), params.fileRecords().data(), this);
        break;
    case MBF_MASK_WRITE_REGISTER:
        msg = new mbClientRunMessageMaskWriteRegister(params.offset(), this);
        msg->innerBufferReg()[0] = params.maskAnd();
        msg->innerBufferReg()[1] = params.maskOr ();
        return msg;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        msg = new mbClientRunMessageReadWriteMultipleRegisters(params.offset(),
                                                               params.count(),
                                                               params.writeOffset(),
                                                               params.writeCount(),
                                                               this);
        break;
    case MBF_READ_FIFO_QUEUE:
        return new mbClientRunMessageReadFIFOQueue(params.offset(), this);
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
        return new mbClientRunMessageReadDeviceId(params.deviceId(), params.objectId(), this);
    default:
        return nullptr;
    }

    auto data = m_converter.toByteArray(params);
    if (data.count())
    {
        msg->setData(0, params.count(), data.data());
    }
    return msg;
}

void mbClientSendMessageUi::sendMessage()
{
    if (m_messageList.count() == 0)
        return;
    mbClientRunMessagePtr msg = m_messageList.value(m_messageIndex);
    if (msg)
    {
        //ui->txtModbusTx->clear();
        ui->txtModbusRx->clear();
        if (m_sendTo == SendToPortUnit)
        {
            mbClientPort *port = currentPort();
            if (!port)
                return;
            mbClient::global()->sendPortMessage(port->handle(), msg);
        }
        else
        {
            mbClientDevice *device = currentDevice();
            if (!device)
                return;
            mbClient::global()->sendMessage(device->handle(), msg);
        }
    }
}

void mbClientSendMessageUi::prepareToSend(mbClientRunMessage *msg)
{
    if (m_sendTo == SendToPortUnit)
        msg->setUnit(m_unit);
    connect(msg, &mbClientRunMessage::signalBytesTx, this, &mbClientSendMessageUi::slotBytesTx     );
    connect(msg, &mbClientRunMessage::signalBytesRx, this, &mbClientSendMessageUi::slotBytesRx     );
    connect(msg, &mbClientRunMessage::signalAsciiTx, this, &mbClientSendMessageUi::slotBytesTx     );
    connect(msg, &mbClientRunMessage::signalAsciiRx, this, &mbClientSendMessageUi::slotBytesRx     );
    connect(msg, &mbClientRunMessage::completed    , this, &mbClientSendMessageUi::messageCompleted);
    auto w = currentFunctionWidget();
    if (w->function() == msg->function())
        w->prepareToSend();
}

void mbClientSendMessageUi::clearAfterSend(mbClientRunMessage *msg)
{
    msg->disconnect(this);
}

void mbClientSendMessageUi::startSendMessages()
{
    setEnableParams(false);
    ui->txtModbusTx->clear();
    ui->txtModbusRx->clear();
    Q_FOREACH (auto msg, m_messageList)
    {
        prepareToSend(msg.data());
    }
    sendMessage();
    if ((m_messageList.count() > 1 || ui->chbUseLoop->isChecked()) && isTimerStopped())
    {
        m_timer = startTimer(ui->spPeriod->value());;
    }
}

void mbClientSendMessageUi::stopSendMessages()
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

mbClientPort *mbClientSendMessageUi::currentPort() const
{
    if (m_project)
    {
        int i = ui->cmbPort->currentIndex();
        return m_project->port(i);
    }
    return nullptr;
}

mbClientDevice *mbClientSendMessageUi::currentDevice() const
{
    if (m_project)
    {
        int i = ui->cmbDevice->currentIndex();
        return m_project->device(i);
    }
    return nullptr;
}

void mbClientSendMessageUi::setEnableParams(bool enable)
{
    ui->grCommonParams ->setEnabled(enable);
    ui->grFunctionData ->setEnabled(enable);
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

void mbClientSendMessageUi::setSendTo(int type)
{
    if (m_sendTo != type)
    {
        switch (type)
        {
        case SendToPortUnit:
            m_sendTo = SendToPortUnit;
            ui->lbSendTo->setText(QStringLiteral("Port:"));
            ui->swSendTo->setCurrentWidget(ui->pgPortUnit);
            ui->rdPortUnit->setChecked(true);
            break;
        case SendToDevice:
        default:
            m_sendTo = SendToDevice;
            ui->lbSendTo->setText(QStringLiteral("Device:"));
            ui->swSendTo->setCurrentWidget(ui->pgDevice);
            ui->rdDevice->setChecked(true);
            break;
        }
    }
}

bool mbClientSendMessageUi::prepareSendParams()
{
    switch (m_sendTo)
    {
    case SendToPortUnit:
        m_port = currentPort();
        if (m_port)
        {
            const mbClientDevice::Defaults &def = mbClientDevice::Defaults::instance();
            m_dataParams.maxReadCoils               = def.maxReadCoils             ;
            m_dataParams.maxReadDiscreteInputs      = def.maxReadDiscreteInputs    ;
            m_dataParams.maxReadHoldingRegisters    = def.maxReadHoldingRegisters  ;
            m_dataParams.maxReadInputRegisters      = def.maxReadInputRegisters    ;
            m_dataParams.maxWriteMultipleCoils      = def.maxWriteMultipleCoils    ;
            m_dataParams.maxWriteMultipleRegisters  = def.maxWriteMultipleRegisters;
            m_converter.setSwapBytes           (def.swapBytes         );
            m_converter.setRegisterOrder       (def.registerOrder     );
            m_converter.setByteArrayFormat     (def.byteArrayFormat   );
            m_converter.setStringEncoding      (def.stringEncoding    );
            m_converter.setStringLengthType    (def.stringLengthType  );
            m_converter.setByteArraySeparator  (def.byteArraySeparator);
            m_unit = static_cast<decltype(m_unit)>(ui->spUnit->value());
        }
        else
            return false;
        break;
    default:
        m_device = currentDevice();
        if (m_device)
        {
            m_dataParams.maxReadCoils               = m_device->maxReadCoils             ();
            m_dataParams.maxReadDiscreteInputs      = m_device->maxReadDiscreteInputs    ();
            m_dataParams.maxReadHoldingRegisters    = m_device->maxReadHoldingRegisters  ();
            m_dataParams.maxReadInputRegisters      = m_device->maxReadInputRegisters    ();
            m_dataParams.maxWriteMultipleCoils      = m_device->maxWriteMultipleCoils    ();
            m_dataParams.maxWriteMultipleRegisters  = m_device->maxWriteMultipleRegisters();
            m_converter.setSwapBytes           (m_device->swapBytes                ());
            m_converter.setRegisterOrder       (m_device->registerOrder            ());
            m_converter.setByteArrayFormat     (m_device->byteArrayFormat          ());
            m_converter.setStringEncoding      (m_device->stringEncoding           ());
            m_converter.setStringLengthType    (m_device->stringLengthType         ());
            m_converter.setByteArraySeparator  (m_device->byteArraySeparator       ());
        }
        else
            return false;
        break;
    }
    return true;
}

void mbClientSendMessageUi::fillParams(mbClientMessageParams &params)
{
    auto w = currentFunctionWidget();
    params.setFunction(w->function());
    w->fillParams(params);
}

void mbClientSendMessageUi::fillForm(mbClientMessageParams &params)
{
    setCurrentFuncNum(params.function());
    auto w = currentFunctionWidget();
    w->setParams(params);
}

void mbClientSendMessageUi::fillForm(const mbClientRunMessagePtr &message)
{
    if (!Modbus::StatusIsGood(message->status()))
        return;
    mbClientMessageParams params;
    params.setFunction(message->function());
    switch (message->function())
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    {
        uint16_t count = message->count();
        QByteArray data((count+7)/8, '\0');
        message->getData(0, count, data.data());
        params.setData(data);
    }
        break;
    case MBF_READ_EXCEPTION_STATUS:
    {
        uint16_t count = 1;
        QByteArray data(count, '\0');
        message->getData(0, count, data.data());
        params.setData(data);
    }
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    {
        uint16_t count = message->count();
        QByteArray data(count * 2, '\0');
        message->getData(0, count, data.data());
        params.setData(data);
    }
        break;
    case MBF_DIAGNOSTICS:
    {
        uint16_t count = message->count();
        QByteArray data(reinterpret_cast<char*>(message->innerBuffer()), count);
        params.setData(data);
    }
        break;
    case MBF_GET_COMM_EVENT_COUNTER:
    {
        auto m = reinterpret_cast<mbClientRunMessageGetCommEventCounter*>(message.data());
        params.setStatus(m->status());
        params.setEventCount(m->eventCount());
    }
        break;
    case MBF_GET_COMM_EVENT_LOG:
    {
        auto m = reinterpret_cast<mbClientRunMessageGetCommEventLog*>(message.data());
        params.setStatus(m->status());
        params.setEventCount(m->eventCount());
        params.setMessageCount(m->messageCount());
        QByteArray data(reinterpret_cast<char*>(message->innerBuffer()), m->count());
        params.setData(data);
    }
        break;
    case MBF_REPORT_SERVER_ID:
    {
        QByteArray data(reinterpret_cast<char*>(message->innerBuffer()), message->count());
        params.setData(data);
    }
        break;
    case MBF_READ_FILE_RECORD:
    {
        auto *m = static_cast<mbClientRunMessageFileRecord*>(message.data());
        auto dataSize = m->dataSize();
        QByteArray b(reinterpret_cast<char*>(m->fileData()), dataSize);
        params.setData(b);
        params.setFileRecords(m->fileRecords(), m->count());
    }
        break;
    case MBF_READ_FIFO_QUEUE:
    {
        uint16_t count = message->count();
        QByteArray data(count * 2, '\0');
        message->getData(0, count, data.data());
        params.setCount(count);
        params.setData(data);
    }
        break;
    case MBF_ENCAPSULATED_INTERFACE_TRANSPORT:
    {
        auto m = reinterpret_cast<mbClientRunMessageReadDeviceId*>(message.data());
        params.setConformityLevel(m->conformityLevel());
        params.setNextObjectId(m->nextObjectId());
        params.setMoreFollows(m->moreFollows());
        QByteArray data(reinterpret_cast<char*>(m->innerBuffer()), m->dataSize());
        params.setData(data);   
    }
        break;
    default:
        return;
    }
    fillForm(params);
}

uint8_t mbClientSendMessageUi::getCurrentFuncNum() const
{
    return m_funcNums.value(ui->cmbFunction->currentIndex());
}

void mbClientSendMessageUi::setCurrentFuncNum(uint8_t func)
{
    auto w = m_funcWidget.value(func, nullptr);
    if (w)
        ui->swFunctionData->setCurrentWidget(w);
    int i = 0;
    Q_FOREACH (int f, m_funcNums)
    {
        if (f == func)
        {
            ui->cmbFunction->setCurrentIndex(i);
            break;
        }
        ++i;
    }
}
