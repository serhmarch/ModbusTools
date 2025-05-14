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
#ifndef SERVER_DEVICE_H
#define SERVER_DEVICE_H

#include <QReadWriteLock>
#include <QSharedMemory>

#include <project/core_device.h>
#include <server_global.h>

class mbServerProject;

class mbServerDevice :  public mbCoreDevice
{
    Q_OBJECT
public:
    struct Strings : public mbCoreDevice::Strings
    {
        const QString count0x               ;
        const QString count1x               ;
        const QString count3x               ;
        const QString count4x               ;
        const QString isSaveData            ;
        const QString isReadOnly            ;
        const QString exceptionStatusAddress;
        const QString delay                 ;
        const QString isEnableScript        ;
        const QString scriptInit            ;
        const QString scriptLoop            ;
        const QString scriptFinal           ;

        Strings();
        static const Strings &instance();
    };

    struct Defaults : public mbCoreDevice::Defaults
    {
        const int  maxCount0x            ;
        const int  maxCount1x            ;
        const int  maxCount3x            ;
        const int  maxCount4x            ;
        const int  count0x               ;
        const int  count1x               ;
        const int  count3x               ;
        const int  count4x               ;
        const bool isSaveData            ;
        const bool isReadOnly            ;
        const int  exceptionStatusAddress;
        const uint delay                 ;
        const bool isEnableScript        ;

        Defaults();
        static const Defaults &instance();
    };

    class MemoryBlock
    {
    public:
        MemoryBlock();

    public:
        inline int size() const { QReadLocker _(&m_lock); return m_data.size(); }
        inline int sizeBits() const { QReadLocker _(&m_lock); return m_sizeBits; }
        inline int sizeBytes() const { return size(); }
        inline int sizeRegs() const { QReadLocker _(&m_lock); return m_data.size() / MB_REGE_SZ_BYTES; }
        void resize(int bytes);
        void resizeBits(int bits);
        inline void resizeBytes(int bytes) { resize(bytes); }
        inline void resizeRegs(int regs) { resize(regs*MB_REGE_SZ_BYTES); }
        void memGet(uint byteOffset, void *buff, size_t size);
        void memSetMask(uint byteOffset, const void *buff, const void *mask, size_t size);

    public:
        inline uint changeCounter() const { QReadLocker _(&m_lock); return m_changeCounter; }
        void zerroAll();
        Modbus::StatusCode read(uint offset, uint count, void *values, uint *fact = nullptr) const;
        Modbus::StatusCode write(uint offset, uint count, const void *values, uint *fact = nullptr);
        Modbus::StatusCode readBits(uint bitOffset, uint bitCount, void *values, uint *fact = nullptr) const;
        Modbus::StatusCode writeBits(uint bitOffset, uint bitCount, const void *values, uint *fact = nullptr);
        Modbus::StatusCode readBools(uint bitOffset, uint bitCount, bool *values, uint *fact = nullptr) const;
        Modbus::StatusCode writeBools(uint bitOffset, uint bitCount, const bool *values, uint *fact = nullptr);
        Modbus::StatusCode readRegs(uint regOffset, uint regCount, quint16 *values, uint *fact = nullptr) const;
        Modbus::StatusCode writeRegs(uint regOffset, uint regCount, const quint16 *values, uint *fact = nullptr);
        Modbus::StatusCode readFrameBools(uint bitOffset, int columns, QByteArray &values, uint maxColumns) const;
        Modbus::StatusCode writeFrameBools(uint bitOffset, int columns, const QByteArray &values, int maxColumns);
        Modbus::StatusCode readFrameRegs(uint regOffset, int columns, QByteArray &values, int maxColumns) const;
        Modbus::StatusCode writeFrameRegs(uint regOffset, int columns, const QByteArray &values, int maxColumns);

    private:
        mutable QReadWriteLock m_lock;
        QByteArray m_data;
        uint m_sizeBits;
        uint m_changeCounter;
    };

