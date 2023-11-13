/*
    Modbus

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
#ifndef MODBUS_H
#define MODBUS_H

/*
   Major part of modbuslib version
*/
#define MODBUSLIB_VERSION_MAJOR 0

/*
   Minor part of modbuslib version
*/
#define MODBUSLIB_VERSION_MINOR 1

/*
   Patch part of modbuslib version
*/
#define MODBUSLIB_VERSION_PATCH 0

/*
   MODBUSLIB_VERSION is (major << 16) + (minor << 8) + patch.
*/
#define MODBUSLIB_VERSION ((MODBUSLIB_VERSION_MAJOR<<16)|(MODBUSLIB_VERSION_MINOR<<8)|(MODBUSLIB_VERSION_PATCH))

/*
   MODBUSLIB_VERSION_STR "major.minor.patch"
*/

#define MODBUSLIB_VERSION_STR_HELPER(major,minor,patch) #major"."#minor"."#patch

#define MODBUSLIB_VERSION_STR_MAKE(major,minor,patch) MODBUSLIB_VERSION_STR_HELPER(major,minor,patch)

#define MODBUSLIB_VERSION_STR MODBUSLIB_VERSION_STR_MAKE(MODBUSLIB_VERSION_MAJOR,MODBUSLIB_VERSION_MINOR,MODBUSLIB_VERSION_PATCH)

#include <QMetaEnum>
#include <QHash>

#if defined(MODBUS_EXPORTS) && defined(Q_DECL_EXPORT)
#define MODBUS_EXPORT Q_DECL_EXPORT
#elif defined(Q_DECL_IMPORT)
#define MODBUS_EXPORT Q_DECL_IMPORT
#else
#define MODBUS_EXPORT
#endif

// --------------------------------------------------------------------------------------------------------
// -------------------------------------------- Helper macros ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

#define GET_BIT(bitBuff, bitNum) ((reinterpret_cast<const uint8_t*>(bitBuff)[(bitNum)/8] & (1<<((bitNum)%8))) != 0)

#define SET_BIT(bitBuff, bitNum, value)                                                                                     \
    if (value)                                                                                                              \
        reinterpret_cast<uint8_t*>(bitBuff)[(bitNum)/8] |= (1<<((bitNum)%8));                                               \
    else                                                                                                                    \
        reinterpret_cast<uint8_t*>(bitBuff)[(bitNum)/8] &= (~(1<<((bitNum)%8)));

#define GET_BITS(bitBuff, bitNum, bitCount, boolBuff)                                                                       \
    for (uint16_t __i__ = 0; __i__ < bitCount; __i__++)                                                                     \
        boolBuff[__i__] = (reinterpret_cast<const uint8_t*>(bitBuff)[((bitNum)+__i__)/8] & (1<<(((bitNum)+__i__)%8))) != 0;

#define SET_BITS(bitBuff, bitNum, bitCount, boolBuff)                                                                       \
    for (uint16_t __i__ = 0; __i__ < bitCount; __i__++)                                                                     \
        if (boolBuff[__i__])                                                                                                \
            reinterpret_cast<uint8_t*>(bitBuff)[((bitNum)+__i__)/8] |= (1<<(((bitNum)+__i__)%8));                           \
        else                                                                                                                \
            reinterpret_cast<uint8_t*>(bitBuff)[((bitNum)+__i__)/8] &= (~(1<<(((bitNum)+__i__)%8)));


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- Modbus function codes ----------------------------------------
// --------------------------------------------------------------------------------------------------------

#define MBF_READ_COILS                          1
#define MBF_READ_DISCRETE_INPUTS                2
#define MBF_READ_HOLDING_REGISTERS              3
#define MBF_READ_INPUT_REGISTERS                4
#define MBF_WRITE_SINGLE_COIL                   5
#define MBF_WRITE_SINGLE_REGISTER               6
#define MBF_READ_EXCEPTION_STATUS               7
#define MBF_DIAGNOSTICS                         8
#define MBF_GET_COMM_EVENT_COUNTER              11
#define MBF_GET_COMM_EVENT_LOG                  12
#define MBF_WRITE_MULTIPLE_COILS                15
#define MBF_WRITE_MULTIPLE_REGISTERS            16
#define MBF_REPORT_SERVER_ID                    17
#define MBF_READ_FILE_RECORD                    20
#define MBF_WRITE_FILE_RECORD                   21
#define MBF_MASK_WRITE_REGISTER                 22
#define MBF_READ_WRITE_MULTIPLE_REGISTERS       23
#define MBF_READ_FIFO_QUEUE                     24
#define MBF_ENCAPSULATED_INTERFACE_TRANSPORT    43
#define MBF_ILLEGAL_FUNCTION                    73 
#define MBF_EXCEPTION                           128    


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------- Modbus count constants ----------------------------------------
// --------------------------------------------------------------------------------------------------------

