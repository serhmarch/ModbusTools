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

#include "client_sendmessagelistmodel.h"

mbClientSendMessageUi::Strings::Strings() :
    prefix         (QStringLiteral("Ui.SendMessage.")),
    sendTo         (prefix+QStringLiteral("sendTo")),
    unit           (prefix+QStringLiteral("unit")),
    function       (prefix+QStringLiteral("function")),
    readAdrType    (prefix+QStringLiteral("readAdrType")),
    readAddress    (prefix+QStringLiteral("readAddress")),
    readFormat     (prefix+QStringLiteral("readFormat")),
    readCount      (prefix+QStringLiteral("readCount")),
    writeAdrType   (prefix+QStringLiteral("writeAdrType")),
    writeAddress   (prefix+QStringLiteral("writeAddress")),
    writeFormat    (prefix+QStringLiteral("writeFormat")),
    writeCount     (prefix+QStringLiteral("writeCount")),
    writeData      (prefix+QStringLiteral("writeData")),
    list           (prefix+QStringLiteral("list")),
    period         (prefix+QStringLiteral("period")),
    writeMaskAnd   (prefix+QStringLiteral("writeMaskAnd")),
    writeMaskOr    (prefix+QStringLiteral("writeMaskOr"))
{
}

const mbClientSendMessageUi::Strings &mbClientSendMessageUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientSendMessageUi::mbClientSendMessageUi(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().prefix, parent),
    ui(new Ui::mbClientSendMessageUi)
{
    ui->setupUi(this);
    m_list = new mbClientSendMessageListModel(this);
    ui->lsList->setModel(m_list);
    m_project = nullptr;
    m_sendTo = -1;
    m_timer = 0;

    QStringList ls;
    QComboBox *cmb;
    QSpinBox *sp;

    mbClient *core = mbClient::global();

    m_funcNums.append(MBF_READ_COILS                    );
    m_funcNums.append(MBF_READ_DISCRETE_INPUTS          );
    m_funcNums.append(MBF_READ_HOLDING_REGISTERS        );
    m_funcNums.append(MBF_READ_INPUT_REGISTERS          );
    m_funcNums.append(MBF_WRITE_SINGLE_COIL             );
    m_funcNums.append(MBF_WRITE_SINGLE_REGISTER         );
    m_funcNums.append(MBF_READ_EXCEPTION_STATUS         );
    m_funcNums.append(MBF_WRITE_MULTIPLE_COILS          );
    m_funcNums.append(MBF_WRITE_MULTIPLE_REGISTERS      );
    m_funcNums.append(MBF_REPORT_SERVER_ID              );
    m_funcNums.append(MBF_MASK_WRITE_REGISTER           );
    m_funcNums.append(MBF_READ_WRITE_MULTIPLE_REGISTERS );

    // -----------------------------------------------------------------------
    // Unit
    sp = ui->spUnit;
    sp->setMinimum(0);
    sp->setMaximum(255);

    // Read Data
    // Address type
    // Note: need to initialize earlier because it's used in setCurrentFuncIndex
    m_readAddress = new mbCoreAddressWidget();
    m_readAddress->setEnabledAddressType(false);
    ui->formLayoutReadData->setWidget(1, QFormLayout::FieldRole, m_readAddress);

    // ReadFormat
    cmb = ui->cmbReadFormat;
    ls = mb::enumFormatKeyList();
    Q_FOREACH (const QString &s, ls)
        cmb->addItem(s);
    cmb->setCurrentIndex(mb::Dec16);

    sp = ui->spReadCount;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS); // TODO: if register was choosen than change this value

    // -----------------------------------------------------------------------
    // Write Data
    // Address type
    // Note: need to initialize earlier because it's used in setCurrentFuncIndex
    m_writeAddress = new mbCoreAddressWidget();
    m_writeAddress->setEnabledAddressType(false);
    ui->formLayoutWriteData->setWidget(1, QFormLayout::FieldRole, m_writeAddress);

    // WriteFormat
    cmb = ui->cmbWriteFormat;
    ls = mb::enumFormatKeyList();
    Q_FOREACH (const QString &s, ls)
        cmb->addItem(s);
    cmb->setCurrentIndex(mb::Dec16);

    sp = ui->spWriteCount;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS); // TODO: if register was choosen than change this value

    sp = ui->spPeriod;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);
    sp->setValue(1000);


    sp = ui->spWriteMaskAnd;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setDisplayIntegerBase(16);
    sp->setValue(0);

    sp = ui->spWriteMaskOr;
    sp->setMinimum(0);
    sp->setMaximum(USHRT_MAX);
    sp->setDisplayIntegerBase(16);
    sp->setValue(0);

    cmb = ui->cmbFunction;
    connect(cmb, SIGNAL(currentIndexChanged(int)), this, SLOT(setCurrentFuncIndex(int)));
    Q_FOREACH (uint8_t funcNum, m_funcNums)
    {
        cmb->addItem(QString("%1 - %2")
                         .arg(funcNum, 2, 10, QChar('0'))
                         .arg(mb::ModbusFunctionString(funcNum))
                     );
    }
    cmb->setCurrentIndex(2);

    connect(mbCore::globalCore(), &mbCore::addressNotationChanged, this, &mbClientSendMessageUi::setModbusAddresNotation);

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

    connect(ui->btnSendOne  , &QPushButton::clicked, this, &mbClientSendMessageUi::slotSendOne );
    connect(ui->btnSendList , &QPushButton::clicked, this, &mbClientSendMessageUi::slotSendList);
    connect(ui->btnStop     , &QPushButton::clicked, this, &mbClientSendMessageUi::slotStop    );
    connect(ui->btnClose    , &QPushButton::clicked, this, &QDialog::close);

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
    m[s.sendTo      ] = QMetaEnum::fromType<SendTo>().valueToKey(m_sendTo);
    m[s.unit        ] = ui->spUnit->value();
    m[s.function    ] = getCurrentFuncNum();
  //m[s.readAdrType ] = ui->cmbReadAdrType ->currentText();
    m[s.readAddress ] = getReadAddress      ();
    m[s.readFormat  ] = ui->cmbReadFormat  ->currentText();
    m[s.readCount   ] = ui->spReadCount    ->value      ();
  //m[s.writeAdrType] = ui->cmbWriteAdrType->currentText();
    m[s.writeAddress] = getWriteAddress      ();
    m[s.writeFormat ] = ui->cmbWriteFormat ->currentText();
    m[s.writeCount  ] = ui->spWriteCount   ->value      ();
    m[s.writeData   ] = ui->txtWriteData   ->toPlainText();
    m[s.list        ] = this               ->getListItems();
    m[s.period      ] = ui->spPeriod       ->value      ();
    m[s.writeMaskAnd] = ui->spWriteMaskAnd ->value      ();
    m[s.writeMaskOr ] = ui->spWriteMaskOr  ->value      ();
    m[s.wGeometry   ] = this->saveGeometry();

    return m;
}

