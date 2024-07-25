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
#include "client_dialogsendmessage.h"
#include "ui_client_dialogsendmessage.h"

#include <client.h>

#include <project/client_project.h>
#include <project/client_device.h>

#include <runtime/client_runmessage.h>

mbClientDialogSendMessage::mbClientDialogSendMessage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbClientDialogSendMessage)
{
    ui->setupUi(this);
    m_project = nullptr;
    m_timer = 0;

    QStringList ls;
    QMetaEnum e;
    QComboBox *cmb;
    QSpinBox *sp;

    mbClient *core = mbClient::global();

    // Address type
    // Note: need to initialize earlier because it's used in setModbusFunction
    cmb = ui->cmbAdrType;
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
    cmb->addItem(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
    cmb->setCurrentIndex(3);
    cmb->setEnabled(false);

    cmb = ui->cmbFunction;
    connect(cmb, &QComboBox::currentTextChanged, this, &mbClientDialogSendMessage::setModbusFunction);
    cmb->addItem(mb::ModbusFunctionString(MBF_READ_COILS              ));
    cmb->addItem(mb::ModbusFunctionString(MBF_READ_DISCRETE_INPUTS    ));
    cmb->addItem(mb::ModbusFunctionString(MBF_READ_HOLDING_REGISTERS  ));
    cmb->addItem(mb::ModbusFunctionString(MBF_READ_INPUT_REGISTERS    ));
    cmb->addItem(mb::ModbusFunctionString(MBF_WRITE_SINGLE_COIL       ));
    cmb->addItem(mb::ModbusFunctionString(MBF_WRITE_SINGLE_REGISTER   ));
    cmb->addItem(mb::ModbusFunctionString(MBF_READ_EXCEPTION_STATUS   ));
    cmb->addItem(mb::ModbusFunctionString(MBF_WRITE_MULTIPLE_COILS    ));
    cmb->addItem(mb::ModbusFunctionString(MBF_WRITE_MULTIPLE_REGISTERS));
    cmb->setCurrentText(mb::ModbusFunctionString(MBF_READ_HOLDING_REGISTERS));

    // Offset
    sp = ui->spOffset;
    sp->setMinimum(1);
    sp->setMaximum(USHRT_MAX+1);

    cmb = ui->cmbFormat;
    ls = mb::enumFormatKeyList();
    Q_FOREACH (const QString &s, ls)
        cmb->addItem(s);
    cmb->setCurrentIndex(mb::Dec16);

    sp = ui->spCount;
    sp->setMinimum(1);
    sp->setMaximum(MB_MAX_DISCRETS); // TODO: if register was choosen than change this value

    sp = ui->spPeriod;
    sp->setMinimum(1);
    sp->setMaximum(INT_MAX);
    sp->setValue(1000);

    connect(ui->btnSendOne          , &QPushButton::clicked, this, &mbClientDialogSendMessage::sendOne);
    connect(ui->btnSendPeriodically , &QPushButton::clicked, this, &mbClientDialogSendMessage::sendPeriodically);
    connect(ui->btnStop             , &QPushButton::clicked, this, &mbClientDialogSendMessage::stopSending);
    connect(ui->btnClose            , &QPushButton::clicked, this, &QDialog::close);

    connect(core, &mbClient::projectChanged, this, &mbClientDialogSendMessage::setProject);
    setProject(core->project());
}

mbClientDialogSendMessage::~mbClientDialogSendMessage()
{
    delete ui;
}

void mbClientDialogSendMessage::setProject(mbCoreProject *p)
{
    mbClientProject *project = static_cast<mbClientProject*>(p);
    if (m_project != project)
    {
        if (m_project)
        {
            project->disconnect(this);
            ui->cmbDevice->clear();
        }
        m_project = project;
        if (m_project)
        {
            QList<mbClientDevice*> devices = m_project->devices();
            connect(m_project, &mbClientProject::deviceAdded   , this, &mbClientDialogSendMessage::addDevice   );
            connect(m_project, &mbClientProject::deviceRemoving, this, &mbClientDialogSendMessage::removeDevice);
            connect(m_project, &mbClientProject::deviceRenaming, this, &mbClientDialogSendMessage::renameDevice);
            Q_FOREACH (mbClientDevice *d, devices)
                addDevice(d);
        }
    }
}

void mbClientDialogSendMessage::addDevice(mbCoreDevice *device)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->insertItem(i, device->name());
}

