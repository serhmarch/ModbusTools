#include "Modbus.h"

#include "ModbusAscPort.h"
#include "ModbusRtuPort.h"
#include "ModbusTcpPort.h"
#include "ModbusClientPort.h"
#include "ModbusTcpServer.h"
#include "ModbusServerResource.h"

namespace Modbus {

static inline Char hexDigit(uint8_t value) { return value < 10 ? '0' + value : 'A' + (value - 10); }

uint16_t crc16(const uint8_t *bytes, uint32_t count)
{
    uint16_t crc = 0xFFFF;
    for (uint32_t i = 0; i < count; i++)
    {
        crc ^= bytes[i];
        for (uint32_t j = 0; j < 8; j++)
        {
            uint16_t temp = crc & 0x0001;
            crc >>= 1;
            if (temp) crc ^= 0xA001;
        }
    }
    return crc;
}

uint8_t lrc(const uint8_t *bytes, uint32_t count)
{
    uint8_t lrc = 0x00;
    for (; count; count--)
        lrc += *bytes++;
    return static_cast<uint8_t>(-static_cast<int8_t>(lrc));
}

StatusCode readMemRegs(uint32_t offset, uint32_t count, void *values, const void *memBuff, uint32_t memRegCount)
{
    if (static_cast<uint32_t>(offset + count) > memRegCount)
        return Status_BadIllegalDataAddress;
    const uint16_t *mem = reinterpret_cast<const uint16_t*>(memBuff);
    memcpy(values, &mem[offset], count * MB_REGE_SZ_BYTES);
    return Status_Good;
}

StatusCode writeMemRegs(uint32_t offset, uint32_t count, const void *values, void *memBuff, uint32_t memRegCount)
{
    if (static_cast<uint32_t>(offset + count) > memRegCount)
        return Status_BadIllegalDataAddress;
    uint16_t *mem = reinterpret_cast<uint16_t*>(memBuff);
    memcpy(&mem[offset], values, count * MB_REGE_SZ_BYTES);
    return Status_Good;
}

StatusCode readMemBits(uint32_t offset, uint32_t count, void *values, const void *memBuff, uint32_t memBitCount)
{
    if (static_cast<uint32_t>(offset + count) > memBitCount)
        return Status_BadIllegalDataAddress;
    uint16_t byteOffset = offset/MB_BYTE_SZ_BITES;
    uint16_t bytes = count/MB_BYTE_SZ_BITES;
    uint16_t shift = offset%MB_BYTE_SZ_BITES;
    const uint8_t *mem = reinterpret_cast<const uint8_t*>(memBuff);
    if (shift)
    {
        for (uint16_t i = 0; i < bytes; i++)
        {
            uint16_t v = *(reinterpret_cast<const uint16_t*>(&mem[byteOffset+i])) >> shift;
            reinterpret_cast<uint8_t*>(values)[i] = static_cast<uint8_t>(v);
        }
        if (uint16_t resid = count%MB_BYTE_SZ_BITES)
        {
            int8_t mask = static_cast<int8_t>(0x80);
            mask = ~(mask>>(7-resid));
            if ((shift+resid) > MB_BYTE_SZ_BITES)
            {
                uint16_t v = ((*reinterpret_cast<const uint16_t*>(&mem[byteOffset+bytes])) >> shift) & mask;
                reinterpret_cast<uint8_t*>(values)[bytes] = static_cast<uint8_t>(v);
            }
            else
                reinterpret_cast<uint8_t*>(values)[bytes] = (mem[byteOffset+bytes]>>shift) & mask;
        }
    }
    else
    {
        memcpy(values, &mem[byteOffset], static_cast<size_t>(bytes));
        if (uint16_t resid = count%MB_BYTE_SZ_BITES)
        {
            int8_t mask = static_cast<int8_t>(0x80);
            mask = ~(mask>>(7-resid));
            reinterpret_cast<uint8_t*>(values)[bytes] = mem[byteOffset+bytes] & mask;
        }
    }
    return Status_Good;
}

StatusCode writeMemBits(uint32_t offset, uint32_t count, const void *values, void *memBuff, uint32_t memBitCount)
{
    if (static_cast<uint32_t>(offset + count) > memBitCount)
        return Status_BadIllegalDataAddress;
    uint16_t byteOffset = offset/MB_BYTE_SZ_BITES;
    uint16_t bytes = count/MB_BYTE_SZ_BITES;
    uint16_t shift = offset%MB_BYTE_SZ_BITES;
    uint8_t *mem = reinterpret_cast<uint8_t*>(memBuff);
    if (shift)
    {
        for (uint16_t i = 0; i < bytes; i++)
        {
            uint16_t mask = static_cast<uint16_t>(0x00FF) << shift;
            uint16_t v = static_cast<uint16_t>(reinterpret_cast<const uint8_t*>(values)[i]) << shift;
            *reinterpret_cast<uint16_t*>(&mem[byteOffset+i]) &= ~mask; // zero undermask values
            *reinterpret_cast<uint16_t*>(&mem[byteOffset+i]) |= v; // set bit values
        }
        if (uint16_t resid = count%MB_BYTE_SZ_BITES)
        {
            if ((shift+resid) > MB_BYTE_SZ_BITES)
            {
                int16_t m = static_cast<int16_t>(0x8000); // using signed mask for right shift filled by '1'-bit
                m = m>>(resid-1);
                uint16_t mask = *reinterpret_cast<uint16_t*>(&m);
                mask = mask >> (MB_REGE_SZ_BITES-resid-shift);
                uint16_t v = (static_cast<uint16_t>(reinterpret_cast<const uint8_t*>(values)[bytes]) << shift) & mask;
                *reinterpret_cast<uint16_t*>(&mem[byteOffset+bytes]) &= ~mask; // zero undermask values
                *reinterpret_cast<uint16_t*>(&mem[byteOffset+bytes]) |= v;
            }
            else
            {
                int8_t m = static_cast<int8_t>(0x80); // using signed mask for right shift filled by '1'-bit
                m = m>>(resid-1);
                uint8_t mask = *reinterpret_cast<uint8_t*>(&m);
                mask = mask >> (MB_BYTE_SZ_BITES-resid-shift);
                uint8_t v = (reinterpret_cast<const uint8_t*>(values)[bytes] << shift) & mask;
                mem[byteOffset+bytes] &= ~mask; // zero undermask values
                mem[byteOffset+bytes] |= v;
            }
        }
    }
    else
    {
        memcpy(&mem[byteOffset], values, static_cast<size_t>(bytes));
        if (uint16_t resid = count%MB_BYTE_SZ_BITES)
        {
            int8_t mask = static_cast<int8_t>(0x80);
            mask = mask>>(7-resid);
            mem[byteOffset+bytes] &= mask;
            mask = ~mask;
            mem[byteOffset+bytes] |= (reinterpret_cast<const uint8_t*>(values)[bytes] & mask);
        }
    }
    return Status_Good;
}


uint32_t bytesToAscii(const uint8_t *bytesBuff, uint8_t* asciiBuff, uint32_t count)
{
    uint32_t i, j, qAscii = 0;
    uint8_t tmp;

    for (i = 0; i < count; i++)
    {
        for (j = 0; j < 2; j++)
        {
            tmp = (bytesBuff[i] >> ((1 - j) * 4)) & 0x0F;
            asciiBuff[i * 2 + j] = ((tmp < 10) ? (0x30 | tmp) : (55 + tmp));
            qAscii++;
        }
    }
    return qAscii;
}

uint32_t asciiToBytes(const uint8_t *asciiBuff, uint8_t* bytesBuff, uint32_t count)
{
    uint32_t i, j, qBytes = 0;
    uint8_t tmp;
    for (i = 0; i < count; i++)
    {
        j = i & 1;
        if (!j)
        {
            bytesBuff[i / 2] = 0;
            qBytes++;
        }
        tmp = asciiBuff[i];
        if ((tmp >= '0') && (tmp <= '9'))
            tmp &= 0x0F;
        else if ((tmp >= 'A') && (tmp <= 'F'))
            tmp -= 55;
        else
            return 0;
        bytesBuff[i / 2] |= tmp << (4 * (1 - j));
    }
    return qBytes;
}

Char *sbytes(const uint8_t* buff, uint32_t count, Char *str, uint32_t strmaxlen)
{
    String s = bytesToString(buff, count);
    strncpy(str, s.data(), strmaxlen-1);
    str[strmaxlen-1] = '\0';
    return str;
}

Char *sascii(const uint8_t* buff, uint32_t count, Char *str, uint32_t strmaxlen)
{
    String s = asciiToString(buff, count);
    strncpy(str, s.data(), strmaxlen-1);
    str[strmaxlen-1] = '\0';
    return str;
}

String bytesToString(const uint8_t* buff, uint32_t count)
{
    String str;
    for (uint32_t i = 0; i < count; ++i)
    {
        uint8_t num = buff[i];
        str += hexDigit(num >> 4);
        str += hexDigit(num & 0xF);
        str += CharLiteral(' ');
    }
    return str;
}

String asciiToString(const uint8_t* buff, uint32_t count)
{
    String str;
    bool lastHex = false;
    for (uint32_t i = 0; i < count; ++i)
    {
        uint8_t c = buff[i];
        switch (c)
        {
        case '\r':
            str += ((str.size() && (str.back() == CharLiteral(' '))) ? StringLiteral("CR ") : StringLiteral(" CR "));
            lastHex = false;
            break;
        case '\n':
            str += ((str.size() && (str.back() == CharLiteral(' '))) ? StringLiteral("LF ") : StringLiteral(" LF "));
            lastHex = false;
            break;
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            str += c;
            if (lastHex)
            {
                str += " ";
                lastHex = false;
            }
            else
                lastHex = true;
            break;
        default:
            str += c;
            str += " ";
            lastHex = false;
            break;
        }
    }
    return str;
}

ModbusPort *createPort(ProtocolType type, const void *settings, bool blocking)
{
    ModbusPort *port = nullptr;
    switch (type)
    {
    case RTU:
    {
        ModbusRtuPort *rtu = new ModbusRtuPort(blocking);
        const SerialSettings *s = reinterpret_cast<const SerialSettings*>(settings);
        rtu->setPortName        (s->portName        );
        rtu->setBaudRate        (s->baudRate        );
        rtu->setDataBits        (s->dataBits        );
        rtu->setParity          (s->parity          );
        rtu->setStopBits        (s->stopBits        );
        rtu->setFlowControl     (s->flowControl     );
        rtu->setTimeoutFirstByte(s->timeoutFirstByte);
        rtu->setTimeoutInterByte(s->timeoutInterByte);
        port = rtu;
    }
        break;
    case ASC:
    {
        ModbusAscPort *asc = new ModbusAscPort(blocking);
        const SerialSettings *s = reinterpret_cast<const SerialSettings*>(settings);
        asc->setPortName        (s->portName        );
        asc->setBaudRate        (s->baudRate        );
        asc->setDataBits        (s->dataBits        );
        asc->setParity          (s->parity          );
        asc->setStopBits        (s->stopBits        );
        asc->setFlowControl     (s->flowControl     );
        asc->setTimeoutFirstByte(s->timeoutFirstByte);
        asc->setTimeoutInterByte(s->timeoutInterByte);
        port = asc;
    }
        break;
    case TCP:
    {
        ModbusTcpPort *tcp = new ModbusTcpPort(blocking);
        const TcpSettings *s = reinterpret_cast<const TcpSettings*>(settings);
        tcp->setHost   (s->host   );
        tcp->setPort   (s->port   );
        tcp->setTimeout(s->timeout);
        port = tcp;
    }
        break;
    }
    return port;
}

ModbusClientPort *createClientPort(ProtocolType type, const void *settings, bool blocking)
{
    ModbusPort *port = createPort(type, settings, blocking);
    ModbusClientPort *clientPort = new ModbusClientPort(port);
    return clientPort;
}

ModbusServerPort *createServerPort(ModbusInterface *device, ProtocolType type, const void *settings, bool blocking)
{
    ModbusServerPort *serv = nullptr;
    switch (type)
    {
    case RTU:
    case ASC:
    {
        ModbusPort *port = createPort(type, settings, blocking);
        serv = new ModbusServerResource(port, device);
    }
        break;
    case TCP:
    {
        ModbusTcpServer *tcp = new ModbusTcpServer(device);
        const TcpSettings *s = reinterpret_cast<const TcpSettings*>(settings);
        tcp->setPort   (s->port   );
        tcp->setTimeout(s->timeout);
        serv = tcp;
    }
        break;
    }
    return serv;
}

List<int32_t> availableBaudRate()
{
    List<int32_t> ls;
    ls.push_back(1200);
    ls.push_back(2400);
    ls.push_back(4800);
    ls.push_back(9600);
    ls.push_back(19200);
    ls.push_back(38400);
    ls.push_back(57600);
    ls.push_back(115200);
    return ls;
}

List<int8_t> availableDataBits()
{
    List<int8_t> ls;
    ls.push_back(5);
    ls.push_back(6);
    ls.push_back(7);
    ls.push_back(8);
    return ls;
}

List<Parity> availableParity()
{
    List<Parity> ls;
    ls.push_back(NoParity   );
    ls.push_back(EvenParity );
    ls.push_back(OddParity  );
    ls.push_back(SpaceParity);
    ls.push_back(MarkParity );
    return ls;
}

List<StopBits> availableStopBits()
{
    List<StopBits> ls;
    ls.push_back(OneStop       );
    ls.push_back(OneAndHalfStop);
    ls.push_back(TwoStop       );
    return ls;
}

List<FlowControl> availableFlowControl()
{
    List<FlowControl> ls;
    ls.push_back(NoFlowControl  );
    ls.push_back(HardwareControl);
    ls.push_back(SoftwareControl);
    return ls;
}

} //namespace Modbus

Modbus::StatusCode ModbusInterface::readCoils(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, void */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::readDiscreteInputs(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, void */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::readHoldingRegisters(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, uint16_t */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::readInputRegisters(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, uint16_t */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::writeSingleCoil(uint8_t /*unit*/, uint16_t /*offset*/, bool /*value*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::writeSingleRegister(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*value*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::readExceptionStatus(uint8_t /*unit*/, uint8_t */*status*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::writeMultipleCoils(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, const void */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}

Modbus::StatusCode ModbusInterface::writeMultipleRegisters(uint8_t /*unit*/, uint16_t /*offset*/, uint16_t /*count*/, const uint16_t */*values*/)
{
    return Modbus::Status_BadIllegalFunction;
}
