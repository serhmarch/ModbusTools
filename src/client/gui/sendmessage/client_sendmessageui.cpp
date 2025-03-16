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

#include <client.h>

#include <project/client_project.h>
#include <project/client_port.h>
#include <project/client_device.h>

#include <runtime/client_runmessage.h>

#include <gui/widgets/core_addresswidget.h>

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

    connect(ui->btnSendOne          , &QPushButton::clicked, this, &mbClientSendMessageUi::sendOne);
    connect(ui->btnSendPeriodically , &QPushButton::clicked, this, &mbClientSendMessageUi::sendPeriodically);
    connect(ui->btnStop             , &QPushButton::clicked, this, &mbClientSendMessageUi::stopSending);
    connect(ui->btnClose            , &QPushButton::clicked, this, &QDialog::close);

    connect(core, &mbClient::projectChanged, this, &mbClientSendMessageUi::setProject);
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

void mbClientSendMessageUi::setCurrentFuncIndex(int i)
{
    uint8_t funcNum = m_funcNums.value(i);
    setCurrentFuncNum(funcNum);
}

void mbClientSendMessageUi::sendOne()
{
    mbClient *core = mbClient::global();
    if (!core->isRunning())
        core->start();
    if (!prepareSendParams())
        return;
    createMessage();
    if (!m_message)
        return;
    fillData(m_message);
    switch (m_sendTo)
    {
    case SendToPortUnit:
    {
        mbClientPort *port = currentPort();
        if (!port)
            return;
        mbClient::global()->sendPortMessage(port->handle(), m_message);
    }
    break;
    default:
    {
        mbClientDevice *device = currentDevice();
        if (!device)
            return;
        mbClient::global()->sendMessage(device->handle(), m_message);
    }
        break;
    }

}

void mbClientSendMessageUi::sendPeriodically()
{
    if (m_timer)
        return;
    sendOne();
    if (!m_message)
        return;
    int msec = ui->spPeriod->value();
    m_timer = startTimer(msec);
    setEnableParams(false);
}

void mbClientSendMessageUi::stopSending()
{
    if (m_timer)
    {
        killTimer(m_timer);
        m_timer = 0;
        setEnableParams(true);
    }
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
    }
}

void mbClientSendMessageUi::createMessage()
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return;
    uint8_t func = getCurrentFuncNum();
    m_message = nullptr;
    switch(func)
    {
    case MBF_READ_COILS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getReadAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spReadCount  ->value());
        m_message = new mbClientRunMessageReadCoils(offset, count,  m_dataParams.maxReadCoils, this);
    }
        break;
    case MBF_READ_DISCRETE_INPUTS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getReadAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spReadCount  ->value());
        m_message = new mbClientRunMessageReadDiscreteInputs(offset, count,  m_dataParams.maxReadDiscreteInputs, this);
    }
        break;
    case MBF_READ_HOLDING_REGISTERS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getReadAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spReadCount  ->value());
        m_message = new mbClientRunMessageReadHoldingRegisters(offset, count,  m_dataParams.maxReadHoldingRegisters, this);
    }
        break;
    case MBF_READ_INPUT_REGISTERS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getReadAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spReadCount  ->value());
        m_message = new mbClientRunMessageReadInputRegisters(offset, count, m_dataParams.maxReadInputRegisters, this);
    }
        break;
    case MBF_WRITE_SINGLE_COIL:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        m_message = new mbClientRunMessageWriteSingleCoil(offset, this);
    }
        break;
    case MBF_WRITE_SINGLE_REGISTER:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        m_message = new mbClientRunMessageWriteSingleRegister(offset, this);
    }
        break;
    case MBF_READ_EXCEPTION_STATUS:
    {
        m_message = new mbClientRunMessageReadExceptionStatus(this);
    }
        break;
    case MBF_WRITE_MULTIPLE_COILS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spWriteCount  ->value());
        m_message = new mbClientRunMessageWriteMultipleCoils(offset, count,  m_dataParams.maxWriteMultipleCoils, this);
    }
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        uint16_t count  = static_cast<uint16_t>(ui->spWriteCount  ->value());
        m_message = new mbClientRunMessageWriteMultipleRegisters(offset, count, m_dataParams.maxWriteMultipleRegisters, this);
    }
        break;
    case MBF_REPORT_SERVER_ID:
    {
        m_message = new mbClientRunMessageReportServerID(this);
    }
        break;
    case MBF_MASK_WRITE_REGISTER:
    {
        uint16_t offset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        m_message = new mbClientRunMessageMaskWriteRegister(offset, this);
    }
        break;
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
    {
        uint16_t readOffset  = static_cast<uint16_t>(Modbus::toModbusOffset(getReadAddress()));
        uint16_t readCount   = static_cast<uint16_t>(ui->spReadCount   ->value());
        uint16_t writeOffset = static_cast<uint16_t>(Modbus::toModbusOffset(getWriteAddress()));
        uint16_t writeCount  = static_cast<uint16_t>(ui->spWriteCount  ->value());
        m_message = new mbClientRunMessageReadWriteMultipleRegisters(readOffset, readCount, writeOffset, writeCount, this);
    }
        break;
    default:
        return;
    }
    connect(m_message, &mbClientRunMessage::signalBytesTx, this, &mbClientSendMessageUi::slotBytesTx);
    connect(m_message, &mbClientRunMessage::signalBytesRx, this, &mbClientSendMessageUi::slotBytesRx);
    connect(m_message, &mbClientRunMessage::signalAsciiTx, this, &mbClientSendMessageUi::slotAsciiTx);
    connect(m_message, &mbClientRunMessage::signalAsciiRx, this, &mbClientSendMessageUi::slotAsciiRx);
    connect(m_message, &mbClientRunMessage::completed, this, &mbClientSendMessageUi::messageCompleted);
    if (m_sendTo == SendToPortUnit)
        m_message->setUnit(m_unit);
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