void mbClientDialogSendMessage::removeDevice(mbCoreDevice *device)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->removeItem(i);
}

void mbClientDialogSendMessage::renameDevice(mbCoreDevice *device, const QString newName)
{
    int i = m_project->deviceIndex(device);
    ui->cmbDevice->setItemText(i, newName);
}

void mbClientDialogSendMessage::setModbusFunction(const QString &s)
{
    uint8_t func = mb::ModbusFunction(s);
    switch(func)
    {
    case MBF_READ_COILS              :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(true);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
        break;
    case MBF_READ_DISCRETE_INPUTS    :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(true);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_1x));
        break;
    case MBF_READ_HOLDING_REGISTERS  :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(true);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
        break;
    case MBF_READ_INPUT_REGISTERS    :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(true);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_3x));
        break;
    case MBF_WRITE_SINGLE_COIL       :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(false);
        ui->txtData->setReadOnly(false);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
        break;
    case MBF_WRITE_SINGLE_REGISTER   :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(false);
        ui->txtData->setReadOnly(false);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
        break;
    case MBF_READ_EXCEPTION_STATUS   :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgEmpty);
        ui->spCount->setEnabled(false);
        ui->txtData->setReadOnly(true);
        break;
    case MBF_WRITE_MULTIPLE_COILS    :
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(false);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_0x));
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
        ui->swModbusFuncParams->setCurrentWidget(ui->pgParams);
        ui->spCount->setEnabled(true);
        ui->txtData->setReadOnly(false);
        ui->cmbAdrType->setCurrentText(mb::toModbusMemoryTypeString(Modbus::Memory_4x));
        break;
    }
}

void mbClientDialogSendMessage::sendOne()
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return;
    createMessage();
    if (!m_message)
        return;
    fillData(device, m_message);
    mbClient::global()->sendMessage(device->handle(), m_message);
}

void mbClientDialogSendMessage::sendPeriodically()
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

void mbClientDialogSendMessage::stopSending()
{
    if (m_timer)
    {
        killTimer(m_timer);
        m_timer = 0;
        setEnableParams(true);
    }
}

void mbClientDialogSendMessage::slotBytesTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientDialogSendMessage::slotBytesRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::bytesToString(bytes));
}