    enum ScriptType
    {
        Script_Init,
        Script_Loop,
        Script_Final
    };

public:
    explicit mbServerDevice(QObject *parent = nullptr);

public:
    inline mbServerProject* project() const { return reinterpret_cast<mbServerProject*>(mbCoreDevice::projectCore()); }
    inline void setProject(mbServerProject* project) { mbCoreDevice::setProjectCore(reinterpret_cast<mbCoreProject*>(project)); }

public: // Exception Status
    inline mb::Address exceptionStatusAddress() const { return m_settings.exceptionStatusAddress; }
    inline int exceptionStatusAddressInt() const { return mb::toInt(exceptionStatusAddress()); }
    inline QString exceptionStatusAddressStr() const { return mb::toString(exceptionStatusAddress()); }
    inline void setExceptionStatusAddress(mb::Address exceptionStatusAddress) { m_settings.exceptionStatusAddress = exceptionStatusAddress; }
    inline void setExceptionStatusAddressInt(int address) { setExceptionStatusAddress(mb::toAddress(address)); }
    inline void setExceptionStatusAddressStr(const QString& address) { setExceptionStatusAddress(mb::toAddress(address)); }
    inline void setExceptionStatusAddress(int address) { setExceptionStatusAddressInt(address); }
    inline void setExceptionStatusAddress(const QString& address) { setExceptionStatusAddressStr(address); }
    quint8 exceptionStatus() const;

public: // settings
    inline bool isReadOnly() const { return m_settings.isReadOnly; }
    inline void setReadOnly(bool v) { m_settings.isReadOnly = v; }
    inline bool isSaveData() const { return m_settings.isSaveData; }
    inline void setSaveData(bool save) { m_settings.isSaveData = save; }
    inline uint delay() const { return m_settings.delay; }
    inline void setDelay(uint delay) { m_settings.delay = delay; }
    inline bool isEnableScript() const { return m_settings.isEnableScript; }
    inline void setEnableScript(bool v) { m_settings.isEnableScript = v; }