void mbClientSendMessageUi::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogBase::setCachedSettings(m);

    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();
    //bool ok;

    it = m.find(s.sendTo      ); if (it != end) setSendTo(mb::enumValue<SendTo>(it.value()));
    it = m.find(s.unit        ); if (it != end) ui->spUnit->setValue(it.value().toInt());
    it = m.find(s.function    ); if (it != end) setCurrentFuncNum(static_cast<uint8_t>(it.value().toInt()));
  //it = m.find(s.readAdrType ); if (it != end) ui->cmbReadAdrType ->setCurrentText (it.value().toString());
    it = m.find(s.readAddress ); if (it != end) setReadAddress                      (it.value().toInt()   );
    it = m.find(s.readFormat  ); if (it != end) ui->cmbReadFormat  ->setCurrentText (it.value().toString());
    it = m.find(s.readCount   ); if (it != end) ui->spReadCount    ->setValue       (it.value().toInt()   );
  //it = m.find(s.writeAdrType); if (it != end) ui->cmbWriteAdrType->setCurrentText (it.value().toString());
    it = m.find(s.writeAddress); if (it != end) setWriteAddress                     (it.value().toInt()   );
    it = m.find(s.writeFormat ); if (it != end) ui->cmbWriteFormat ->setCurrentText (it.value().toString());
    it = m.find(s.writeCount  ); if (it != end) ui->spWriteCount   ->setValue       (it.value().toInt()   );
    it = m.find(s.writeData   ); if (it != end) ui->txtWriteData   ->setPlainText   (it.value().toString());
    it = m.find(s.list        ); if (it != end) this               ->setListItems   (it.value().toStringList()     );
    it = m.find(s.period      ); if (it != end) ui->spPeriod       ->setValue       (it.value().toInt()   );
    it = m.find(s.writeMaskAnd); if (it != end) ui->spWriteMaskAnd ->setValue       (it.value().toInt()   );
    it = m.find(s.writeMaskOr ); if (it != end) ui->spWriteMaskOr  ->setValue       (it.value().toInt()   );
    it = m.find(s.wGeometry   ); if (it != end) this               ->restoreGeometry(it.value().toByteArray());
}