void mbClientDialogSendMessage::slotAsciiTx(const QByteArray &bytes)
{
    ui->txtModbusTx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientDialogSendMessage::slotAsciiRx(const QByteArray &bytes)
{
    ui->txtModbusRx->setPlainText(Modbus::asciiToString(bytes));
}

void mbClientDialogSendMessage::messageCompleted()
{
    mbClientRunMessagePtr message = qobject_cast<mbClientRunMessage*>(sender());
    if (message)
    {
        ui->lnStatus   ->setText(mb::toString(message->status   ()));
        ui->lnTimestamp->setText(mb::toString(message->timestamp()));
        fillForm(currentDevice(), message);
    }
}

void mbClientDialogSendMessage::createMessage()
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return;
    QString s = ui->cmbFunction->currentText();
    uint8_t func = mb::ModbusFunction(s);
    m_message = nullptr;
    switch(func)
    {
    case MBF_READ_COILS              :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageReadCoils(offset, count, device->maxReadCoils(), this);
    }
        break;
    case MBF_READ_DISCRETE_INPUTS    :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageReadDiscreteInputs(offset, count, device->maxReadDiscreteInputs(), this);
    }
        break;
    case MBF_READ_HOLDING_REGISTERS  :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageReadHoldingRegisters(offset, count, device->maxReadHoldingRegisters(), this);
    }
        break;
    case MBF_READ_INPUT_REGISTERS    :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageReadInputRegisters(offset, count, device->maxReadInputRegisters(), this);
    }
        break;
    case MBF_WRITE_SINGLE_COIL       :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        m_message = new mbClientRunMessageWriteSingleCoil(offset, this);
    }
        break;
    case MBF_WRITE_SINGLE_REGISTER   :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value());
        m_message = new mbClientRunMessageWriteSingleRegister(offset, this);
    }
        break;
    case MBF_READ_EXCEPTION_STATUS   :
    {
        m_message = new mbClientRunMessageReadExceptionStatus(this);
    }
        break;
    case MBF_WRITE_MULTIPLE_COILS    :
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageWriteMultipleCoils(offset, count, device->maxWriteMultipleCoils(), this);
    }
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
    {
        uint16_t offset = static_cast<uint16_t>(ui->spOffset->value()-1);
        uint16_t count  = static_cast<uint16_t>(ui->spCount ->value());
        m_message = new mbClientRunMessageWriteMultipleRegisters(offset, count, device->maxWriteMultipleRegisters(), this);
    }
        break;
    default:
        return;
    }
    connect(m_message, &mbClientRunMessage::signalBytesTx, this, &mbClientDialogSendMessage::slotBytesTx);
    connect(m_message, &mbClientRunMessage::signalBytesRx, this, &mbClientDialogSendMessage::slotBytesRx);
    connect(m_message, &mbClientRunMessage::signalAsciiTx, this, &mbClientDialogSendMessage::slotAsciiTx);
    connect(m_message, &mbClientRunMessage::signalAsciiRx, this, &mbClientDialogSendMessage::slotAsciiRx);
    connect(m_message, &mbClientRunMessage::completed, this, &mbClientDialogSendMessage::messageCompleted);
}

mbClientDevice *mbClientDialogSendMessage::currentDevice() const
{
    if (m_project)
    {
        QString name = ui->cmbDevice->currentText();
        return m_project->device(name);
    }
    return nullptr;
}

void mbClientDialogSendMessage::setEnableParams(bool v)
{
    ui->grModbusParams->setEnabled(v);
    ui->grData->setEnabled(v);
    ui->spPeriod->setEnabled(v);
    ui->btnSendOne->setEnabled(v);
    ui->btnSendPeriodically->setEnabled(v);
}

void mbClientDialogSendMessage::timerEvent(QTimerEvent */*event*/)
{
    mbClientDevice *device = currentDevice();
    if (!device)
        return;
    if (m_message->isCompleted())
    {
        m_message->clearCompleted();
        mbClient::global()->sendMessage(device->handle(), m_message);
    }
}

void mbClientDialogSendMessage::fillForm(mbClientDevice *device, const mbClientRunMessagePtr &message)
{
    QStringList ls;
    mb::Format format = mb::enumFormatValueByIndex(ui->cmbFormat->currentIndex());
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
                                    mb::LessSignifiedFirst,
                                    device->registerOrder(),
                                    device->byteArrayFormat(),
                                    device->stringEncoding(),
                                    device->stringLengthType(),
                                    device->byteArraySeparator(),
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format, device);
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
                                    mb::LessSignifiedFirst,
                                    device->registerOrder(),
                                    device->byteArrayFormat(),
                                    device->stringEncoding(),
                                    device->stringLengthType(),
                                    device->byteArraySeparator(),
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format, device);
            break;
        }
    }
    break;
    case MBF_READ_HOLDING_REGISTERS:
    case MBF_READ_INPUT_REGISTERS:
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
                                    mb::LessSignifiedFirst,
                                    device->registerOrder(),
                                    device->byteArrayFormat(),
                                    device->stringEncoding(),
                                    device->stringLengthType(),
                                    device->byteArraySeparator(),
                                    data.count()).toString());
            break;
        default:
            ls = toStringListNumbers(data, format, device);
            break;
        }
    }
        break;
    default:
        return;
    }
    ui->txtData->clear();
    if (ls.count())
    {
        QStringList::ConstIterator it = ls.constBegin();
        QString s = *it;
        for (++it; it != ls.constEnd(); ++it)
            s += QStringLiteral(",") + *it;
        ui->txtData->setPlainText(s);
    }
}