// 8 = count bits in byte (byte size in bits)
#define MB_BYTE_SZ_BITES 8

// 16 = count bits in 16 bit register (register size in bits) 
#define MB_REGE_SZ_BITES 16

// 2 = count bytes in 16 bit register (register size in bytes) 
#define MB_REGE_SZ_BYTES 2

// 255 - count_of_bytes in function readHoldingRegisters, readCoils etc
#define MB_MAX_BYTES 255

// 127 = 255(count_of_bytes in function readHoldingRegisters etc) / 2 (register size in bytes)
#define MB_MAX_REGISTERS 127

// 2040 = 255(count_of_bytes in function readCoils etc) * 8 (bits in byte)
#define MB_MAX_DISCRETS 2040

// 255 - count_of_bytes in function readHoldingRegisters, readCoils etc
#define MB_VALUE_BUFF_SZ MB_MAX_BYTES

// 1 byte(unit)+1 byte(function)+256 bytes(maximum data length e.g. readCoils etc)+2 bytes(CRC)
#define MB_RTU_IO_BUFF_SZ 260

// 1 byte(start symbol ':')+(1 byte(unit)+1 byte(function)+256 bytes(maximum data length e.g. readCoils etc)+1 byte(LRC) )*2+2 bytes(CR+LF)
#define MB_ASC_IO_BUFF_SZ 521

// 6 bytes(tcp-prefix)
#define MB_TCP_PREFIX_SZ 6

// 6 bytes(tcp-prefix)+1 byte(unit)+1 byte(function)+256 bytes(maximum data length e.g. readCoils etc)
#define MB_TCP_IO_BUFF_SZ 264

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- Modbus namespace -------------------------------------------
// --------------------------------------------------------------------------------------------------------                 

namespace Modbus {
Q_NAMESPACE_EXPORT(MODBUS_EXPORT)

class Port;
class ClientPort;
class ServerPort;
class Interface;

typedef QHash<QString, QVariant> Settings;

enum Constants
{
    VALID_MODBUS_ADDRESS_BEGIN = 1,
    VALID_MODBUS_ADDRESS_END = 247,
    STANDARD_TCP_PORT = 502
};

//=========== Modbus protocol types ===============

enum Type
{
    ASC,
    RTU,
    TCP
};
Q_ENUM_NS(Type)

enum MemoryType
{
    Memory_0x = 0,
    Memory_Coils = Memory_0x,
    Memory_1x = 1,
    Memory_InputStatus = Memory_1x,
    Memory_3x = 3,
    Memory_InputRegisters = Memory_3x,
    Memory_4x = 4,
    Memory_HoldingRegisters = Memory_4x,
};
Q_ENUM_NS(MemoryType)

//====== Modbus response values =======
enum StatusCode
{
    // ---- not finished operation response ------
    Status_Processing               = 0x80000000,
    Status_ProcessingBegin          = 0x80000001,
    // -------------- Good response --------------
    Status_Good                     = 0x00000000,
    // --------------- Bad response --------------
    Status_Bad                      = 0x01000000,
    // ------------ Uncertain response -----------
    Status_Uncertain                = 0x02000000,

    //------ Modbus standart errors begin -------
    // from 0 to 255
    Status_BadIllegalFunction       = Status_Bad | 1,
    Status_BadIllegalDataAddress    = Status_Bad | 2,
    Status_BadIllegalDataValue      = Status_Bad | 3,
    Status_BadSlaveDeviceFailure    = Status_Bad | 4,
    Status_BadAcknowledge           = Status_Bad | 5,
    Status_BadSlaveDeviceBusy       = Status_Bad | 6,
    Status_BadNegativeAcknowledge   = Status_Bad | 7,
    Status_BadMemoryParityError     = Status_Bad | 8,
    //------- Modbus standart errors end --------