void mbClientSendMessageUi::setModbusAddresNotation(mb::AddressNotation notation)
{
    m_readAddress ->setAddressNotation(notation);
    m_writeAddress->setAddressNotation(notation);
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
    mbClientSendMessageParams params;
    fillParams(params);
    int i = currentListIndex();
    m_list->insertMessage(i, params);
}

void mbClientSendMessageUi::slotListEdit()
{
    int i = currentListIndex();
    if (i < 0)
        return;
    mbClientSendMessageParams params;
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
            QList<const mbClientSendMessageParams*> messages;
            while (!in.atEnd())
            {
                QString line = in.readLine();
                mbClientSendMessageParams *m = restoreParams(line);
                if (m)
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
            QList<const mbClientSendMessageParams*> messages = m_list->messages();
            Q_FOREACH (const mbClientSendMessageParams *m, messages)
            {
                QString line = saveParams(*m);
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
    const mbClientSendMessageParams *p = m_list->message(i);
    fillForm(*p);
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
    return saveMessages(m_list->messages());
}

void mbClientSendMessageUi::setListItems(const QStringList &list)
{
    m_list->setMessages(restoreMessages(list));
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
    mbClientSendMessageParams params;
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
    Q_FOREACH (const auto* params, msgList)
    {
        mbClientRunMessage *msg = this->createMessage(*params);
        m_messageList.append(msg);
    }
}

mbClientRunMessage *mbClientSendMessageUi::createMessage(const mbClientSendMessageParams &params)
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return nullptr;
    QByteArray data;
    uint16_t c = 0;
    mbClientRunMessage *msg;
    switch(params.func)
    {
    case MBF_READ_COILS:
        return new mbClientRunMessageReadCoils(params.offset,
                                               params.count,
                                               m_dataParams.maxReadCoils,
                                               this);
    case MBF_READ_DISCRETE_INPUTS:
        return new mbClientRunMessageReadDiscreteInputs(params.offset,
                                                        params.count,
                                                        m_dataParams.maxReadDiscreteInputs,
                                                        this);
    case MBF_READ_HOLDING_REGISTERS:
        return new mbClientRunMessageReadHoldingRegisters(params.offset,
                                                          params.count,
                                                          m_dataParams.maxReadHoldingRegisters,
                                                          this);
    case MBF_READ_INPUT_REGISTERS:
        return new mbClientRunMessageReadInputRegisters(params.offset,
                                                        params.count,
                                                        m_dataParams.maxReadInputRegisters,
                                                        this);
    case MBF_WRITE_SINGLE_COIL:
    {
        msg = new mbClientRunMessageWriteSingleCoil(params.offset,
                                                    this);
        char v = (params.data == QStringLiteral("1"));
        data = QByteArray(v, 1);
        c = 1;
    }
        break;
    case MBF_WRITE_SINGLE_REGISTER:
    {
        msg = new mbClientRunMessageWriteSingleRegister(params.offset,
                                                        this);
        switch (params.format)
        {
        case mb::Bool:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListBits(ls);
        }
            break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(params.data,
                                   params.format,
                                   Modbus::Memory_4x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   msg->count()*2);
            break;
        default:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListNumbers(ls, params.format);
        }
            break;
        }
    }
        break;
    case MBF_READ_EXCEPTION_STATUS:
        return new mbClientRunMessageReadExceptionStatus(this);
    case MBF_WRITE_MULTIPLE_COILS:
    {
        msg = new mbClientRunMessageWriteMultipleCoils(params.offset,
                                                       params.count,
                                                       m_dataParams.maxWriteMultipleCoils,
                                                       this);
        switch (params.format)
        {
        case mb::Bool:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListBits(ls);
            c = ls.count();
        }
        break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(params.data,
                                   params.format,
                                   Modbus::Memory_0x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   (msg->count()+7)/8);
            c = data.count() * 8;
            break;
        default:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListNumbers(ls, params.format);
            c = data.count() * 8;
        }
        break;
        }
    }
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
    {
        msg = new mbClientRunMessageWriteMultipleRegisters(params.offset,
                                                           params.count,
                                                           m_dataParams.maxWriteMultipleRegisters,
                                                           this);
        switch (params.format)
        {
        case mb::Bool:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListBits(ls);
        }
        break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(params.data,
                                   params.format,
                                   Modbus::Memory_4x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   msg->count()*2);
            break;
        default:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListNumbers(ls, params.format);
        }
            break;
        }
        if (data.count() && (data.count() & 1))
            data.append('\0');
        c = data.count() / 2;
    }
        break;
    case MBF_REPORT_SERVER_ID:
       return new mbClientRunMessageReportServerID(this);
    case MBF_MASK_WRITE_REGISTER:
    {
        msg = new mbClientRunMessageMaskWriteRegister(params.offset, this);
       struct { quint16 andMask; quint16 orMask; } v;
       v.andMask = static_cast<quint16>(ui->spWriteMaskAnd->value());
       v.orMask  = static_cast<quint16>(ui->spWriteMaskOr ->value());
       data = QByteArray(reinterpret_cast<char*>(&v), sizeof(v));
       c = 2;
    }
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
    {
        msg = new mbClientRunMessageReadWriteMultipleRegisters(params.offset,
                                                               params.count,
                                                               params.writeOffset,
                                                               params.writeCount,
                                                               this);
        switch (params.format)
        {
        case mb::Bool:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListBits(ls);
        }
        break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(params.data,
                                   params.format,
                                   Modbus::Memory_4x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   msg->count()*2);
            break;
        default:
        {
            QStringList ls = dataToStringList(params.data);
            data = fromStringListNumbers(ls, params.format);
        }
            break;
        }
        if (data.count() && (data.count() & 1))
            data.append('\0');
        c = data.count() / 2;
    }
        break;
    default:
        return nullptr;
    }

    if (data.count())
    {
        msg->setData(0, c, data.data());
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
        prepareToSend(reinterpret_cast<mbClientRunMessageRaw*>(msg.data()));
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

mbClientSendMessageParams *mbClientSendMessageUi::restoreParams(const QString &params)
{
    mbClientSendMessageParams *res = new mbClientSendMessageParams;
    QStringList pairs = params.split(';', Qt::SkipEmptyParts);

    Q_FOREACH (const QString &pair, pairs)
    {
        int eqPos = pair.indexOf('=');
        if (eqPos > 0)
        {
            QString name = pair.left(eqPos).trimmed();
            QString value = pair.mid(eqPos + 1).trimmed();
            if (name == "func")
                res->func = static_cast<decltype(res->func)>(value.toInt());
            else if (name == "offset" || name == "readoffset")
                res->offset = static_cast<decltype(res->offset)>(value.toInt());
            else if (name == "count" || name == "readcount")
                res->count = static_cast<decltype(res->count)>(value.toInt());
            else if (name == "writeoffset")
                res->writeOffset = static_cast<decltype(res->writeOffset)>(value.toInt());
            else if (name == "writecount")
                res->writeCount = static_cast<decltype(res->writeCount)>(value.toInt());
            else if (name == "format")
                res->format = mb::enumFormatValue(value);
            else if (name == "data")
                res->data = value;
        }
    }
    return res;
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
    ui->grReadData     ->setEnabled(enable);
    ui->grWriteData    ->setEnabled(enable);
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

int mbClientSendMessageUi::getReadAddress() const
{
    return m_readAddress->getAddress().number();
}

void mbClientSendMessageUi::setReadAddress(int v)
{
    mb::Address adr = m_readAddress->getAddress();
    adr.setNumber(v);
    m_readAddress->setAddress(adr);
}

uint16_t mbClientSendMessageUi::getReadOffset() const
{
    return m_readAddress->getAddress().offset();
}

void mbClientSendMessageUi::setReadOffset(uint16_t v)
{
    mb::Address adr = m_readAddress->getAddress();
    adr.setOffset(v);
    m_readAddress->setAddress(adr);
}

int mbClientSendMessageUi::getWriteAddress() const
{
    return m_writeAddress->getAddress().number();
}

void mbClientSendMessageUi::setWriteAddress(int v)
{
    mb::Address adr = m_writeAddress->getAddress();
    adr.setNumber(v);
    m_writeAddress->setAddress(adr);
}

uint16_t mbClientSendMessageUi::getWriteOffset() const
{
    return m_writeAddress->getAddress().offset();
}

void mbClientSendMessageUi::setWriteOffset(uint16_t v)
{
    mb::Address adr = m_writeAddress->getAddress();
    adr.setOffset(v);
    m_writeAddress->setAddress(adr);
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
            m_dataParams.byteOrder                  = def.byteOrder                ;
            m_dataParams.registerOrder              = def.registerOrder            ;
            m_dataParams.byteArrayFormat            = def.byteArrayFormat          ;
            m_dataParams.stringEncoding             = def.stringEncoding           ;
            m_dataParams.stringLengthType           = def.stringLengthType         ;
            m_dataParams.byteArraySeparator         = def.byteArraySeparator       ;
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
            m_dataParams.byteOrder                  = m_device->byteOrder                ();
            m_dataParams.registerOrder              = m_device->registerOrder            ();
            m_dataParams.byteArrayFormat            = m_device->byteArrayFormat          ();
            m_dataParams.stringEncoding             = m_device->stringEncoding           ();
            m_dataParams.stringLengthType           = m_device->stringLengthType         ();
            m_dataParams.byteArraySeparator         = m_device->byteArraySeparator       ();
        }
        else
            return false;
        break;
    }
    return true;
}