    Modbus::Settings settings() const;
    bool setSettings(const Modbus::Settings& settings);

public:
    QByteArray readData(const mb::Address &address, quint16 count);
    void writeData(const mb::Address &address, quint16 count, const QByteArray &data);

public: // 'Modbus'-like Interface
    Modbus::StatusCode readCoils(uint16_t offset, uint16_t count, void *values);
    Modbus::StatusCode readDiscreteInputs(uint16_t offset, uint16_t count, void *values);
    Modbus::StatusCode readHoldingRegisters(uint16_t offset, uint16_t count, uint16_t *values);
    Modbus::StatusCode readInputRegisters(uint16_t offset, uint16_t count, uint16_t *values);
    Modbus::StatusCode writeSingleCoil(uint16_t offset, bool value);
    Modbus::StatusCode writeSingleRegister(uint16_t offset, uint16_t value);
    Modbus::StatusCode readExceptionStatus(uint8_t *status);
    Modbus::StatusCode writeMultipleCoils(uint16_t offset, uint16_t count, const void *values);
    Modbus::StatusCode writeMultipleRegisters(uint16_t offset, uint16_t count, const uint16_t *values);
    Modbus::StatusCode reportServerID(uint8_t *count, uint8_t *data);
    Modbus::StatusCode maskWriteRegister(uint16_t offset, uint16_t andMask, uint16_t orMask);
    Modbus::StatusCode readWriteMultipleRegisters(uint16_t readOffset, uint16_t readCount, uint16_t *readValues, uint16_t writeOffset, uint16_t writeCount, const uint16_t *writeValues);

public: // memory-0x management functions
    inline uint changeCounter_0x() const { return m_mem_0x.changeCounter(); }
    inline int count_0x() const { return m_mem_0x.sizeBits(); }
    inline int count_0x_bites() const { return m_mem_0x.sizeBits(); }
    inline int count_0x_bytes() const { return m_mem_0x.sizeBytes(); }
    inline int count_0x_reges() const { return m_mem_0x.sizeRegs(); }
    void realloc_0x(int count);
    inline void zerroAll_0x() { m_mem_0x.zerroAll(); }
    Modbus::StatusCode read_0x (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_0x.readBits (bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode write_0x(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_0x.writeBits(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_0x_bit(uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const  { return read_0x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_0x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return write_0x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_0x_bool(uint bitOffset, uint bitCount, bool* values, uint *fact = nullptr) const  { return m_mem_0x.readBools(bitOffset, bitCount, values, fact); }
    inline Modbus::StatusCode write_0x_bool(uint bitOffset, uint bitCount, const bool* values, uint *fact = nullptr) { return m_mem_0x.writeBools(bitOffset, bitCount, values, fact); }
    inline Modbus::StatusCode readFrame_0x(uint bitOffset, int columns, QByteArray& values, int maxColumns) const  { return m_mem_0x.readFrameBools(bitOffset, columns, values, maxColumns); }
    inline Modbus::StatusCode writeFrame_0x(uint bitOffset, int columns, const QByteArray& values, int maxColumns) { return m_mem_0x.writeFrameBools(bitOffset, columns, values, maxColumns); }
    inline bool bool_0x(uint bitOffset) const               { bool v = false; m_mem_0x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_0x(uint bitOffset, bool v)          { m_mem_0x.writeBits(bitOffset, 1, &v); }
    inline qint8 int8_0x(uint bitOffset) const              { qint8 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt8_0x(uint bitOffset, qint8 v)         { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint8 uint8_0x(uint bitOffset) const            { quint8 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt8_0x(uint bitOffset, quint8 v)       { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint16 int16_0x(uint bitOffset) const            { qint16 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt16_0x(uint bitOffset, qint16 v)       { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint16 uint16_0x(uint bitOffset) const          { quint16 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt16_0x(uint bitOffset, quint16 v)     { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint32 int32_0x(uint bitOffset) const            { qint32 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt32_0x(uint bitOffset, qint32 v)       { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint32 uint32_0x(uint bitOffset) const          { quint32 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt32_0x(uint bitOffset, quint32 v)     { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint64 int64_0x(uint bitOffset) const            { qint64 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt64_0x(uint bitOffset, qint64 v)       { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint64 uint64_0x(uint bitOffset) const          { quint64 v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt64_0x(uint bitOffset, quint64 v)     { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int int_0x(uint bitOffset) const                 { int v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt_0x(uint bitOffset, int v)            { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline unsigned int uint_0x(uint bitOffset) const       { unsigned int v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt_0x(uint bitOffset, unsigned int v)  { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline float float_0x(uint bitOffset) const             { float v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setFloat_0x(uint bitOffset, float v)        { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline double double_0x(uint bitOffset) const           { double v = 0; m_mem_0x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setDouble_0x(uint bitOffset, double v)      { m_mem_0x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }

    inline void mem0xGet(uint byteOffset, void *buff, size_t size) { m_mem_0x.memGet(byteOffset, buff, size); }
    inline void mem0xSetMask(uint byteOffset, const void *buff, const void *mask, size_t size) { m_mem_0x.memSetMask(byteOffset, buff, mask, size); }
    mbServerDevice::MemoryBlock &memBlockRef_0x() { return m_mem_0x; }

public: // memory-1x management functions
    inline uint changeCounter_1x() const { return m_mem_1x.changeCounter(); }
    inline int count_1x() const { return m_mem_1x.sizeBits(); }
    inline int count_1x_bites() const { return m_mem_1x.sizeBits(); }
    inline int count_1x_bytes() const { return m_mem_1x.sizeBytes(); }
    inline int count_1x_reges() const { return m_mem_1x.sizeRegs(); }
    void realloc_1x(int count);
    inline void zerroAll_1x() { m_mem_1x.zerroAll(); }
    Modbus::StatusCode read_1x (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_1x.readBits (bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode write_1x(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_1x.writeBits(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_1x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return read_1x (bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode write_1x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return write_1x(bitOffset, bitCount, bites, fact); }
    inline Modbus::StatusCode read_1x_bool (uint bitOffset, uint bitCount, bool* values, uint *fact = nullptr) const { return m_mem_1x.readBools (bitOffset, bitCount, values, fact); }
    inline Modbus::StatusCode write_1x_bool(uint bitOffset, uint bitCount, const bool* values, uint *fact = nullptr) { return m_mem_1x.writeBools(bitOffset, bitCount, values, fact); }
    inline Modbus::StatusCode readFrame_1x (uint bitOffset, int columns, QByteArray& values, int maxColumns) const { return m_mem_1x.readFrameBools (bitOffset, columns, values, maxColumns); }
    inline Modbus::StatusCode writeFrame_1x(uint bitOffset, int columns, const QByteArray& values, int maxColumns) { return m_mem_1x.writeFrameBools(bitOffset, columns, values, maxColumns); }
    inline bool bool_1x(uint bitOffset) const               { bool v = false; m_mem_1x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_1x(uint bitOffset, bool v)          { m_mem_1x.writeBits(bitOffset, 1, &v); }
    inline qint8 int8_1x(uint bitOffset) const              { qint8 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt8_1x(uint bitOffset, qint8 v)         { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint8 uint8_1x(uint bitOffset) const            { quint8 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt8_1x(uint bitOffset, quint8 v)       { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint16 int16_1x(uint bitOffset) const            { qint16 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt16_1x(uint bitOffset, qint16 v)       { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint16 uint16_1x(uint bitOffset) const          { quint16 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt16_1x(uint bitOffset, quint16 v)     { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint32 int32_1x(uint bitOffset) const            { qint32 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt32_1x(uint bitOffset, qint32 v)       { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint32 uint32_1x(uint bitOffset) const          { quint32 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt32_1x(uint bitOffset, quint32 v)     { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline qint64 int64_1x(uint bitOffset) const            { qint64 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt64_1x(uint bitOffset, qint64 v)       { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline quint64 uint64_1x(uint bitOffset) const          { quint64 v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt64_1x(uint bitOffset, quint64 v)     { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline int int_1x(uint bitOffset) const                 { int v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setInt_1x(uint bitOffset, int v)            { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline unsigned int uint_1x(uint bitOffset) const       { unsigned int v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setUInt_1x(uint bitOffset, unsigned int v)  { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline float float_1x(uint bitOffset) const             { float v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setFloat_1x(uint bitOffset, float v)        { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }
    inline double double_1x(uint bitOffset) const           { double v = 0; m_mem_1x.readBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); return v; }
    inline void setDouble_1x(uint bitOffset, double v)      { m_mem_1x.writeBits(bitOffset, sizeof(v) * MB_BYTE_SZ_BITES, &v); }

    inline void mem1xGet(uint byteOffset, void *buff, size_t size) { m_mem_1x.memGet(byteOffset, buff, size); }
    inline void mem1xSetMask(uint byteOffset, const void *buff, const void *mask, size_t size) { m_mem_1x.memSetMask(byteOffset, buff, mask, size); }
    mbServerDevice::MemoryBlock &memBlockRef_1x() { return m_mem_1x; }

public: // memory-3x management functions
    inline uint changeCounter_3x() const { return m_mem_3x.changeCounter(); }
    inline int count_3x() const { return m_mem_3x.sizeRegs(); }
    inline int count_3x_bites() const { return m_mem_3x.sizeBits(); }
    inline int count_3x_bytes() const { return m_mem_3x.sizeBytes(); }
    inline int count_3x_reges() const { return m_mem_3x.sizeRegs(); }
    void realloc_3x(int count);
    inline void zerroAll_3x() { m_mem_3x.zerroAll(); }
    Modbus::StatusCode read_3x (uint offset, uint regCount, void* values, uint *fact = nullptr) const { return m_mem_3x.readRegs (offset, regCount, reinterpret_cast<quint16*>(values), fact); }
    Modbus::StatusCode write_3x(uint offset, uint regCount, const void* values, uint *fact = nullptr) { return m_mem_3x.writeRegs(offset, regCount, reinterpret_cast<const quint16*>(values), fact); }
    Modbus::StatusCode read_3x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_3x.readBits (bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode write_3x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_3x.writeBits(bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode readFrame_3x (uint regOffset, int columns, QByteArray& values, int maxColumns) const { return m_mem_3x.readFrameRegs (regOffset, columns, values, maxColumns); }
    Modbus::StatusCode writeFrame_3x(uint regOffset, int columns, const QByteArray& values, int maxColumns) { return m_mem_3x.writeFrameRegs(regOffset, columns, values, maxColumns); }
    inline bool bool_3x(uint bitOffset) const       { bool v = false; m_mem_3x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_3x(uint bitOffset, bool v)  { m_mem_3x.writeBits(bitOffset, 1, &v); }
    qint8 int8_3x(quint32 byteOffset) const         { qint8 v = 0; m_mem_3x.read(byteOffset, sizeof(v), &v); return v; }
    void setInt8_3x(quint32 byteOffset, qint8 v)    { m_mem_3x.write(byteOffset, sizeof(v), &v); }
    quint8 uint8_3x(quint32 byteOffset) const       { quint8 v = 0; m_mem_3x.read(byteOffset, sizeof(v), &v); return v; }
    void setUInt8_3x(quint32 byteOffset, quint8 v)  { m_mem_3x.write(byteOffset, sizeof(v), &v); }
    qint16 int16_3x(uint regOffset) const           { qint16 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt16_3x(uint regOffset, qint16 v)      { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint16 uint16_3x(uint regOffset) const         { quint16 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt16_3x(uint regOffset, quint16 v)    { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    qint32 int32_3x(uint regOffset) const           { qint32 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt32_3x(uint regOffset, qint32 v)      { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint32 uint32_3x(uint regOffset) const         { quint32 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt32_3x(uint regOffset, quint32 v)    { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    qint64 int64_3x(uint regOffset) const           { qint64 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt64_3x(uint regOffset, qint64 v)      { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint64 uint64_3x(uint regOffset) const         { quint64 v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt64_3x(uint regOffset, quint64 v)    { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    int int_3x(uint regOffset) const                { int v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt_3x(uint regOffset, int v)           { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    unsigned int uint_3x(uint regOffset) const      { unsigned int v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt_3x(uint regOffset, unsigned int v) { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    float float_3x(uint regOffset) const            { float v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setFloat_3x(uint regOffset, float v)       { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    double double_3x(uint regOffset) const          { double v = 0; m_mem_3x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setDouble_3x(uint regOffset, double v)     { m_mem_3x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }

    inline void mem3xGet(uint byteOffset, void *buff, size_t size) { m_mem_3x.memGet(byteOffset, buff, size); }
    inline void mem3xSetMask(uint byteOffset, const void *buff, const void *mask, size_t size) { m_mem_3x.memSetMask(byteOffset, buff, mask, size); }
    mbServerDevice::MemoryBlock &memBlockRef_3x() { return m_mem_3x; }

public: // memory-4x management functions
    inline uint changeCounter_4x() const { return m_mem_4x.changeCounter(); }
    inline int count_4x() const { return m_mem_4x.sizeRegs(); }
    inline int count_4x_bites() const { return m_mem_4x.sizeBits(); }
    inline int count_4x_bytes() const { return m_mem_4x.sizeBytes(); }
    inline int count_4x_reges() const { return m_mem_4x.sizeRegs(); }
    void realloc_4x(int count);
    inline void zerroAll_4x() { m_mem_4x.zerroAll(); }
    Modbus::StatusCode read_4x (uint offset, uint regCount, void* values, uint *fact = nullptr) const { return m_mem_4x.readRegs (offset, regCount, reinterpret_cast<quint16*>(values), fact); }
    Modbus::StatusCode write_4x(uint offset, uint regCount, const void* values, uint *fact = nullptr) { return m_mem_4x.writeRegs(offset, regCount, reinterpret_cast<const quint16*>(values), fact); }
    Modbus::StatusCode read_4x_bit (uint bitOffset, uint bitCount, void* bites, uint *fact = nullptr) const { return m_mem_4x.readBits (bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode write_4x_bit(uint bitOffset, uint bitCount, const void* bites, uint *fact = nullptr) { return m_mem_4x.writeBits(bitOffset, bitCount, bites, fact); }
    Modbus::StatusCode readFrame_4x (uint regOffset, int columns, QByteArray& values, int maxColumns) const { return m_mem_4x.readFrameRegs (regOffset, columns, values, maxColumns); }
    Modbus::StatusCode writeFrame_4x(uint regOffset, int columns, const QByteArray& values, int maxColumns) { return m_mem_4x.writeFrameRegs(regOffset, columns, values, maxColumns); }
    inline bool bool_4x(uint bitOffset) const       { bool v = false; m_mem_4x.readBits(bitOffset, 1, &v); return v; }
    inline void setBool_4x(uint bitOffset, bool v)  { m_mem_4x.writeBits(bitOffset, 1, &v); }
    qint8 int8_4x(quint32 byteOffset) const         { qint8 v = 0; m_mem_4x.read(byteOffset, sizeof(v), &v); return v; }
    void setInt8_4x(quint32 byteOffset, qint8 v)    { m_mem_4x.write(byteOffset, sizeof(v), &v); }
    quint8 uint8_4x(quint32 byteOffset) const       { quint8 v = 0; m_mem_4x.read(byteOffset, sizeof(v), &v); return v; }
    void setUInt8_4x(quint32 byteOffset, quint8 v)  { m_mem_4x.write(byteOffset, sizeof(v), &v); }
    qint16 int16_4x(uint regOffset) const           { qint16 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt16_4x(uint regOffset, qint16 v)      { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint16 uint16_4x(uint regOffset) const         { quint16 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt16_4x(uint regOffset, quint16 v)    { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    qint32 int32_4x(uint regOffset) const           { qint32 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt32_4x(uint regOffset, qint32 v)      { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint32 uint32_4x(uint regOffset) const         { quint32 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt32_4x(uint regOffset, quint32 v)    { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    qint64 int64_4x(uint regOffset) const           { qint64 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt64_4x(uint regOffset, qint64 v)      { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    quint64 uint64_4x(uint regOffset) const         { quint64 v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt64_4x(uint regOffset, quint64 v)    { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    int int_4x(uint regOffset) const                { int v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setInt_4x(uint regOffset, int v)           { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    unsigned int uint_4x(uint regOffset) const      { unsigned int v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setUInt_4x(uint regOffset, unsigned int v) { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    float float_4x(uint regOffset) const            { float v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setFloat_4x(uint regOffset, float v)       { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }
    double double_4x(uint regOffset) const          { double v = 0; m_mem_4x.read(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); return v; }
    void setDouble_4x(uint regOffset, double v)     { m_mem_4x.write(regOffset * MB_REGE_SZ_BYTES, sizeof(v), &v); }

    inline void mem4xGet(uint byteOffset, void *buff, size_t size) { m_mem_4x.memGet(byteOffset, buff, size); }
    inline void mem4xSetMask(uint byteOffset, const void *buff, const void *mask, size_t size) { m_mem_4x.memSetMask(byteOffset, buff, mask, size); }
    mbServerDevice::MemoryBlock &memBlockRef_4x() { return m_mem_4x; }

public:
    QVariant value(mb::Address address, mb::DataType dataType);
    void setValue(mb::Address address, mb::DataType dataType, const QVariant& value);

public: // scripts
    MBSETTINGS scriptSources() const;
    void setScriptSources(const MBSETTINGS &script);

    inline QString scriptInit() const { return m_script.sInit; }
    inline void setScriptInit(const QString &script) { m_script.sInit = script; }

    inline QString scriptLoop() const { return m_script.sLoop; }
    inline void setScriptLoop(const QString &script) { m_script.sLoop = script; }

    inline QString scriptFinal() const { return m_script.sFinal; }
    inline void setScriptFinal(const QString &script) { m_script.sFinal = script; }

    QString script(ScriptType scriptType) const;
    void setScript(ScriptType scriptType, const QString &script);

Q_SIGNALS:
    void count_0x_changed(int count);
    void count_1x_changed(int count);
    void count_3x_changed(int count);
    void count_4x_changed(int count);

private: // Memory
    mutable QReadWriteLock m_lock;
    MemoryBlock m_mem_0x;
    MemoryBlock m_mem_1x;
    MemoryBlock m_mem_3x;
    MemoryBlock m_mem_4x;

private: // settings
    struct
    {
        bool        isSaveData            ;
        bool        isReadOnly            ;
        mb::Address exceptionStatusAddress;
        uint        delay                 ;
        bool        isEnableScript        ;
    } m_settings;

    struct
    {
        QString sInit;
        QString sLoop;
        QString sFinal;
    } m_script;
};

#endif // SERVER_DEVICE_H
