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
#include "client_scannerthread.h"

#include <ModbusClientPort.h>
#include <ModbusSerialPort.h>
#include <ModbusTcpPort.h>

#include <client.h>

#include "client_scanner.h"

mbClientScannerThread::mbClientScannerThread(mbClientScanner *scanner, QObject *parent)
    : QThread(parent)
{
    m_scanner = scanner;
    m_ctrlRun = true;
    m_unitStart = Modbus::VALID_MODBUS_ADDRESS_BEGIN;
    m_unitEnd = Modbus::VALID_MODBUS_ADDRESS_END;
    moveToThread(this);
}

mbClientScannerThread::~mbClientScannerThread()
{
}

void mbClientScannerThread::setSettings(const Modbus::Settings &settings)
{
    const Modbus::Strings &s = Modbus::Strings::instance();

    m_settings = settings;
    m_unitStart = mbClientScanner::getSettingUnitStart(settings);
    m_unitEnd   = mbClientScanner::getSettingUnitEnd(settings)  ;
    m_request   = mbClientScanner::getSettingRequest(settings)  ;
    m_combinationCount = 1;

    m_divMods   .clear();
    m_names     .clear();
    m_valuesList.clear();

    Modbus::ProtocolType type = Modbus::getSettingType(settings);
    switch (type)
    {
    case Modbus::ASC:
    case Modbus::RTU:
    {
        QVariantList baudRate = mbClientScanner::getSettingBaudRate(settings);
        QVariantList dataBits = mbClientScanner::getSettingDataBits(settings);
        QVariantList parity   = mbClientScanner::getSettingParity  (settings);
        QVariantList stopBits = mbClientScanner::getSettingStopBits(settings);

#define DEFINE_COMBINATION_ELEMENT(elem)            \
        if (elem.count())                           \
        {                                           \
            DivMod dm;                              \
            dm.div = m_combinationCount;            \
            dm.mod = elem.count();                  \
            m_divMods.append(dm);                   \
            m_names.append(s.elem);                 \
            m_valuesList.append(elem);              \
            m_combinationCount *= elem.count();     \
        }

        DEFINE_COMBINATION_ELEMENT(stopBits)
        DEFINE_COMBINATION_ELEMENT(parity  )
        DEFINE_COMBINATION_ELEMENT(dataBits)
        DEFINE_COMBINATION_ELEMENT(baudRate)
    }
        break;
    default:
        break;
    }
    m_combinationCountAll = m_combinationCount * (m_unitEnd - m_unitStart + 1);
    m_statTx = 0;
    m_statRx = 0;
}