void mbClientSendMessageUi::fillParams(mbClientSendMessageParams &params)
{
    params.func = getCurrentFuncNum();
    switch(params.func)
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
        params.offset = getReadOffset();
        params.count  = static_cast<uint16_t>(ui->spReadCount->value());
        break;
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
        params.offset = getWriteOffset();
        params.format = mb::enumFormatValueByIndex(ui->cmbWriteFormat->currentIndex());
        params.data = ui->txtWriteData->toPlainText();
        break;
    case MBF_READ_EXCEPTION_STATUS:
        break;
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
        params.offset = getWriteOffset();
        params.count  = static_cast<uint16_t>(ui->spWriteCount->value());
        params.format = mb::enumFormatValueByIndex(ui->cmbWriteFormat->currentIndex());
        params.data = ui->txtWriteData->toPlainText();
        break;
    case MBF_REPORT_SERVER_ID:
        break;
    case MBF_MASK_WRITE_REGISTER:
        params.offset = getWriteOffset();
        params.format = mb::enumFormatValueByIndex(ui->cmbWriteFormat->currentIndex());
        params.data = ui->txtWriteData->toPlainText();
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        params.offset      = getReadOffset();
        params.count       = static_cast<uint16_t>(ui->spReadCount   ->value());
        params.writeOffset = getWriteOffset();
        params.writeCount  = static_cast<uint16_t>(ui->spWriteCount  ->value());
        params.format = mb::enumFormatValueByIndex(ui->cmbWriteFormat->currentIndex());
        params.data = ui->txtWriteData->toPlainText();
        break;
    default:
        return;
    }
}

