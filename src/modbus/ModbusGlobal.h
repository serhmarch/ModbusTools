/*!
 * \file   ModbusGlobal.h
 * \brief  Contains general definitions of the Modbus libarary (for C++ and "pure" C).
 *
 * \author serhmarch
 * \date   May 2024
 */
#ifndef MODBUSGLOBAL_H
#define MODBUSGLOBAL_H

#include <stdint.h>
#include <string.h>

#ifdef QT_CORE_LIB
#include <qobjectdefs.h>
#endif

#include "ModbusPlatform.h"
#include "Modbus_config.h"

/// \brief ModbusLib version value that defines as MODBUSLIB_VERSION = (major << 16) + (minor << 8) + patch.
#define MODBUSLIB_VERSION ((MODBUSLIB_VERSION_MAJOR<<16)|(MODBUSLIB_VERSION_MINOR<<8)|(MODBUSLIB_VERSION_PATCH))

/// \cond
#define MODBUSLIB_VERSION_STR_HELPER(major,minor,patch) #major"."#minor"."#patch

#define MODBUSLIB_VERSION_STR_MAKE(major,minor,patch) MODBUSLIB_VERSION_STR_HELPER(major,minor,patch)
/// \endcond

/// \brief ModbusLib version value that defines as MODBUSLIB_VERSION_STR "major.minor.patch"
#define MODBUSLIB_VERSION_STR MODBUSLIB_VERSION_STR_MAKE(MODBUSLIB_VERSION_MAJOR,MODBUSLIB_VERSION_MINOR,MODBUSLIB_VERSION_PATCH)

/// \brief MODBUS_EXPORT defines macro for import/export functions and classes.
#if defined(MODBUS_EXPORTS) && defined(MB_DECL_EXPORT)
#define MODBUS_EXPORT MB_DECL_EXPORT
#elif defined(MB_DECL_IMPORT)
#define MODBUS_EXPORT MB_DECL_IMPORT
#else
#define MODBUS_EXPORT
#endif

/// \brief Macro for creating string literal, must be used like: `StringLiteral("Some string")`
#define StringLiteral(cstr) cstr

/// \brief Macro for creating char literal, must be used like: `CharLiteral('A')`
#define CharLiteral(cchar) cchar

// --------------------------------------------------------------------------------------------------------
// -------------------------------------------- Helper macros ---------------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \brief Macro for get bit with number `bitNum` from array `bitBuff`.
#define GET_BIT(bitBuff, bitNum) ((((const uint8_t*)(bitBuff))[(bitNum)/8] & (1<<((bitNum)%8))) != 0)

/// \brief Macro for set bit `value` with number `bitNum` to array `bitBuff`.
#define SET_BIT(bitBuff, bitNum, value)                                                                                     \
    if (value)                                                                                                              \
        ((uint8_t*)(bitBuff))[(bitNum)/8] |= (1<<((bitNum)%8));                                                             \
    else                                                                                                                    \
        ((uint8_t*)(bitBuff))[(bitNum)/8] &= (~(1<<((bitNum)%8)));

/// \brief Macro for get bits begins with number `bitNum` with `count` from input bit array `bitBuff` to output bool array `boolBuff`.
#define GET_BITS(bitBuff, bitNum, bitCount, boolBuff)                                                                       \
    for (uint16_t __i__ = 0; __i__ < bitCount; __i__++)                                                                     \
        boolBuff[__i__] = (((const uint8_t*)(bitBuff))[((bitNum)+__i__)/8] & (1<<(((bitNum)+__i__)%8))) != 0;

/// \brief Macro for set bits begins with number `bitNum` with `count` from input bool array `boolBuff`to output bit array `bitBuff`.
#define SET_BITS(bitBuff, bitNum, bitCount, boolBuff)                                                                       \
    for (uint16_t __i__ = 0; __i__ < bitCount; __i__++)                                                                     \
        if (boolBuff[__i__])                                                                                                \
            ((uint8_t*)(bitBuff))[((bitNum)+__i__)/8] |= (1<<(((bitNum)+__i__)%8));                                         \
        else                                                                                                                \
            ((uint8_t*)(bitBuff))[((bitNum)+__i__)/8] &= (~(1<<(((bitNum)+__i__)%8)));