void mbClientScannerThread::run()
{
    const mbClientScanner::Strings &s = mbClientScanner::Strings::instance();
    m_ctrlRun = true;
    mbClient::LogInfo(s.name, QStringLiteral("Start scanning"));


    Modbus::Settings settings = m_settings;
    uint8_t dummy[MB_MAX_BYTES];
    memset(dummy, 0, sizeof(dummy));

    quint32 deviceCount = 0;
    for (int c = 0; m_ctrlRun && (c < m_combinationCount); c++)
    {
        // Get comibation number for each setting
        for (quint16 si = 0; si < m_names.count(); si++)
        {
            const QString &name = m_names.at(si);
            const DivMod &dm = m_divMods.at(si);
            // Calc value of each setting corresponding to current combination number
            int sc = (c / dm.div) % dm.mod;
            const QVariant &v = m_valuesList.at(si).at(sc);
            settings[name] = v;
        }
        ModbusClientPort *clientPort = Modbus::createClientPort(settings, false);
        QString sPort;
        switch (clientPort->type())
        {
        case Modbus::ASC:
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotAsciiTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotAsciiRx);
            sPort = QString("ASC:%1:%2:%3%4%5")
                        .arg(static_cast<ModbusSerialPort*>(clientPort->port())->portName(),
                             QString::number(static_cast<ModbusSerialPort*>(clientPort->port())->baudRate()),
                             QString::number(static_cast<ModbusSerialPort*>(clientPort->port())->dataBits()),
                             mbClientScanner::toShortParityStr(static_cast<ModbusSerialPort*>(clientPort->port())->parity()),
                             mbClientScanner::toShortStopBitsStr(static_cast<ModbusSerialPort*>(clientPort->port())->stopBits()));
            break;
        case Modbus::RTU:
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotBytesTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotBytesRx);
            sPort = QString("RTU:%1:%2:%3%4%5")
                        .arg(static_cast<ModbusSerialPort*>(clientPort->port())->portName(),
                             QString::number(static_cast<ModbusSerialPort*>(clientPort->port())->baudRate()),
                             QString::number(static_cast<ModbusSerialPort*>(clientPort->port())->dataBits()),
                             mbClientScanner::toShortParityStr(static_cast<ModbusSerialPort*>(clientPort->port())->parity()),
                             mbClientScanner::toShortStopBitsStr(static_cast<ModbusSerialPort*>(clientPort->port())->stopBits()));
            break;
        default:
            clientPort->connect(&ModbusClientPort::signalTx, this, &mbClientScannerThread::slotBytesTx);
            clientPort->connect(&ModbusClientPort::signalRx, this, &mbClientScannerThread::slotBytesRx);
            sPort = QString("TCP:%1:%2")
                        .arg(static_cast<ModbusTcpPort*>(clientPort->port())->host(),
                             QString::number(static_cast<ModbusTcpPort*>(clientPort->port())->port()));
            break;
        }
        mbClient::LogInfo(s.name, QString("Begin scanning '%1'").arg(sPort));
        for (uint16_t unit = m_unitStart; unit <= m_unitEnd; unit++)
        {
            if (!m_ctrlRun)
                break;
            QString sPortUnit = QString("%1,Unit=%2").arg(sPort, QString::number(unit));
            clientPort->setObjectName(sPortUnit.toLatin1().constData());
            m_scanner->setStatDevice(sPortUnit);
            Modbus::StatusCode status;
            bool deviceIsFound = false;
            Q_FOREACH (const mbClientScanner::FuncParams &f, m_request)
            {
                while(1)
                {
                    if (!m_ctrlRun)
                        break;
                    switch (f.func)
                    {
                    case MBF_READ_COILS:
                        status = clientPort->readCoils(static_cast<uint16_t>(unit), f.offset, f.count, dummy);
                        break;
                    case MBF_READ_DISCRETE_INPUTS:
                        status = clientPort->readDiscreteInputs(static_cast<uint16_t>(unit), f.offset, f.count, dummy);
                        break;
                    case MBF_READ_HOLDING_REGISTERS:
                        status = clientPort->readHoldingRegisters(static_cast<uint16_t>(unit), f.offset, f.count, reinterpret_cast<uint16_t*>(dummy));
                        break;
                    case MBF_READ_INPUT_REGISTERS:
                        status = clientPort->readInputRegisters(static_cast<uint16_t>(unit), f.offset, f.count, reinterpret_cast<uint16_t*>(dummy));
                        break;
                    case MBF_WRITE_SINGLE_COIL:
                        status = clientPort->writeSingleCoil(static_cast<uint16_t>(unit), f.offset, 0);
                        break;
                    case MBF_WRITE_SINGLE_REGISTER:
                        status = clientPort->writeSingleRegister(static_cast<uint16_t>(unit), f.offset, 0);
                        break;
                    case MBF_READ_EXCEPTION_STATUS:
                        status = clientPort->readExceptionStatus(static_cast<uint16_t>(unit), dummy);
                        break;
                    case MBF_WRITE_MULTIPLE_COILS:
                        status = clientPort->writeMultipleCoils(static_cast<uint16_t>(unit), f.offset, f.count, dummy);
                        break;
                    case MBF_WRITE_MULTIPLE_REGISTERS:
                        status = clientPort->writeMultipleRegisters(static_cast<uint16_t>(unit), f.offset, f.count, reinterpret_cast<uint16_t*>(dummy));
                        break;
                    case MBF_MASK_WRITE_REGISTER:
                        status = clientPort->maskWriteRegister(static_cast<uint16_t>(unit), f.offset, 0, 0);
                        break;
                    case MBF_READ_WRITE_MULTIPLE_REGISTERS:
                        status = clientPort->readWriteMultipleRegisters(static_cast<uint16_t>(unit), f.offset, f.count, reinterpret_cast<uint16_t*>(dummy), f.offset, f.count, reinterpret_cast<uint16_t*>(dummy));
                        break;
                    }

                    if (Modbus::StatusIsProcessing(status))
                        Modbus::msleep(1);
                    else
                        break;
                }
                if (Modbus::StatusIsGood(status))
                {
                    if (!deviceIsFound)
                    {
                        deviceIsFound = true;
                        Modbus::setSettingUnit(settings, unit);
                        m_scanner->deviceAdd(settings);
                    }
                }
                else if (Modbus::StatusIsBad(status))
                {
                    mbClient::LogInfo(s.name, QString("%1, Error (%2): %3").arg(sPortUnit, QString::number(status, 16), clientPort->lastErrorText()));
                }
                if (!m_ctrlRun)
                    break;
            }
            m_scanner->setStatPercent(++deviceCount*100/m_combinationCountAll);
        }
        clientPort->close();
        mbClient::LogInfo(s.name, QString("End scanning '%1'").arg(sPort));
        delete clientPort;
    }
    mbClient::LogInfo(s.name, QStringLiteral("Finish scanning"));
}

void mbClientScannerThread::slotBytesTx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTxRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(" Tx: ") + Modbus::bytesToString(buff, size).data());
    incStatTx();
}

void mbClientScannerThread::slotBytesRx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTxRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(" Rx: ") + Modbus::bytesToString(buff, size).data());
    incStatRx();
}

void mbClientScannerThread::slotAsciiTx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTxRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(" Tx: ") + Modbus::asciiToString(buff, size).data());
    incStatTx();
}

void mbClientScannerThread::slotAsciiRx(const Modbus::Char *source, const uint8_t *buff, uint16_t size)
{
    mbClient::LogTxRx(mbClientScanner::Strings::instance().name, source + QStringLiteral(" Rx: ") + Modbus::asciiToString(buff, size).data());
    incStatRx();
}

void mbClientScannerThread::incStatTx()
{
    m_statTx++;
    m_scanner->setStatCountTx(m_statTx);
}

void mbClientScannerThread::incStatRx()
{
    m_statRx++;
    m_scanner->setStatCountRx(m_statRx);
}