void mbClientSendMessageUi::fillForm(const mbClientSendMessageParams &params)
{
    setCurrentFuncNum(params.func);
    switch(params.func)
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
        setReadOffset(params.offset);
        ui->spReadCount->setValue(params.count);
        break;
    case MBF_WRITE_SINGLE_COIL:
    case MBF_WRITE_SINGLE_REGISTER:
        setReadOffset(params.offset);
        ui->cmbWriteFormat->setCurrentText(mb::enumFormatKey(params.format));
        ui->txtWriteData->setPlainText(params.data);
        break;
    case MBF_READ_EXCEPTION_STATUS:
        break;
    case MBF_WRITE_MULTIPLE_COILS:
    case MBF_WRITE_MULTIPLE_REGISTERS:
        setWriteOffset(params.offset);
        ui->spWriteCount->setValue(params.count);
        ui->cmbWriteFormat->setCurrentText(mb::enumFormatKey(params.format));
        ui->txtWriteData->setPlainText(params.data);
        break;
    case MBF_REPORT_SERVER_ID:
        break;
    case MBF_MASK_WRITE_REGISTER:
        setWriteOffset(params.offset);
        ui->cmbWriteFormat->setCurrentText(mb::enumFormatKey(params.format));
        ui->txtWriteData->setPlainText(params.data);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        setReadOffset(params.offset);
        ui->spReadCount->setValue(params.count);
        setWriteOffset(params.writeOffset);
        ui->spWriteCount->setValue(params.writeCount);
        ui->cmbWriteFormat->setCurrentText(mb::enumFormatKey(params.format));
        ui->txtWriteData->setPlainText(params.data);
        break;
    default:
        return;
    }
}