// --------------------------------------------------------------------------------------------------------
// ----------------------------------------- Modbus function codes ----------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \name Modbus Functions
/// Modbus Function's codes.
///@{ 
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
///@}


// --------------------------------------------------------------------------------------------------------
// ---------------------------------------- Modbus count constants ----------------------------------------
// --------------------------------------------------------------------------------------------------------

/// \brief 8 = count bits in byte (byte size in bits)
#define MB_BYTE_SZ_BITES 8

/// \brief 16 = count bits in 16 bit register (register size in bits) 
#define MB_REGE_SZ_BITES 16

/// \brief 2 = count bytes in 16 bit register (register size in bytes) 
#define MB_REGE_SZ_BYTES 2

/// \brief 255 - count_of_bytes in function readHoldingRegisters, readCoils etc
#define MB_MAX_BYTES 255

/// \brief 127 = 255(count_of_bytes in function readHoldingRegisters etc) / 2 (register size in bytes)
#define MB_MAX_REGISTERS 127

/// \brief 2040 = 255(count_of_bytes in function readCoils etc) * 8 (bits in byte)
#define MB_MAX_DISCRETS 2040

/// \brief Same as `MB_MAX_BYTES`
#define MB_VALUE_BUFF_SZ 255

/// \brief Maximum func data size: WriteMultipleCoils
/// 261 = 1 byte(function) + 2 bytes (starting offset) + 2 bytes (count) + 1 bytes (byte count) + 255 bytes(maximum data length)

/// \brief 1 byte(unit) + 261 (max func data size: WriteMultipleCoils) + 2 bytes(CRC)
#define MB_RTU_IO_BUFF_SZ 264

/// \brief 1 byte(start symbol ':')+(( 1 byte(unit) + 261 (max func data size: WriteMultipleCoils)) + 1 byte(LRC) ))*2+2 bytes(CR+LF)
#define MB_ASC_IO_BUFF_SZ 529

/// \brief 6 bytes(tcp-prefix)+1 byte(unit)+261 (max func data size: WriteMultipleCoils)
#define MB_TCP_IO_BUFF_SZ 268

#ifdef __cplusplus