void mbClientSendMessageUi::setEnableParams(bool v)
{
    ui->grCommonParams->setEnabled(v);
    ui->grReadData->setEnabled(v);
    ui->grWriteData->setEnabled(v);
    ui->spPeriod->setEnabled(v);
    ui->btnSendPeriodically->setEnabled(v);
}

int mbClientSendMessageUi::getReadAddress() const
{
    return m_readAddress->getAddress().offset+1;
}

void mbClientSendMessageUi::setReadAddress(int v)
{
    mb::Address adr = m_readAddress->getAddress();
    adr.offset = v-1;
    m_readAddress->setAddress(adr);
}

int mbClientSendMessageUi::getWriteAddress() const
{
    return m_writeAddress->getAddress().offset+1;
}

void mbClientSendMessageUi::setWriteAddress(int v)
{
    mb::Address adr = m_writeAddress->getAddress();
    adr.offset = v-1;
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

void mbClientSendMessageUi::timerEvent(QTimerEvent */*event*/)
{
    //mbClientDevice *device = currentDevice();
    //if (!device)
    //    return;
    if (m_message->isCompleted())
    {
        m_message->clearCompleted();
        if (m_sendTo == SendToPortUnit)
        {
            mbClientPort *port = currentPort();
            if (!port)
                return;
            mbClient::global()->sendPortMessage(port->handle(), m_message);
        }
        else
        {
            mbClientDevice *device = currentDevice();
            if (!device)
                return;
            mbClient::global()->sendMessage(device->handle(), m_message);
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

void mbClientSendMessageUi::fillData(mbClientRunMessagePtr &message)
{
    mb::Format format = mb::enumFormatValueByIndex(ui->cmbWriteFormat->currentIndex());
    QByteArray data;
    uint16_t c;
    QString s = ui->txtWriteData->toPlainText();
    switch (message->function())
    {
    case MBF_WRITE_SINGLE_COIL:
    {
        char v = (s == QStringLiteral("1"));
        data = QByteArray(v, 1);
        c = 1;
    }
        break;
    case MBF_WRITE_SINGLE_REGISTER:
    {
        switch (format)
        {
        case mb::Bool:
        {
            QStringList ls = params(s);
            data = fromStringListBits(ls);
        }
            break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(s,
                                   format,
                                   Modbus::Memory_4x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   2);
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format);
        }
            break;
        }
        c = 1;
        if (data.count() && (data.count() < 2))
            data.append('\0');
    }
        break;
    case MBF_WRITE_MULTIPLE_COILS:
    {
        switch (format)
        {
        case mb::Bool:
        {
            QStringList ls = params(s);
            data = fromStringListBits(ls);
            c = ls.count();
        }
            break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(s,
                                   format,
                                   Modbus::Memory_0x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   (message->count()+7)/8);
            c = data.count() * 8;
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format);
            c = data.count() * 8;
        }
            break;
        }
    }
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
    {
        switch (format)
        {
        case mb::Bool:
        {
            QStringList ls = params(s);
            data = fromStringListBits(ls);
        }
            break;
        case mb::ByteArray:
        case mb::String:
            data = mb::toByteArray(s,
                                   format,
                                   Modbus::Memory_0x,
                                   m_dataParams.byteOrder,
                                   m_dataParams.registerOrder,
                                   m_dataParams.byteArrayFormat,
                                   m_dataParams.stringEncoding,
                                   m_dataParams.stringLengthType,
                                   m_dataParams.byteArraySeparator,
                                   message->count()*2);
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format);
        }
            break;
        }
    }
        if (data.count() && (data.count() & 1))
            data.append('\0');
        c = data.count() / 2;
        break;
    case MBF_MASK_WRITE_REGISTER:
    {
        struct { quint16 andMask; quint16 orMask; } v;
        v.andMask = static_cast<quint16>(ui->spWriteMaskAnd->value());
        v.orMask  = static_cast<quint16>(ui->spWriteMaskOr ->value());
        data = QByteArray(reinterpret_cast<char*>(&v), sizeof(v));
        c = 2;
    }
        break;
    default:
        return;
    }
    if (data.count())
    {
        message->setData(0, c, data.data());
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

QStringList mbClientSendMessageUi::params(const QString &s)
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