void mbClientSendMessageUi::fillForm(const mbClientRunMessagePtr &message)
{
    QStringList ls;
    mb::Format format = mb::enumFormatValueByIndex(ui->cmbReadFormat->currentIndex());
    switch (message->function())
    {
    case MBF_READ_COILS:
    case MBF_READ_DISCRETE_INPUTS:
    {
        uint16_t count = message->count();
        QByteArray data((count+7)/8, '\0');
        message->getData(0, count, data.data());
        switch (format)
        {
        case mb::Bool:
            ls = toStringListBits(data, count);
            break;
        case mb::ByteArray:
        case mb::String:
            ls.append(mb::toVariant(data,
                                    format,
                                    Modbus::Memory_0x,
                                    m_dataParams.byteOrder,
                                    m_dataParams.registerOrder,
                                    m_dataParams.byteArrayFormat,
                                    m_dataParams.stringEncoding,
                                    m_dataParams.stringLengthType,
                                    m_dataParams.byteArraySeparator,
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format);
            break;
        }
    }
        break;
    case MBF_READ_EXCEPTION_STATUS:
    {
        uint16_t count = 8;
        QByteArray data((count+7)/8, '\0');
        message->getData(0, count, data.data());
        switch (format)
        {
        case mb::Bool:
            ls = toStringListBits(data, count);
            break;
        case mb::ByteArray:
        case mb::String:
            ls.append(mb::toVariant(data,
                                    format,
                                    Modbus::Memory_0x,
                                    m_dataParams.byteOrder,
                                    m_dataParams.registerOrder,
                                    m_dataParams.byteArrayFormat,
                                    m_dataParams.stringEncoding,
                                    m_dataParams.stringLengthType,
                                    m_dataParams.byteArraySeparator,
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format);
            break;
        }
    }
        break;
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
    {
        uint16_t count = message->count();
        QByteArray data(count * 2, '\0');
        message->getData(0, count, data.data());
        switch (format)
        {
        case mb::Bool:
            ls = toStringListBits(data, count * 16);
            break;
        case mb::ByteArray:
        case mb::String:
            ls.append(mb::toVariant(data,
                                    format,
                                    Modbus::Memory_0x,
                                    m_dataParams.byteOrder,
                                    m_dataParams.registerOrder,
                                    m_dataParams.byteArrayFormat,
                                    m_dataParams.stringEncoding,
                                    m_dataParams.stringLengthType,
                                    m_dataParams.byteArraySeparator,
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format);
            break;
        }
    }
        break;
    case MBF_REPORT_SERVER_ID:
    {
        uint16_t count = message->count();
        QByteArray data(count, '\0');
        message->getData(0, count, data.data());
        switch (format)
        {
        case mb::Bool:
            ls = toStringListBits(data, count);
            break;
        case mb::ByteArray:
        case mb::String:
            ls.append(mb::toVariant(data,
                                    format,
                                    Modbus::Memory_0x,
                                    m_dataParams.byteOrder,
                                    m_dataParams.registerOrder,
                                    m_dataParams.byteArrayFormat,
                                    m_dataParams.stringEncoding,
                                    m_dataParams.stringLengthType,
                                    m_dataParams.byteArraySeparator,
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format);
            break;
        }
    }
        break;
    default:
        return;
    }
    ui->txtReadData->clear();
    if (ls.count())
    {
        QStringList::ConstIterator it = ls.constBegin();
        QString s = *it;
        for (++it; it != ls.constEnd(); ++it)
            s += QStringLiteral(",") + *it;
        ui->txtReadData->setPlainText(s);
    }
}

QStringList mbClientSendMessageUi::toStringListBits(const QByteArray &data, uint16_t count)
{
    QStringList ls;
    for (int i = 0; i < count; i++)
    {
        bool v = data.at(i / 8) & (1 << (i % 8));
        QString s = v ? QStringLiteral("1") : QStringLiteral("0");
        ls.append(s);
    }
    return ls;
}

QStringList mbClientSendMessageUi::toStringListNumbers(const QByteArray &data, mb::Format format)
{
    QStringList ls;
    int sz = static_cast<int>(mb::sizeofFormat(format));
    int c = data.size() / sz;
    for (int i = 0; i < c; i++)
    {
        QByteArray numData = data.mid(i*sz, sz);
        QString s = mb::toVariant(numData,
                                  format,
                                  Modbus::Memory_4x,
                                  m_dataParams.byteOrder,
                                  m_dataParams.registerOrder,
                                  m_dataParams.byteArrayFormat,
                                  m_dataParams.stringEncoding,
                                  m_dataParams.stringLengthType,
                                  m_dataParams.byteArraySeparator,
                                  numData.count()).toString();
        ls.append(s);
    }
    size_t szRemainder = data.size() - c * sz;
    if (szRemainder)
    {
        quint64 v = 0;
        memcpy(&v, &data.data()[c*sz], szRemainder);
        QByteArray numData(reinterpret_cast<char*>(&v), sizeof(v));
        QString s = mb::toVariant(numData,
                                  format,
                                  Modbus::Memory_4x,
                                  m_dataParams.byteOrder,
                                  m_dataParams.registerOrder,
                                  m_dataParams.byteArrayFormat,
                                  m_dataParams.stringEncoding,
                                  m_dataParams.stringLengthType,
                                  m_dataParams.byteArraySeparator,
                                  numData.count()).toString();
        ls.append(s);
    }
    return ls;
}

QByteArray mbClientSendMessageUi::fromStringListBits(const QStringList &ls)
{
    QByteArray r((ls.count()+7)/8, '\0');
    int i = 0;
    Q_FOREACH(const QString &s, ls)
    {
        if (s == QStringLiteral("1"))
            r.data()[i / 8] |= (1 << (i % 8));
        i++;
    }
    return r;
}

QByteArray mbClientSendMessageUi::fromStringListNumbers(const QStringList &ls, mb::Format format)
{
    QByteArray data;
    Q_FOREACH(const QString &s, ls)
        data.append(mb::toByteArray(s,
                                    format,
                                    Modbus::Memory_4x,
                                    m_dataParams.byteOrder,
                                    m_dataParams.registerOrder,
                                    m_dataParams.byteArrayFormat,
                                    m_dataParams.stringEncoding,
                                    m_dataParams.stringLengthType,
                                    m_dataParams.byteArraySeparator,
                                    0));

    return data;
}

bool mbClientSendMessageUi::fromStringNumber(mb::Format format, const QString &v, void *buff)
{
    bool ok = false;
    switch (format)
    {
    case mb::Bin16:
        *reinterpret_cast<quint16*>(buff) = v.toUShort(&ok, 2);
        break;
    case mb::Oct16:
        *reinterpret_cast<quint16*>(buff) = v.toUShort(&ok, 8);
        break;
    case mb::Dec16:
        *reinterpret_cast<qint16*>(buff) = v.toShort(&ok, 10);
        break;
    case mb::UDec16:
        *reinterpret_cast<quint16*>(buff) = v.toUShort(&ok, 10);
        break;
    case mb::Hex16:
        *reinterpret_cast<quint16*>(buff) = v.toUShort(&ok, 16);
        break;
    case mb::Bin32:
        *reinterpret_cast<quint32*>(buff) = v.toULong(&ok, 2);
        break;
    case mb::Oct32:
        *reinterpret_cast<quint32*>(buff) = v.toULong(&ok, 8);
        break;
    case mb::Dec32:
        *reinterpret_cast<qint32*>(buff) = v.toLong(&ok, 10);
        break;
    case mb::UDec32:
        *reinterpret_cast<quint32*>(buff) = v.toULong(&ok, 10);
        break;
    case mb::Hex32:
        *reinterpret_cast<quint32*>(buff) = v.toULong(&ok, 16);
        break;
    case mb::Bin64:
        *reinterpret_cast<quint64*>(buff) = v.toULongLong(&ok, 2);
        break;
    case mb::Oct64:
        *reinterpret_cast<quint64*>(buff) = v.toULongLong(&ok, 8);
        break;
    case mb::Dec64:
        *reinterpret_cast<qint64*>(buff) = v.toLongLong(&ok, 10);
        break;
    case mb::UDec64:
        *reinterpret_cast<quint64*>(buff) = v.toULongLong(&ok, 10);
        break;
    case mb::Hex64:
        *reinterpret_cast<quint64*>(buff) = v.toULongLong(&ok, 16);
        break;
    case mb::Float:
        *reinterpret_cast<float*>(buff) = v.toFloat(&ok);
        break;
    case mb::Double:
        *reinterpret_cast<double*>(buff) = v.toDouble(&ok);
        break;
    default:
        break;
    }
    return ok;
}

QStringList mbClientSendMessageUi::dataToStringList(const QString &s)
{
    QStringList ls = s.split(',');
    return ls;
}

uint8_t mbClientSendMessageUi::getCurrentFuncNum() const
{
    return m_funcNums.value(ui->cmbFunction->currentIndex());
}

void mbClientSendMessageUi::setCurrentFuncNum(uint8_t func)
{
    switch(func)
    {
    case MBF_READ_COILS:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(false);
        m_readAddress->setAddressType(Modbus::Memory_0x);
        m_readAddress->setEnabledAddress(true);
        ui->spReadCount->setEnabled(true);
        break;
    case MBF_READ_DISCRETE_INPUTS:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(false);
        m_readAddress->setAddressType(Modbus::Memory_1x);
        m_readAddress->setEnabledAddress(true);
        ui->spReadCount->setEnabled(true);
        break;
    case MBF_READ_HOLDING_REGISTERS:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(false);
        m_readAddress->setAddressType(Modbus::Memory_4x);
        m_readAddress->setEnabledAddress(true);
        ui->spReadCount->setEnabled(true);
        break;
    case MBF_READ_INPUT_REGISTERS:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(false);
        m_readAddress->setAddressType(Modbus::Memory_3x);
        m_readAddress->setEnabledAddress(true);
        ui->spReadCount->setEnabled(true);
        break;
    case MBF_WRITE_SINGLE_COIL:
        ui->grReadData->setVisible(false);
        ui->grWriteData->setVisible(true);
        m_writeAddress->setAddressType(Modbus::Memory_0x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(false);
        ui->swWriteData->setCurrentWidget(ui->pgWriteData);
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        ui->grReadData->setVisible(false);
        ui->grWriteData->setVisible(true);
        m_writeAddress->setAddressType(Modbus::Memory_4x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(false);
        ui->swWriteData->setCurrentWidget(ui->pgWriteData);
        break;
    case MBF_READ_EXCEPTION_STATUS:
    case MBF_REPORT_SERVER_ID:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(false);
        m_readAddress->setEnabledAddress(false);
        ui->spReadCount->setEnabled(false);
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        ui->grReadData->setVisible(false);
        ui->grWriteData->setVisible(true);
        m_writeAddress->setAddressType(Modbus::Memory_0x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(true);
        ui->swWriteData->setCurrentWidget(ui->pgWriteData);
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        ui->grReadData->setVisible(false);
        ui->grWriteData->setVisible(true);
        m_writeAddress->setAddressType(Modbus::Memory_4x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(true);
        ui->swWriteData->setCurrentWidget(ui->pgWriteData);
        break;
    case MBF_MASK_WRITE_REGISTER:
        ui->grReadData->setVisible(false);
        ui->grWriteData->setVisible(true);
        m_writeAddress->setAddressType(Modbus::Memory_4x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(false);
        ui->swWriteData->setCurrentWidget(ui->pgWriteMask);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        ui->grReadData->setVisible(true);
        ui->grWriteData->setVisible(true);
        m_readAddress->setAddressType(Modbus::Memory_4x);
        m_readAddress->setEnabledAddress(true);
        ui->spReadCount->setEnabled(true);
        m_writeAddress->setAddressType(Modbus::Memory_4x);
        m_writeAddress->setEnabledAddress(true);
        ui->spWriteCount->setEnabled(true);
        ui->swWriteData->setCurrentWidget(ui->pgWriteData);
        break;
    }
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

QStringList mbClientSendMessageUi::saveMessages(const QList<const mbClientSendMessageParams *> messages)
{
    QStringList res;
    Q_FOREACH(const mbClientSendMessageParams *p, messages)
    {
        res.append(saveParams(*p));
    }
    return res;
}

QList<const mbClientSendMessageParams *> mbClientSendMessageUi::restoreMessages(const QStringList &messages)
{
    QList<const mbClientSendMessageParams *> res;
    Q_FOREACH(const QString &p, messages)
    {
        res.append(restoreParams(p));
    }
    return res;
}

QString mbClientSendMessageUi::saveParams(const mbClientSendMessageParams &params)
{
    QString res = QString("func=%1").arg(params.func);
    switch(params.func)
    {
    case MBF_READ_COILS:
        res += QString(";offset=%1;count=%2").arg(params.offset).arg(params.count);
        break;
    case MBF_READ_DISCRETE_INPUTS:
        res += QString(";offset=%1;count=%2").arg(params.offset).arg(params.count);
        break;
    case MBF_READ_HOLDING_REGISTERS:
        res += QString(";offset=%1;count=%2").arg(params.offset).arg(params.count);
        break;
    case MBF_READ_INPUT_REGISTERS:
        res += QString(";offset=%1;count=%2").arg(params.offset).arg(params.count);
        break;
    case MBF_WRITE_SINGLE_COIL:
        res += QString(";offset=%1;data=%2").arg(params.offset).arg(mb::enumFormatKey(params.format), params.data);
        break;
    case MBF_WRITE_SINGLE_REGISTER:
        res += QString(";offset=%1;format=%2;data=%3").arg(params.offset).arg(mb::enumFormatKey(params.format), params.data);
        break;
    case MBF_READ_EXCEPTION_STATUS:
        break;
    case MBF_WRITE_MULTIPLE_COILS:
        res += QString(";offset=%1;count=%2;format=%3;data=%4")
                   .arg(params.offset).arg(params.count).arg(mb::enumFormatKey(params.format), params.data);
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        res += QString(";offset=%1;count=%2;format=%3;data=%4")
                   .arg(params.offset).arg(params.count).arg(mb::enumFormatKey(params.format), params.data);
        break;
    case MBF_REPORT_SERVER_ID:
        break;
    case MBF_MASK_WRITE_REGISTER:
        res += QString(";offset=%1;data=%2").arg(params.offset).arg(params.data);
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
        res += QString(";readoffset=%1;readcount=%2;writeoffset=%3;writecount=%4;format=%5;data=%6")
                   .arg(params.offset).arg(params.count).arg(params.writeOffset).arg(params.writeCount).arg(params.data);
        break;
    default:
        break;
    }
    return res;
}