namespace Modbus {

#ifdef QT_CORE_LIB
Q_NAMESPACE
#endif

#endif // __cplusplus

/// \brief Handle type for native OS values
typedef void* Handle;

/// \brief Type for Modbus character
typedef char Char;

/// \brief Type for Modbus timer
typedef uint32_t Timer;

/// \brief Define list of contants of Modbus protocol.
enum Constants
{
    VALID_MODBUS_ADDRESS_BEGIN = 1  , ///< Start of Modbus device address range according to specification
    VALID_MODBUS_ADDRESS_END   = 247, ///< End of the Modbus protocol device address range according to the specification
    STANDARD_TCP_PORT          = 502  ///< Standard TCP port of the Modbus protocol
};

//=========== Modbus protocol types ===============

/// \brief Defines type of memory used in Modbus protocol.
typedef enum _MemoryType
{
    Memory_Unknown = 0xFFFF,                 ///< Invalid memory type
    Memory_0x = 0,                           ///< Memory allocated for coils/discrete outputs
    Memory_Coils = Memory_0x,                ///< Same as `Memory_0x`.
    Memory_1x = 1,                           ///< Memory allocated for discrete inputs
    Memory_DiscreteInputs = Memory_1x,       ///< Same as `Memory_1x`.
    Memory_3x = 3,                           ///< Memory allocated for analog inputs
    Memory_InputRegisters = Memory_3x,       ///< Same as `Memory_3x`.
    Memory_4x = 4,                           ///< Memory allocated for holding registers/analog outputs
    Memory_HoldingRegisters = Memory_4x,     ///< Same as `Memory_4x`.
} MemoryType;

/// \brief Defines status of executed Modbus functions.
#ifdef __cplusplus // Note: for Qt/moc support
enum StatusCode
#else
typedef enum _StatusCode
#endif
{
    Status_Processing               = 0x80000000, ///< The operation is not complete. Further operation is required
    Status_Good                     = 0x00000000, ///< Successful result
    Status_Bad                      = 0x01000000, ///< Error. General
    Status_Uncertain                = 0x02000000, ///< The status is undefined
                                                          
    //------ Modbus standart errors begin -------         
    // from 0 to 255                                      
    Status_BadIllegalFunction                    = Status_Bad | 0x01, ///< Standard error. The feature is not supported
    Status_BadIllegalDataAddress                 = Status_Bad | 0x02, ///< Standard error. Invalid data address
    Status_BadIllegalDataValue                   = Status_Bad | 0x03, ///< Standard error. Invalid data value
    Status_BadServerDeviceFailure                = Status_Bad | 0x04, ///< Standard error. Failure during a specified operation
    Status_BadAcknowledge                        = Status_Bad | 0x05, ///< Standard error. The server has accepted the request and is processing it, but it will take a long time
    Status_BadServerDeviceBusy                   = Status_Bad | 0x06, ///< Standard error. The server is busy processing a long command. The request must be repeated later
    Status_BadNegativeAcknowledge                = Status_Bad | 0x07, ///< Standard error. The programming function cannot be performed
    Status_BadMemoryParityError                  = Status_Bad | 0x08, ///< Standard error. The server attempted to read a record file but detected a parity error in memory
    Status_BadGatewayPathUnavailable             = Status_Bad | 0x0A, ///< Standard error. Indicates that the gateway was unable to allocate an internal communication path from the input port o the output port for processing the request. Usually means that the gateway is misconfigured or overloaded
    Status_BadGatewayTargetDeviceFailedToRespond = Status_Bad | 0x0B, ///< Standard error. Indicates that no response was obtained from the target device. Usually means that the device is not present on the network
    //------- Modbus standart errors end --------
                                                          
    //------- Modbus common errors begin --------         
    Status_BadEmptyResponse         = Status_Bad | 0x101, ///< Error. Empty request/response body
    Status_BadNotCorrectRequest     ,                     ///< Error. Invalid request
    Status_BadNotCorrectResponse    ,                     ///< Error. Invalid response
    Status_BadWriteBufferOverflow   ,                     ///< Error. Write buffer overflow
    Status_BadReadBufferOverflow    ,                     ///< Error. Request receive buffer overflow

    //-------- Modbus common errors end ---------

    //--_ Modbus serial specified errors begin --         
    Status_BadSerialOpen            = Status_Bad | 0x201, ///< Error. Serial port cannot be opened
    Status_BadSerialWrite           ,                     ///< Error. Cannot send a parcel to the serial port
    Status_BadSerialRead            ,                     ///< Error. Reading the serial port (timeout)
    //---_ Modbus serial specified errors end ---         
                                                          
    //---- Modbus ASC specified errors begin ----         
    Status_BadAscMissColon          = Status_Bad | 0x301, ///< Error (ASC). Missing packet start character ':'
    Status_BadAscMissCrLf           ,                     ///< Error (ASC). '\\r\\n' end of packet character missing
    Status_BadAscChar               ,                     ///< Error (ASC). Invalid ASCII character
    Status_BadLrc                   ,                     ///< Error (ASC). Invalid checksum
    //---- Modbus ASC specified errors end ----           
                                                          
    //---- Modbus RTU specified errors begin ----         
    Status_BadCrc                   = Status_Bad | 0x401, ///< Error (RTU). Wrong checksum
    //----- Modbus RTU specified errors end -----         
                                                          
    //--_ Modbus TCP specified errors begin --            
    Status_BadTcpCreate             = Status_Bad | 0x501, ///< Error. Unable to create a TCP socket
    Status_BadTcpConnect,                                 ///< Error. Unable to create a TCP connection
    Status_BadTcpWrite,                                   ///< Error. Unable to send a TCP packet
    Status_BadTcpRead,                                    ///< Error. Unable to receive a TCP packet
    Status_BadTcpBind,                                    ///< Error. Unable to bind a TCP socket (server side)
    Status_BadTcpListen,                                  ///< Error. Unable to listen a TCP socket (server side)
    Status_BadTcpAccept,                                  ///< Error. Unable accept bind a TCP socket (server side)
    Status_BadTcpDisconnect,                              ///< Error. Bad disconnection result
    //---_ Modbus TCP specified errors end ---
}
#ifdef __cplusplus
;
#else
StatusCode;
#endif

/// \brief Defines type of Modbus protocol.
#ifdef __cplusplus // Note: for Qt/moc support
enum ProtocolType
#else
typedef enum _ProtocolType
#endif
{
    ASC, ///< ASCII version of Modbus communication protocol.
    RTU, ///< RTU version of Modbus communication protocol.
    TCP  ///< TCP version of Modbus communication protocol.
}
#ifdef __cplusplus
;
#else
ProtocolType;
#endif


/// \brief Defines Parity for serial port
#ifdef __cplusplus // Note: for Qt/moc support
enum Parity
#else
typedef enum _Parity
#endif
{
    NoParity   ,    ///< No parity bit it sent. This is the most common parity setting.
    EvenParity ,  ///< The number of 1 bits in each character, including the parity bit, is always even.
    OddParity  ,   ///< The number of 1 bits in each character, including the parity bit, is always odd. It ensures that at least one state transition occurs in each character.
    SpaceParity, ///< Space parity. The parity bit is sent in the space signal condition. It does not provide error detection information.
    MarkParity   ///< Mark parity. The parity bit is always set to the mark signal condition (logical 1). It does not provide error detection information.
}
#ifdef __cplusplus
;
#else
Parity;
#endif


/// \brief Defines Stop Bits for serial port
#ifdef __cplusplus // Note: for Qt/moc support
enum StopBits
#else
typedef enum _StopBits
#endif
{
    OneStop       , ///< 1 stop bit.
    OneAndHalfStop, ///< 1.5 stop bit.
    TwoStop         ///< 2 stop bits.
}
#ifdef __cplusplus
;
#else
StopBits;
#endif

/// \brief FlowControl Parity for serial port
#ifdef __cplusplus // Note: for Qt/moc support
enum FlowControl
#else
typedef enum _FlowControl
#endif
{
    NoFlowControl  , ///< No flow control.
    HardwareControl, ///< Hardware flow control (RTS/CTS).
    SoftwareControl  ///< Software flow control (XON/XOFF).
}
#ifdef __cplusplus
;
#else
FlowControl;
#endif

#ifdef QT_CORE_LIB
Q_ENUM_NS(StatusCode)
Q_ENUM_NS(ProtocolType)
Q_ENUM_NS(Parity)
Q_ENUM_NS(StopBits)
Q_ENUM_NS(FlowControl)
#endif

/// \brief Struct to define settings for Serial Port
typedef struct 
{
    const Char *portName        ; ///< Value for the serial port name
    int32_t     baudRate        ; ///< Value for the serial port's baud rate
    int8_t      dataBits        ; ///< Value for the serial port's data bits
    Parity      parity          ; ///< Value for the serial port's patiry
    StopBits    stopBits        ; ///< Value for the serial port's stop bits
    FlowControl flowControl     ; ///< Value for the serial port's flow control
    uint32_t    timeoutFirstByte; ///< Value for the serial port's timeout waiting first byte of packet
    uint32_t    timeoutInterByte; ///< Value for the serial port's timeout waiting next byte of packet
} SerialSettings;

/// \brief Struct to define settings for TCP connection
typedef struct 
{
    const Char *host   ; ///< Value for the IP address or DNS name of the remote device
    uint16_t    port   ; ///< Value for the TCP port number of the remote device
    uint16_t    timeout; ///< Value for connection timeout (milliseconds)
} TcpSettings;

#ifdef __cplusplus
extern "C" {
#endif

/// \details Returns a general indication that the result of the operation is incomplete.
inline bool StatusIsProcessing(StatusCode status) { return status == Status_Processing; }

/// \details Returns a general indication that the operation result is successful.
inline bool StatusIsGood(StatusCode status) { return status == Status_Good; }

/// \details Returns a general indication that the operation result is unsuccessful.
inline bool StatusIsBad(StatusCode status) { return (status & Status_Bad) != 0; }

/// \details Returns a general sign that the result of the operation is undefined.
inline bool StatusIsUncertain(StatusCode status) { return (status & Status_Uncertain) != 0; }

/// \details Returns a general sign that the result is standard error.
inline bool StatusIsStandardError(StatusCode status) { return (status & Status_Bad) && ((status & 0xFF00) == 0); }

/// \details Returns the value of the bit with number `bitNum' from the bit array `bitBuff'.
inline bool getBit(const void *bitBuff, uint16_t bitNum) { return GET_BIT (bitBuff, bitNum); }

/// \details Returns the value of the bit with the number `bitNum' from the bit array `bitBuff', if the bit number is greater than or equal to `maxBitCount', then `false' is returned.
inline bool getBitS(const void *bitBuff, uint16_t bitNum, uint16_t maxBitCount) { return (bitNum < maxBitCount) ? getBit(bitBuff, bitNum) : false; }

/// \details Sets the value of the bit with the number `bitNum' to the bit array `bitBuff'.
inline void setBit(void *bitBuff, uint16_t bitNum, bool value) { SET_BIT (bitBuff, bitNum, value) }

/// \details Sets the value of the bit with the number `bitNum' to the bit array `bitBuff', controlling the size of the array `maxBitCount' in bits.
inline void setBitS(void *bitBuff, uint16_t bitNum, bool value, uint16_t maxBitCount) { if (bitNum < maxBitCount) setBit(bitBuff, bitNum, value); }

/// \details Gets the values of bits with number `bitNum` and count `bitCount` from the bit array `bitBuff` and stores their values in the boolean array `boolBuff`,
/// where the value of each bit is stored as a separate `bool` value.
/// \return A pointer to the `boolBuff` array.
inline bool *getBits(const void *bitBuff, uint16_t bitNum, uint16_t bitCount, bool *boolBuff) { GET_BITS(bitBuff, bitNum, bitCount, boolBuff) return boolBuff; }

/// \details Similar to the `Modbus::getBits(const void*,uint16_t,uint16_t,bool*)` function, but it is controlled that the size does not exceed the maximum number of bits `maxBitCount`.
/// \return A pointer to the `boolBuff` array.
inline bool *getBitsS(const void *bitBuff, uint16_t bitNum, uint16_t bitCount, bool *boolBuff, uint16_t maxBitCount) { if ((bitNum+bitCount) <= maxBitCount) getBits(bitBuff, bitNum, bitCount, boolBuff); return boolBuff; }

/// \details Sets the values of the bits in the `bitBuff` array starting with the number `bitNum` and the count `bitCount` from the `boolBuff` array,
/// where the value of each bit is stored as a separate `bool` value.
/// \return A pointer to the `bitBuff` array.
inline void *setBits(void *bitBuff, uint16_t bitNum, uint16_t bitCount, const bool *boolBuff) { SET_BITS(bitBuff, bitNum, bitCount, boolBuff) return bitBuff; }

/// \details Similar to the `Modbus::setBits(void*,uint16_t,uint16_t,const bool*)` function, but it is controlled that the size does not exceed the maximum number of bits `maxBitCount`.
/// \return A pointer to the `bitBuff` array.
inline void *setBitsS(void *bitBuff, uint16_t bitNum, uint16_t bitCount, const bool *boolBuff, uint16_t maxBitCount) { if ((bitNum + bitCount) <= maxBitCount) setBits(bitBuff, bitNum, bitCount, boolBuff); return bitBuff; }

/// \details CRC16 checksum hash function (for Modbus RTU).
/// \returns Returns a 16-bit unsigned integer value of the checksum
MODBUS_EXPORT uint16_t crc16(const uint8_t *byteArr, uint32_t count);

/// \details LRC checksum hash function (for Modbus ASCII).
/// \returns Returns an 8-bit unsigned integer value of the checksum
MODBUS_EXPORT uint8_t lrc(const uint8_t *byteArr, uint32_t count);

/// \details Function for copy (read) values from memory input `memBuff` and put it to the output buffer `values` for 16 bit registers:
/// \param[in]  offset      Memory offset to read from `memBuff` in 16-bit registers size.
/// \param[in]  count       Count of 16-bit registers to read from memory `memBuff`.
/// \param[out] values      Output buffer to store data.
/// \param[in]  memBuff     Pointer to the memory which holds data.
/// \param[in]  memRegCount Size of memory buffer `memBuff` in 16-bit registers.
MODBUS_EXPORT StatusCode readMemRegs(uint32_t offset, uint32_t count, void *values, const void *memBuff, uint32_t memRegCount);

/// \details Function for copy (write) values from input buffer `values` to memory `memBuff` for 16 bit registers:
/// \param[in]  offset      Memory offset to write to `memBuff` in 16-bit registers size.
/// \param[in]  count       Count of 16-bit registers to write into memory `memBuff`.
/// \param[out] values      Input buffer that holds data to write.
/// \param[in]  memBuff     Pointer to the memory buffer.
/// \param[in]  memRegCount Size of memory buffer `memBuff` in 16-bit registers.
MODBUS_EXPORT StatusCode writeMemRegs(uint32_t offset, uint32_t count, const void *values, void *memBuff, uint32_t memRegCount);

/// \details Function for copy (read) values from memory input `memBuff` and put it to the output buffer `values` for discretes (bits):
/// \param[in]  offset      Memory offset to read from `memBuff` in bit size.
/// \param[in]  count       Count of bits to read from memory `memBuff`.
/// \param[out] values      Output buffer to store data.
/// \param[in]  memBuff     Pointer to the memory which holds data.
/// \param[in]  memBitCount Size of memory buffer `memBuff` in bits.
MODBUS_EXPORT StatusCode readMemBits(uint32_t offset, uint32_t count, void *values, const void *memBuff, uint32_t memBitCount);

/// \details Function for copy (write) values from input buffer `values` to memory `memBuff` for discretes (bits):
/// \param[in]  offset      Memory offset to write to `memBuff` in bit size.
/// \param[in]  count       Count of bits to write into memory `memBuff`.
/// \param[out] values      Input buffer that holds data to write.
/// \param[in]  memBuff     Pointer to the memory buffer.
/// \param[in]  memBitCount Size of memory buffer `memBuff` in bits.
MODBUS_EXPORT StatusCode writeMemBits(uint32_t offset, uint32_t count, const void *values, void *memBuff, uint32_t memBitCount);

/// \details Function converts byte array \c bytesBuff to ASCII repr of byte array.
/// Every byte of \c bytesBuff are repr as two bytes in \c asciiBuff,
/// where most signified tetrabits represented as leading byte in hex digit in ASCII encoding (upper) and
/// less signified tetrabits represented as tailing byte in hex digit in ASCII encoding (upper).
/// \c count is count bytes of \c bytesBuff.
/// \note Output array \c asciiBuff must be at least twice bigger than input array \c bytesBuff.
/// \returns Returns size of \c asciiBuff in bytes which calc as \c {output = count * 2}
MODBUS_EXPORT uint32_t bytesToAscii(const uint8_t* bytesBuff, uint8_t* asciiBuff, uint32_t count);

/// \details Function converts ASCII repr \c asciiBuff to binary byte array.
/// Every byte of output \c bytesBuff are repr as two bytes in \c asciiBuff,
/// where most signified tetrabits represented as leading byte in hex digit in ASCII encoding (upper) and
/// less signified tetrabits represented as tailing byte in hex digit in ASCII encoding (upper).
/// \c count is a size of input array \c asciiBuff.
/// \note Output array \c bytesBuff must be at least twice smaller than input array \c asciiBuff.
/// \returns Returns size of \c bytesBuff in bytes which calc as \c {output = count / 2}
MODBUS_EXPORT uint32_t asciiToBytes(const uint8_t* asciiBuff, uint8_t* bytesBuff, uint32_t count);

/// \details Make string representation of bytes array and separate bytes by space
MODBUS_EXPORT Char *sbytes(const uint8_t* buff, uint32_t count, Char *str, uint32_t strmaxlen);

/// \details Make string representation of ASCII array and separate bytes by space
MODBUS_EXPORT Char *sascii(const uint8_t* buff, uint32_t count, Char *str, uint32_t strmaxlen);

/// \details Get timer value in milliseconds.
MODBUS_EXPORT Timer timer();

/// \details Make current thread sleep with 'msec' milliseconds.
MODBUS_EXPORT void msleep(uint32_t msec);

#ifdef __cplusplus
} //extern "C"
#endif

#ifdef __cplusplus
} //namespace Modbus
#endif

#endif // MODBUSGLOBAL_H