void mbClientDialogSendMessage::fillData(mbClientDevice *device, mbClientRunMessagePtr &message)
{
    mb::Format format = mb::enumFormatValueByIndex(ui->cmbFormat->currentIndex());
    QByteArray data;
    uint16_t c;
    QString s = ui->txtData->toPlainText();
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
                                   mb::LessSignifiedFirst,
                                   device->registerOrder(),
                                   device->byteArrayFormat(),
                                   device->stringEncoding(),
                                   device->stringLengthType(),
                                   device->byteArraySeparator(),
                                   2);
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format, device);
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
                                   mb::LessSignifiedFirst,
                                   device->registerOrder(),
                                   device->byteArrayFormat(),
                                   device->stringEncoding(),
                                   device->stringLengthType(),
                                   device->byteArraySeparator(),
                                   (message->count()+7)/8);
            c = data.count() * 8;
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format, device);
            c = data.count() * 8;
        }
            break;
        }
    }
        break;
    case MBF_WRITE_MULTIPLE_REGISTERS:
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
                                   mb::LessSignifiedFirst,
                                   device->registerOrder(),
                                   device->byteArrayFormat(),
                                   device->stringEncoding(),
                                   device->stringLengthType(),
                                   device->byteArraySeparator(),
                                   message->count()*2);
            break;
        default:
        {
            QStringList ls = params(s);
            data = fromStringListNumbers(ls, format, device);
        }
            break;
        }
    }
        if (data.count() && (data.count() & 1))
            data.append('\0');
        c = data.count() / 2;
        break;
    default:
        return;
    }
    if (data.count())
    {
        message->setData(0, c, data.data());
    }
}

QStringList mbClientDialogSendMessage::toStringListBits(const QByteArray &data, uint16_t count)
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

QStringList mbClientDialogSendMessage::toStringListNumbers(const QByteArray &data, mb::Format format, mbClientDevice *device)
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
                                  mb::LessSignifiedFirst,
                                  device->registerOrder(),
                                  device->byteArrayFormat(),
                                  device->stringEncoding(),
                                  device->stringLengthType(),
                                  device->byteArraySeparator(),
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
                                  mb::LessSignifiedFirst,
                                  device->registerOrder(),
                                  device->byteArrayFormat(),
                                  device->stringEncoding(),
                                  device->stringLengthType(),
                                  device->byteArraySeparator(),
                                  numData.count()).toString();
        ls.append(s);
    }
    return ls;
}

QByteArray mbClientDialogSendMessage::fromStringListBits(const QStringList &ls)
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

QByteArray mbClientDialogSendMessage::fromStringListNumbers(const QStringList &ls, mb::Format format, mbClientDevice *device)
{
    QByteArray data;
    Q_FOREACH(const QString &s, ls)
        data.append(mb::toByteArray(s,
                                    format,
                                    Modbus::Memory_4x,
                                    mb::LessSignifiedFirst,
                                    device->registerOrder(),
                                    device->byteArrayFormat(),
                                    device->stringEncoding(),
                                    device->stringLengthType(),
                                    device->byteArraySeparator(),
                                    0));

    return data;
}

bool mbClientDialogSendMessage::fromStringNumber(mb::Format format, const QString &v, void *buff)
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

QStringList mbClientDialogSendMessage::params(const QString &s)
{
    QStringList ls = s.split(',');
    return ls;
}