    //------- Modbus common errors begin --------
    Status_BadEmptyResponse         = Status_Bad | 0x101,
    Status_BadNotCorrectRequest     ,
    Status_BadNotCorrectResponse    ,
    Status_BadWriteBufferOverflow   ,
    Status_BadReadBufferOverflow    ,
    Status_BadUnknownUnit           ,
    //-------- Modbus common errors end ---------

    //--_ Modbus serial specified errors begin --
    Status_BadSerialOpen            = Status_Bad | 0x201,
    Status_BadSerialWrite           ,
    Status_BadSerialRead            ,
    //---_ Modbus serial specified errors end ---

    //---- Modbus ASC specified errors begin ----
    Status_BadAscMissColon          = Status_Bad | 0x301,
    Status_BadAscMissCrLf           ,
    Status_BadAscChar               ,
    Status_BadLrc                   ,
    //---- Modbus ASC specified errors end ----

    //---- Modbus RTU specified errors begin ----
    Status_BadCrc                   = Status_Bad | 0x401,
    //----- Modbus RTU specified errors end -----

    //--_ Modbus TCP specified errors begin --
    Status_BadTcpCreate             = Status_Bad | 0x501,
    Status_BadTcpConnect            ,
    Status_BadTcpDisconnect         ,
    Status_BadTcpWrite              ,
    Status_BadTcpRead               ,
    //---_ Modbus TCP specified errors end ---
};

inline bool StatusIsGood(StatusCode status)         { return status == Status_Good; }
inline bool StatusIsBad(StatusCode status)          { return (status & Status_Bad)       != 0; }
inline bool StatusIsUncertain(StatusCode status)    { return (status & Status_Uncertain) != 0; }
inline bool StatusIsProcessing(StatusCode status)   { return (status & Status_Processing)!= 0; }

inline bool StatusIsStandardError(StatusCode status) { return (status & Status_Bad) && ((status & 0xFF00) == 0); }


// convert value to QString key for type
template <class EnumType>
inline QString enumKey(int value)
{
    const QMetaEnum me = QMetaEnum::fromType<EnumType>();
    return QString(me.valueToKey(value));
}

// convert key to value for enumeration by QString key
template <class EnumType>
inline EnumType enumValue(const QString& key, bool* ok = nullptr)
{
    const QMetaEnum me = QMetaEnum::fromType<EnumType>();
    return static_cast<EnumType>(me.keyToValue(key.toLatin1().constData(), ok));

}

// Convert value for enumeration by QVariant (int - value, string - key)
template <class EnumType>
inline EnumType enumValue(const QVariant& value, bool *ok)
{
    bool okInner;
    int v = value.toInt(&okInner);
    if (okInner)
    {
        const QMetaEnum me = QMetaEnum::fromType<EnumType>();
        if (me.valueToKey(v)) // check value exists
        {
            if (ok)
                *ok = true;
            return static_cast<EnumType>(v);
        }
        if (ok)
            *ok = false;
        return static_cast<EnumType>(-1);
    }
    return enumValue<EnumType>(value.toString(), ok);
}

template <class EnumType>
inline EnumType enumValue(const QVariant& value, EnumType defaultValue)
{
    bool okInner;
    EnumType v = enumValue<EnumType>(value, &okInner);
    if (okInner)
        return v;
    return defaultValue;
}

template <class EnumType>
inline EnumType enumValue(const QVariant& value)
{
    return enumValue<EnumType>(value, nullptr);
}

// get bit from byte array 'bitBuff'
inline bool getBit(const void *bitBuff, uint16_t bitNum) { return GET_BIT (bitBuff, bitNum); }

// get bit from byte array 'bitBuff' with size control
inline bool getBit(const void *bitBuff, uint16_t bitNum, uint16_t maxBitCount) { return (bitNum < maxBitCount) ? getBit(bitBuff, bitNum) : false; }

// set bit to byte array 'bitBuff'
inline void setBit(void *bitBuff, uint16_t bitNum, bool value) { SET_BIT (bitBuff, bitNum, value) }

// set bit to byte array 'bitBuff' with size control
inline void setBit(void *bitBuff, uint16_t bitNum, bool value, uint16_t maxBitCount) { if (bitNum < maxBitCount) setBit(bitBuff, bitNum, value); }

// get bits from byte array 'bitBuff' and put it to 'boolBuff'. Returns 'boolBuff'
inline bool *getBits(const void *bitBuff, uint16_t bitNum, uint16_t bitCount, bool *boolBuff) { GET_BITS(bitBuff, bitNum, bitCount, boolBuff) return boolBuff; }

// get bits from byte array 'bitBuff' and put it to 'boolBuff' with size control. Returns 'boolBuff'
inline bool *getBits(const void *bitBuff, uint16_t bitNum, uint16_t bitCount, bool *boolBuff, uint16_t maxBitCount) { if (bitNum < maxBitCount) getBits(bitBuff, bitNum, bitCount, boolBuff); return boolBuff; }

// get bit values from array 'boolBuff' and put it to byte array 'bitBuff'
inline void *setBits(void *bitBuff, uint16_t bitNum, uint16_t bitCount, const bool *boolBuff) { SET_BITS(bitBuff, bitNum, bitCount, boolBuff) return bitBuff; }

// get bit values from array 'boolBuff' and put it to byte array 'bitBuff' with size control. Returns 'bitBuff'
inline void *setBits(void *bitBuff, uint16_t bitNum, uint16_t bitCount, const bool *boolBuff, uint16_t maxBitCount) { if (bitNum < maxBitCount) setBits(bitBuff, bitNum, bitCount, boolBuff); return bitBuff; }

// CRC16 hash function (for Modbus RTU mode). Returns 16 bit unsigned integer
MODBUS_EXPORT uint16_t crc16(const uint8_t *byteArr, uint32_t count);

// LRC hash function (for Modbus ASCII mode). Returns 8 bit unsigned integer
MODBUS_EXPORT uint8_t lrc(const uint8_t *byteArr, uint32_t count);

// convert bytes array to ASCII array. Returns count bytes converted
MODBUS_EXPORT uint16_t bytesToAscii(uint8_t* bytesBuff, uint8_t* asciiBuff, uint16_t count);

// convert ASCII array to bytes array. Returns count bytes converted
MODBUS_EXPORT uint16_t asciiToBytes(uint8_t* asciiBuff, uint8_t* bytesBuff, uint16_t count);

// make string representation of bytes array and separate bytes by space
MODBUS_EXPORT QString bytesToString(const QByteArray& bytes);

// make string representation of ASCII array and separate bytes by space
MODBUS_EXPORT QString asciiToString(const QByteArray& bytes);

// 'ClientPort'-factory function
MODBUS_EXPORT ClientPort *createClientPort(const Settings &settings, QObject *parent = nullptr);

// 'ServerPort'-factory function
MODBUS_EXPORT ServerPort *createServerPort(const Settings &settings, Interface *device, QObject *parent = nullptr);

// --------------------------------------------------------------------------------------------------------
// ------------------------------------------- Modbus interface -------------------------------------------
// --------------------------------------------------------------------------------------------------------

class Interface
{
public:
    virtual StatusCode readCoils(uint8_t unit, uint16_t offset, uint16_t count, void *values) = 0;
    virtual StatusCode readDiscreteInputs(uint8_t unit, uint16_t offset, uint16_t count, void *values) = 0;
    virtual StatusCode readHoldingRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) = 0;
    virtual StatusCode readInputRegisters(uint8_t unit, uint16_t offset, uint16_t count, uint16_t *values) = 0;
    virtual StatusCode writeSingleCoil(uint8_t unit, uint16_t offset, bool value) = 0;
    virtual StatusCode writeSingleRegister(uint8_t unit, uint16_t offset, uint16_t value) = 0;
    virtual StatusCode readExceptionStatus(uint8_t unit, uint8_t *status) = 0;
    virtual StatusCode writeMultipleCoils(uint8_t unit, uint16_t offset, uint16_t count, const void *values) = 0;
    virtual StatusCode writeMultipleRegisters(uint8_t unit, uint16_t offset, uint16_t count, const uint16_t *values) = 0;
};

} //namespace Modbus

#endif // MODBUS_H
