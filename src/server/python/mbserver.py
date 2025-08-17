"""@package docstring
Documentation for this module.

More details.
"""

#from typing import Union

from os import path
from ctypes import *
import struct

from typing import Union
from PyQt5.QtCore import QSharedMemory

from mbconfig import *
import modbus

def swapbyteorder(data: bytearray) -> bytearray:
    """
    @note Since v0.4.2

    @details
    Function swaps byte array pair: 0 with 1, 2 with 3, 4 with 5 and so on.
    If byte count is odd then last byte is left unchanged.

    @return Input `data` reference
    """
    for j in range(0, len(data) // 2):
        i = j * 2
        data[i], data[i + 1] = data[i + 1], data[i]
    return data

## @cond

MB_DATAORDER_DEFAULT      = -1
MB_DATAORDER_LITTLEENDIAN = 0
MB_DATAORDER_BIGENDIAN    = 1

MB_REGISTERORDER_DEFAULT  = -1
MB_REGISTERORDER_R0R1R2R3 = 0
MB_REGISTERORDER_R3R2R1R0 = 1
MB_REGISTERORDER_R1R0R3R2 = 2
MB_REGISTERORDER_R2R3R0R1 = 3

MB_BYTEORDER_DEFAULT = 'little'

# Note (Feb 08 2025): c_long type was replaced by c_int because
#                     on some platforms c_long is size of 8 bytes


class CDeviceBlock(Structure): 
    _fields_ = [("flags"             , c_uint),
                ("cycle"             , c_uint),
                ("count0x"           , c_uint),
                ("count1x"           , c_uint),
                ("count3x"           , c_uint),
                ("count4x"           , c_uint),
                ("exceptionStatusRef", c_uint),
                ("byteOrder"         , c_int ),
                ("registerOrder"     , c_int ),
                ("stoDeviceName"     , c_uint),
                ("stringTableSize"   , c_uint)]

class CPythonBlock(Structure): 
    _fields_ = [("pycycle"           , c_uint)]

class CMemoryBlockHeader(Structure): 
    _fields_ = [("changeCounter"     , c_uint),
                ("changeByteOffset"  , c_uint),
                ("changeByteCount"   , c_uint),
                ("dummy"             , c_uint)]

## @endcond


class _MemoryBlock:
    """Base class for the memory objects mem0x, mem1x, mem3x, mem4x.

       Class is abstract (can't be used directly). 
    """
    ## @cond
    def __init__(self, shmid:str, bytecount:int, id:int, byteorder, regorder:int):
        if isinstance(byteorder, int):
            if byteorder == MB_DATAORDER_BIGENDIAN:
                self._byteorder = 'big'
            else:
                self._byteorder = 'little'
        elif isinstance(byteorder, str):
            if byteorder == 'big':
                self._byteorder = 'big'
            else:
                self._byteorder = 'little'
        else:
            self._byteorder = 'little'
        self._registerorder = regorder
        shm = QSharedMemory(shmid)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{shmid}'")
        qptr = shm.data()
        memptr = c_void_p(qptr.__int__())
        sz = shm.size()
        cbytes = bytecount if bytecount <= sz else sz
        self._shm = shm
        self._countbytes = cbytes
        self._id = id
        ptrhead = cast(memptr, POINTER(CMemoryBlockHeader))
        self._head = ptrhead[0]
        self._pmembytes = cast(byref(ptrhead[1]),POINTER(c_ubyte*1))
        self._pmaskbytes = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_ubyte*1))

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def _recalcheader(self, byteoffset:int, bytecount:int):
        rightedge = byteoffset + bytecount
        if self._head.changeByteOffset > byteoffset:
            if self._head.changeByteCount == 0:
                self._head.changeByteCount = rightedge - byteoffset
            else:
                self._head.changeByteCount += self._head.changeByteOffset - byteoffset # Fixed Jan 30 2025
            self._head.changeByteOffset = byteoffset
        if self._head.changeByteOffset + self._head.changeByteCount < rightedge:
            self._head.changeByteCount = rightedge - self._head.changeByteOffset
        self._head.changeCounter += 1

    def _getbytes(self, byteoffset: int, count: int, bytestype=bytes) -> Union[bytes, bytearray]:
        if 0 <= byteoffset < self._countbytes:
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            b = bytestype(cast(self._pmembytes[byteoffset], POINTER(c_ubyte*c))[0])
            self._shm.unlock()
            return b
        return bytestype()

    def swap32(self, ba:bytearray)->bytearray:
        # Split into 2 16-bit (2-byte) regs
        regs = [ba[i:i+2] for i in range(0, 4, 2)]  # R0, R1, R2, R3

        # Apply byteorder: swap bytes within each register if big-endian
        if self._byteorder == 'big':
            regs = [reg[::-1] for reg in regs]

        # swap registers if needed
        if self._registerorder == MB_REGISTERORDER_R3R2R1R0 or self._registerorder == MB_REGISTERORDER_R1R0R3R2:
            regs = [regs[1],regs[0]]

        # Combine bytes correctly (this is the fixed part)
        result = bytearray()
        for reg in regs:
            result.extend(reg)
        return result


    def swap64(self, ba:bytearray)->bytearray:
        # Split into 4 16-bit (2-byte) regs
        regs = [ba[i:i+2] for i in range(0, 8, 2)]  # R0, R1, R2, R3

        # Apply byteorder: swap bytes within each register if big-endian
        if self._byteorder == 'big':
            regs = [reg[::-1] for reg in regs]

        # swap registers if needed
        if   self._registerorder == MB_REGISTERORDER_R3R2R1R0:
            regs = [regs[3],regs[2],regs[1],regs[0]]
        elif self._registerorder == MB_REGISTERORDER_R1R0R3R2:
            regs = [regs[1],regs[0],regs[3],regs[2]]
        elif self._registerorder == MB_REGISTERORDER_R2R3R0R1:
            regs = [regs[2],regs[3],regs[0],regs[1]]

        # Combine bytes correctly (this is the fixed part)
        result = bytearray()
        for reg in regs:
            result.extend(reg)
        return result
    ## @endcond

    def getid(self)->int:
        """
        @details Returns id (0, 1, 3 or 4) of the current memory object as integer.
        """
        return self._id
    
    def getbyteorder(self)->str:
        """
        @note Since v0.4.2

        @details Returns string 'little' or 'big' for current byte order
        """
        return self._byteorder

    def getregisterorder(self)->str:
        """
        @note Since v0.4.2

        @details Returns int that represents current register order:
        *  -1 = default
        *  0  = R0R1R2R3
        *  1  = R3R2R1R0
        *  2  = R1R0R3R2
        *  3  = R2R3R0R1
        """
        return self._registerorder

    def getbytes(self, byteoffset:int, bytecount:int)->bytes:
        """
        @details Function returns `bytes` object from device memory  starting with `byteoffset` and `bytecount` bytes.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).
        @param[in]  bytecount   Count of bytes to read.
        """
        return self._getbytes(byteoffset, bytecount, bytes)

    def getbytearray(self, byteoffset:int, bytecount:int)->bytearray:
        """
        @details 
        Function returns `bytearray` object from device memory  starting with `byteoffset` and `count` bytes.
        Parameters are the same as getbytes() function.

        @sa getbytes()
        """
        return self._getbytes(byteoffset, bytecount, bytearray)

    def setbytes(self, byteoffset: int, value: Union[bytes, bytearray]):
        """
        @details 
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `byteoffset`.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).
        @param[in]  value       `bytes` or `bytearray` value to set.
        """
        ## @cond
        if 0 <= byteoffset < self._countbytes:
            count = len(value)
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            if not isinstance(value, bytes):
                value = bytes(value)
            self._shm.lock()
            memmove(self._pmembytes[byteoffset], value, c)
            memset(self._pmaskbytes[byteoffset], -1, c)
            self._recalcheader(byteoffset, c)
            self._shm.unlock()
        ## @endcond

    def getbitbytearray(self, bitoffset:int, bitcount:int)->bytearray:
        """
        @details 
        Function returns `bytearray` object from device memory starting with `bitoffset` and `bitcount` bits.
        Parameters are the same as `getbitbytes()` function.

        @sa getbitbytes()
        """
        byteoffset = bitoffset // 8
        rbyteoffset = (bitoffset+bitcount-1) // 8
        bytecount = rbyteoffset-byteoffset+1
        byarray = self._getbytes(byteoffset, bytecount, bytearray)
        shift = bitoffset % 8
        rem = bitcount % 8
        ri = (bitcount-1) // 8
        if shift:
            c = len(byarray)-1
            for i in range(c):
                b1 = byarray[i]
                b2 = byarray[i+1]   
                b = ((b2 << (8-shift)) | (b1 >> shift)) & 0xFF
                byarray[i] = b
        if rem:
            mask = (1 << (rem-1))
            mask |= (mask-1)
            b = byarray[ri]
            b = (b >> shift) & mask
            byarray[ri] = b
        if len(byarray) > ri+1:
            del byarray[ri+1]
        return byarray

    def getbitbytes(self, bitoffset:int, bitcount:int)->bytes:
        """
        @details
        Function returns `bytes` object from device memory starting with `bitoffset` and `count` bits.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15, 
        register with offset 1 is bit offset 16-31, etc.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  bitcount    Count of bits to read.
        """
        return bytes(self.getbitbytearray(bitoffset, bitcount))

    def setbitbytes(self, bitoffset: int, bitcount: int, value: Union[bytes, bytearray]):
        """
        @details
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `bitoffset` and `bitcount` bits.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15, 
        register with offset 1 is bit offset 16-31, etc.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  bitcount    Count of bits to write from `value` array.
        @param[in]  value       Array of bytes that contains bits to write.
        """
        byteoffset = bitoffset // 8
        rbyteoffset = (bitoffset+bitcount-1) // 8
        bytecount = rbyteoffset-byteoffset+1
        byarray = self._getbytes(byteoffset, bytecount, bytearray)
        shift = bitoffset % 8
        c = bitcount // 8
        rem = bitcount % 8
        if shift:
            mask = 0xFF << shift
            notmask = ~mask
            for i in range(c):
                v = value[i] << shift
                b = int.from_bytes([byarray[i], byarray[i+1]], byteorder=MB_BYTEORDER_DEFAULT)
                b &= notmask
                b |= v
                tb = b.to_bytes(2, byteorder=MB_BYTEORDER_DEFAULT)
                byarray[i]   = tb[0]
                byarray[i+1] = tb[1]
        elif c > 0:
            byarray[0:c] = value[0:c]
        if rem:
            mask = (1 << (rem-1))
            mask |= (mask-1)
            mask = mask << shift
            notmask = ~mask
            v = (value[c] << shift) & mask
            if shift+rem > 8:
                b = int.from_bytes([byarray[c], byarray[c+1]], byteorder=MB_BYTEORDER_DEFAULT)
                b &= notmask
                b |= v
                tb = b.to_bytes(2, byteorder=MB_BYTEORDER_DEFAULT)
                byarray[c]   = tb[0]
                byarray[c+1] = tb[1]
            else:
                b = byarray[c] & notmask
                b |= v
                byarray[c] = b
        self.setbytes(byteoffset, bytes(byarray))

    def getbit(self, bitoffset:int)->bool:
        """
        @details
        Function returns value of bit (`True` or `False`) from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15, 
        register with offset 1 is bit offset 16-31, etc.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  bitcount    Count of bits to read.

        @note If `bitoffset` is out of range, function returns `False`.
        """
        ## @cond
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            vbyte = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return (vbyte & (1 << bitoffset % 8)) != 0
        return False
        ## @endcond

    def setbit(self, bitoffset:int, value:bool):
        """
        @details
        Function set value of bit (`True` or `False`) into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15, 
        register with offset 1 is bit offset 16-31, etc.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to set: `True` or `False`.

        @note If `bitoffset` is out of range, function does nothing.
        """
        ## @cond
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            if value:
                self._pmembytes[byteoffset][0] |= (1 << (bitoffset % 8))
            else:
                self._pmembytes[byteoffset][0] &= ~(1 << (bitoffset % 8))
            self._pmaskbytes[byteoffset][0] |= (1 << bitoffset % 8)
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()
        ## @endcond

    def getbitstring(self, bitoffset:int, bytecount:int)->str:
        """
        @note Since v0.4.2

        @details
        Function returns `string` object from device memory starting with `bitoffset` and `bytecount` bytes for `utf-8` encoding.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15,
        register with offset 1 is bit offset 16-31, etc.

        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  bytecount   Count of byte of `utf-8` string encoding to read.
        """
        ## @cond
        if self._byteorder == 'big':
            return swapbyteorder(self.getbitbytearray(bitoffset, bytecount*8)).decode()
        return self.getbitbytes(bitoffset, bytecount*8).decode()
        ## @endcond

    def setbitstring(self, bitoffset:int, value:str):
        """
        @note Since v0.4.2

        @details
        Function set `string` object into device memory starting with `bitoffset`.
        Value will be set using `utf-8` encoding.

        For bit memory `bitoffset` is offset of the bit/coil.
        For register memory `bitoffset` is calculated as `offset * 16`.
        So register with offset 0 is bit offset 0-15,
        register with offset 1 is bit offset 16-31, etc.

        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       String value to be written.
        """
        ## @cond
        if self._byteorder == 'big':
            b = swapbyteorder(bytearray(value.encode()))
        else:
            b = value.encode()
        self.setbitbytes(bitoffset, len(b)*8, b)
        ## @endcond

    def getbytestring(self, byteoffset:int, bytecount:int)->str:
        """
        @note Since v0.4.2

        @details
        Function returns `string` object from device memory starting with `byteoffset` and `bytecount` bytes for `utf-8` encoding.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1,
        register with offset 1 is byte offset 2 and 3, etc.

        @param[in]  byteoffset  Byte offset (0-based).
        @param[in]  bytecount   Count of bytes to read.
        """
        ## @cond
        if self._byteorder == 'big':
            return swapbyteorder(self.getbytearray(byteoffset, bytecount)).decode()
        return self.getbytes(byteoffset, bytecount).decode()
        ## @endcond

    def setbytestring(self, byteoffset:int, value:str):
        """
        @note Since v0.4.2

        @details
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `byteoffset`.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1,
        register with offset 1 is byte offset 2 and 3, etc.

        @param[in]  byteoffset  Byte offset (0-based).
        @param[in]  value       String value to be written.
        """
        ## @cond
        if self._byteorder == 'big':
            b = swapbyteorder(bytearray(value.encode()))
        else:
            b = value.encode()
        self.setbytes(byteoffset, b)
        ## @endcond

    def getregstring(self, regoffset:int, bytecount:int)->str:
        """
        @note Since v0.4.2

        @details
        Function returns `string` object from device memory starting with `regoffset` and `bytecount` bytes for `utf-8` encoding.
        `regoffset` is offset in 16-bit registers and equal to `byteoffset * 2`.
        So `regoffset = 0` is equal to `byteoffset = 0`, `regoffset = 1` is equal to `byteoffset = 2` ans so on.

        For bit memory `regoffset` is calculated as `bitoffset // 16`.
        So bit with offset 0 is in register with offset 0, bit with offset 16 is registere with offset 1, etc.
        For register memory `regoffset` is equal to `offset`.
        So register with offset 0 is byte offset 0 and 1,
        register with offset 1 is byte offset 2 and 3, etc.

        @param[in]  regoffset   16-bit register offset (0-based).
        @param[in]  bytecount   Count of bytes to read.
        """
        ## @cond
        if self._byteorder == 'big':
            return swapbyteorder(self.getbytearray(regoffset*2, bytecount)).decode()
        return self.getbytes(regoffset*2, bytecount).decode()
    ## @endcond

    def setregstring(self, regoffset:int, value:str):
        """
        @note Since v0.4.2

        @details
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `regoffset`, offset in 16-bit registers and equal to `byteoffset * 2`.
        So `regoffset = 0` is equal to `byteoffset = 0`, `regoffset = 1` is equal to `byteoffset = 2` ans so on.

        For bit memory `regoffset` is calculated as `bitoffset // 16`.
        So bit with offset 0 is in register with offset 0, bit with offset 16 is registere with offset 1, etc.
        For register memory `regoffset` is equal to `offset`.
        So register with offset 0 is byte offset 0 and 1,
        register with offset 1 is byte offset 2 and 3, etc.

        @param[in]  regoffset   16-bit register offset (0-based).
        @param[in]  value       String value to be written.
        """
        ## @cond
        if self._byteorder == 'big':
            b = swapbyteorder(bytearray(value.encode()))
        else:
            b = value.encode()
        self.setbytes(regoffset*2, b)
        ## @endcond


class _MemoryBlockBits(_MemoryBlock):
    """Class for the bit memory objects: mem0x, mem1x.

       More details. 
    """
    ## @cond
    def __init__(self, shmid:str, count:int, id:int, byteorder, regorder:int):
        super().__init__(shmid, (count+7)//8, id, byteorder, regorder)
        c = self._countbytes * 8
        self._count = count if count <= c else c
    ## @endcond

    def __getitem__(self, index:int)->int:
        """
        @details
        Same as getbit() function but generates `IndexError` if `index` is out of range.
        """
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.getbit(index)
    
    def __setitem__(self, index:int, value:int):
        """
        @details
        Same as setbit() function but generates `IndexError` if `index` is out of range.
        """
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        self.setbit(index, value)
    
    def getint8(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [-128:127] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 8)
            return int.from_bytes(b, byteorder=self._byteorder, signed=True)
        return 0
    
    def setint8(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [-128:127] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-7:
            b = value.to_bytes(1, self._byteorder, signed=True)
            self.setbitbytes(bitoffset, 8, b)

    def getuint8(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [0:255] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 8)
            return int.from_bytes(b, byteorder=self._byteorder, signed=False)
        return 0
    
    def setuint8(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [0:255] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-7:
            b = value.to_bytes(1, self._byteorder, signed=False)
            self.setbitbytes(bitoffset, 8, b)

    def getint16(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [-32768:32767] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-15:
            b = self.getbitbytearray(bitoffset, 16)
            return int.from_bytes(b, byteorder=self._byteorder, signed=True)
        return 0
    
    def setint16(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [-32768:32767] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-15:
            b = value.to_bytes(2, self._byteorder, signed=True)
            self.setbitbytes(bitoffset, 16, b)

    def getuint16(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [0:65535] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-15:
            b = self.getbitbytearray(bitoffset, 16)
            return int.from_bytes(b, byteorder=self._byteorder, signed=False)
        return 0
    
    def setuint16(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [0:65535] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-15:
            b = value.to_bytes(2, self._byteorder, signed=False)
            self.setbitbytes(bitoffset, 16, b)

    def getint32(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [-2147483648:2147483647] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(self.getbitbytearray(bitoffset, 32))
            return int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
        return 0
    
    def setint32(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [-2147483648:2147483647] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=True))
            self.setbitbytes(bitoffset, 32, b)

    def getuint32(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [0:4294967295] from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(self.getbitbytearray(bitoffset, 32))
            return int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
        return 0
    
    def setuint32(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [0:4294967295] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=False))
            self.setbitbytes(bitoffset, 32, b)

    def getint64(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [-9223372036854775808:9223372036854775807]
        from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(self.getbitbytearray(bitoffset, 64))
            return int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
        return 0
    
    def setint64(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [-9223372036854775808:9223372036854775807]
        into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=True))
            self.setbitbytes(bitoffset, 64, b)

    def getuint64(self, bitoffset:int)->int:
        """
        @details
        Function returns integer value of [0:18446744073709551615]
        from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(self.getbitbytearray(bitoffset, 64))
            return int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
        return 0
    
    def setuint64(self, bitoffset:int, value:int):
        """
        @details
        Function set integer value of [0:18446744073709551615]
        into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=False))
            self.setbitbytes(bitoffset, 64, b)

    def getfloat(self, bitoffset:int)->float:
        """
        @details
        Function returns float 32-bit value from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(self.getbitbytearray(bitoffset, 32))
            return struct.unpack('<f', b)[0]
        return 0.0
    
    def setfloat(self, bitoffset:int, value:float):
        """
        @details
        Function set float 32-bit value into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = self.swap32(struct.pack('<f', value))
            self.setbitbytes(bitoffset, 32, b)

    def getdouble(self, bitoffset:int)->float:
        """
        @details
        Function returns float 64-bit value from device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(self.getbitbytearray(bitoffset, 64))
            return struct.unpack('<d', b)[0]
        return 0.0
    
    def setdouble(self, bitoffset:int, value:float):
        """
        @details
        Function set float 64-bit value into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-63:
            b = self.swap64(struct.pack('<d', value))
            self.setbitbytes(bitoffset, 64, b)

    def getstring(self, bitoffset:int, bytecount:int)->str:
        """
        @note Since v0.4.2

        @details
        For bit memory is same as `getbitstring()`.

        @sa  `getbitstring()`
        """
        return self.getbitstring(bitoffset, bytecount)

    def setstring(self, bitoffset:int, value:str):
        """
        @note Since v0.4.2

        @details
        For bit memory is same as `setbitstring()`.

        @sa  `setbitstring()`
        """
        self.setbitstring(bitoffset, value)


class _MemoryBlockRegs(_MemoryBlock):
    """Class for the register memory objects: mem3x, mem4x.

       More details. 
    """
    ## @cond
    def __init__(self, shmid:str, count:int, id:int, byteorder, regorder:int):
        super().__init__(shmid, count*2, id, byteorder, regorder)
        c = self._countbytes // 2
        self._count = count if count <= c else c
        self._pmem = cast(self._pmembytes,POINTER(c_ushort*1))
        self._pmask = cast(self._pmaskbytes,POINTER(c_ushort*1))
    ## @endcond

    def __getitem__(self, index:int)->int:
        """
        @details
        Same as getuint16() function but generates `IndexError` if `index` is out of range.
        """
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.getuint16(index)
    
    def __setitem__(self, index:int, value:int)->int:
        """
        @details
        Same as setuint16() function but generates `IndexError` if `index` is out of range.
        """
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.setuint16(index, value)
    
    def getint8(self, regoffset:int)->int:
        """
        @note Since v0.4.4 offset input parameter was changed from `byteoffset` to `regoffset`

        @details
        Function returns integer value of [-128:127] from device memory starting with `regoffset`.

        For register memory `regoffset` defines register offset.
        It means that only first byte of the register
        will be got using this method.

        @param[in]  regoffset  Register offset (0-based).

        @note If `regoffset` is out of range, function returns `0`.
        """
        ## @cond
        byteoffset = regoffset * 2
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            value = cast(self._pmembytes[byteoffset], POINTER(c_byte))[0]
            self._shm.unlock()
            return value
        return 0
        ## @endcond

    def setint8(self, regoffset:int, value:int):
        """
        @note Since v0.4.4 offset input parameter was changed from `byteoffset` to `regoffset`

        @details
        Function set value of [-128:127] into device memory starting with `regoffset`.

        For register memory `regoffset` defines register offset.
        It means that only first byte of the register
        will be set using this method.

        @param[in]  regoffset  register offset (0-based).
        @param[in]  value      Integer value of [-128:127].

        @note If `regoffset` is out of range, function does nothing.
        """
        self.setuint8(regoffset, value)

    def getuint8(self, regoffset:int)->int:
        """
        @note Since v0.4.4 offset input parameter was changed from `byteoffset` to `regoffset`

        @details
        Function returns integer value of [0:255] from device memory starting with `regoffset`.

        For register memory `regoffset` defines register offset.
        It means that only first byte of the register
        will be got using this method.

        @param[in]  regoffset  Register offset (0-based).

        @note If `regoffset` is out of range, function returns `0`.
        """
        ## @cond
        byteoffset = regoffset * 2
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            r = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return r
        return 0
        ## @endcond
    
    def setuint8(self, regoffset:int, value:int):
        """
        @note Since v0.4.4 offset input parameter was changed from `byteoffset` to `regoffset`

        @details
        Function set value of [0:255] into device memory starting with `regoffset`.

        For register memory `regoffset` defines register offset.
        It means that only first byte of the register
        will be set using this method.
        
        @param[in]  regoffset  Register offset (0-based).
        @param[in]  value      Integer value of [0:255].

        @note If `regoffset` is out of range, function does nothing.
        """
        ## @cond
        byteoffset = regoffset * 2
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            self._pmembytes [byteoffset][0] = value
            self._pmaskbytes[byteoffset][0] = 0xFF
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()
        ## @endcond
            
    def getint16(self, offset:int)->int:
        """
        @details
        Function returns integer value of [-32768:32767] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count:
            self._shm.lock()
            value = cast(self._pmem[offset], POINTER(c_short))[0]
            self._shm.unlock()
            if self._byteorder == 'big':
                value = struct.unpack('<h', struct.pack('>h', value))[0]
            return value
        return 0

    def setint16(self, offset:int, value:int):
        """
        @details
        Function set integer value of [-32768:32767] into device memory starting with `offset`.

        @param[in]  offset  Offset of the first register (0-based).
        @param[in]  value   Integer value of [-32768:32767].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count:
            if self._byteorder == 'big':
                value = struct.unpack('<h', struct.pack('>h', value))[0]
            self._shm.lock()
            self._pmem [offset][0] = value
            self._pmask[offset][0] = 0xFFFF
            self._recalcheader(offset*2, 2)
            self._shm.unlock()

    def getuint16(self, offset:int)->int:
        """
        @details
        Function returns integer value of [0:65535] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count:
            self._shm.lock()
            value = self._pmem[offset][0]
            self._shm.unlock()
            if self._byteorder == 'big':
                value = struct.unpack('<H', struct.pack('>H', value))[0]
            return value
        return 0
    
    def setuint16(self, offset:int, value:int):
        """
        @details
        Function set integer value of [0:65535] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    Integer value of [0:65535].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count:
            if self._byteorder == 'big':
                value = struct.unpack('<H', struct.pack('>H', value))[0]
            self._shm.lock()
            self._pmem [offset][0] = value
            self._pmask[offset][0] = 0xFFFF
            self._recalcheader(offset*2, 2)
            self._shm.unlock()

    def getint32(self, offset:int)->int:
        """
        @details
        Function returns integer value of [-2147483648:2147483647] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            value = int(cast(self._pmem[offset], POINTER(c_int))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=True))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
            return value
        return 0

    def setint32(self, offset:int, value:int):
        """
        @details
        Function set integer value of [-2147483648:2147483647] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    Integer value of [-2147483648:2147483647].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-1:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=True))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_int))[0] = value
            cast(self._pmask[offset], POINTER(c_int))[0] = 0xFFFFFFFF
            self._recalcheader(offset*2, 4)
            self._shm.unlock()

    def getuint32(self, offset:int)->int:
        """
        @details
        Function returns integer value of [0:4294967295] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            value = int(cast(self._pmem[offset], POINTER(c_uint))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=False))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
            return value
        return 0
    
    def setuint32(self, offset:int, value:int):
        """
        @details
        Function set integer value of [0:4294967295] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    Integer value of [0:4294967295].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-1:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(value.to_bytes(4, MB_BYTEORDER_DEFAULT, signed=False))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_uint))[0] = value
            cast(self._pmask[offset], POINTER(c_uint))[0] = 0xFFFFFFFF
            self._recalcheader(offset*2, 4)
            self._shm.unlock()

    def getint64(self, offset:int)->int:
        """
        @details
        Function returns integer value of [-9223372036854775808:9223372036854775807]
        from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-3:
            self._shm.lock()
            value = int(cast(self._pmem[offset], POINTER(c_longlong))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=True))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
            return value
        return 0

    def setint64(self, offset:int, value:int):
        """
        @details
        Function set integer value of [-9223372036854775808:9223372036854775807]
        into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    Integer value of [-9223372036854775808:9223372036854775807].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-3:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=True))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=True)
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_longlong))[0] = value
            cast(self._pmask[offset], POINTER(c_longlong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()

    def getuint64(self, offset:int)->int:
        """
        @details
        Function returns integer value of [0:18446744073709551615]
        from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-3:
            self._shm.lock()
            value = int(cast(self._pmem[offset], POINTER(c_ulonglong))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=False))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
            return value
        return 0
    
    def setuint64(self, offset:int, value:int):
        """
        @details
        Function set integer value of [0:18446744073709551615]
        into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    Integer value of [0:18446744073709551615].

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-3:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(value.to_bytes(8, MB_BYTEORDER_DEFAULT, signed=False))
                value = int.from_bytes(b, byteorder=MB_BYTEORDER_DEFAULT, signed=False)
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_ulonglong))[0] = value
            cast(self._pmask[offset], POINTER(c_ulonglong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()

    def getfloat(self, offset:int)->int:
        """
        @details
        Function returns float 32-bit value from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            value = float(cast(self._pmem[offset], POINTER(c_float))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(struct.pack('<f', value))
                value = struct.unpack('<f', b)[0]
            return value
        return 0
    
    def setfloat(self, offset:int, value:float):
        """
        @details
        Function set float 32-bit value into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    32-bit float value.

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-1:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap32(struct.pack('<f', value))
                value = struct.unpack('<f', b)[0]
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_float))[0] = value
            cast(self._pmask[offset], POINTER(c_uint))[0] = 0xFFFFFFFF
            self._recalcheader(offset*2, 4)
            self._shm.unlock()

    def getdouble(self, offset:int)->int:
        """
        @details
        Function returns float 64-bit value from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-3:
            self._shm.lock()
            value = float(cast(self._pmem[offset], POINTER(c_double))[0])
            self._shm.unlock()
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(struct.pack('<d', value))
                value = struct.unpack('<d', b)[0]
            return value
        return 0
    
    def setdouble(self, offset:int, value:float):
        """
        @details
        Function set float 64-bit value into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).
        @param[in]  value    64-bit float value.

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-3:
            if not (self._byteorder == MB_BYTEORDER_DEFAULT and self._registerorder == MB_REGISTERORDER_R0R1R2R3):
                b = self.swap64(struct.pack('<d', value))
                value = struct.unpack('<d', b)[0]
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_double))[0] = value
            cast(self._pmask[offset], POINTER(c_ulonglong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()

    def getstring(self, regoffset:int, bytecount:int)->str:
        """
        @note Since v0.4.2

        @details
        For register memory is same as `getregstring()`.

        @sa  `getregstring()`
        """
        return self.getregstring(regoffset, bytecount)

    def setstring(self, regoffset:int, value:str):
        """
        @note Since v0.4.2

        @details
        For register memory is same as `setregstring()`.

        @sa  `setregstring()`
        """
        self.setregstring(regoffset, value)


class _MbDevice:
    """Class for access device parameters.

       More details. 
    """
    ## @cond
    def __init__(self, shmidprefix:str, project:str):
        self._libpath = path.dirname(path.abspath(__file__))
        self._projectpath = path.dirname(path.abspath(project))
        self._projectfile = path.basename(project)
        shmid_device = shmidprefix + ".device"
        shmid_python = shmidprefix + ".python"
        shmid_mem0x  = shmidprefix + ".mem0x"
        shmid_mem1x  = shmidprefix + ".mem1x"
        shmid_mem3x  = shmidprefix + ".mem3x"
        shmid_mem4x  = shmidprefix + ".mem4x"
        shm = QSharedMemory(shmid_device)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{shmid_device}'")
        qptr = shm.data()
        size = shm.size()
        memptr = c_void_p(qptr.__int__())
        pcontrol = cast(memptr, POINTER(CDeviceBlock))
        self._shm = shm
        self._pcontrol = pcontrol
        self._control = pcontrol.contents
        self._shm.lock()
        self._count0x       = int(self._control.count0x)
        self._count1x       = int(self._control.count1x)
        self._count3x       = int(self._control.count3x)
        self._count4x       = int(self._control.count4x)
        self._excstatusref  = int(self._control.exceptionStatusRef)
        self._byteorder     = int(self._control.byteOrder)
        self._registerorder = int(self._control.registerOrder)
        self._strtablesize  = int(self._control.stringTableSize)
        stoDeviceName = int(self._control.stoDeviceName)
        self._pmemstrtable = cast(byref(pcontrol[1]),POINTER(c_ubyte*1))
        self._name = self._getstring(stoDeviceName)
        self._shm.unlock()
        # submodules
        self._python = _MemoryPythonBlock(shmid_python)
        self._mem0x  = _MemoryBlockBits(shmid_mem0x, self._count0x, 0, self._byteorder, self._registerorder)
        self._mem1x  = _MemoryBlockBits(shmid_mem1x, self._count1x, 1, self._byteorder, self._registerorder)
        self._mem3x  = _MemoryBlockRegs(shmid_mem3x, self._count3x, 3, self._byteorder, self._registerorder)
        self._mem4x  = _MemoryBlockRegs(shmid_mem4x, self._count4x, 4, self._byteorder, self._registerorder)
        self._memdict = { modbus.Memory_0x: self._mem0x,
                          modbus.Memory_1x: self._mem1x,
                          modbus.Memory_3x: self._mem3x,
                          modbus.Memory_4x: self._mem4x }
        # Exception status
        adr = modbus.Address(self._excstatusref)
        self._excoffset = adr.offset()
        self._excmem = self._memdict.get(adr.type())
        if self._excmem is None:
            self._excmem = self._mem0x
            self._excoffset = 0

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def _getstring(self, offset:int)->str:
        c = 0
        while self._pmemstrtable[offset+c][0] != 0:
            c += 1
        bs = bytes(cast(self._pmemstrtable[offset], POINTER(c_ubyte*c))[0])
        return bs.decode('utf-8')
    ## @endcond
    
    def getmemsize(self)->int:
        return self._shm.size()

    def getmemdump(self, offset:int=0, size:int=None)->bytes:
        sz = self._shm.size()
        if offset >= sz:
            return None
        c = sz
        if not (size is None):
            c = size
        if offset + c > sz:
            c = sz - offset
        pmembytes = cast(self._pcontrol, POINTER(c_ubyte*1))
        self._shm.lock()
        b = bytes(cast(pmembytes[offset], POINTER(c_ubyte*c))[0])
        self._shm.unlock()
        return b

    def getlibpath(self)->str:
        """
        @note Since v0.4.4

        @details Returns absolute path for library folder of mbtools server.
        """
        return self._libpath

    def getprojectpath(self)->str:
        """
        @note Since v0.4.4

        @details Returns absolute path to the current project folder
        """
        return self._projectpath

    def getprojectfile(self)->str:
        """
        @note Since v0.4.4

        @details Returns file name of the current project.
        """
        return self._projectfile

    def getname(self)->str:
        """
        @details Returns name of the current device as string.
        """
        return self._name
    
    def getflags(self)->int:
        """
        @details Returns bit flags of the current device as integer.
        """
        self._shm.lock()
        r = self._control.flags
        self._shm.unlock()
        return r

    def getcycle(self)->int:
        """
        @details Returns count of cycles of Modbus Server app synchronizer.
        """
        self._shm.lock()
        r = self._control.cycle
        self._shm.unlock()
        return r

    def getcount0x(self)->int:
        """
        @details Returns count of coils (bits, 0x) of the current device.
        """
        self._shm.lock()
        r = self._control.count0x
        self._shm.unlock()
        return r

    def getcount1x(self)->int:
        """
        @details Returns count of discrete inputs (bits, 1x) of the current device.
        """
        self._shm.lock()
        r = self._control.count1x
        self._shm.unlock()
        return r

    def getcount3x(self)->int:
        """
        @details Returns count of input regs (16-bit words, 3x) of the current device.
        """
        self._shm.lock()
        r = self._control.count3x
        self._shm.unlock()
        return r

    def getcount4x(self)->int:
        """
        @details Returns count of holding regs (16-bit words, 4x) of the current device.
        """
        self._shm.lock()
        r = self._control.count4x
        self._shm.unlock()
        return r

    def getexcstatus(self)->int:
        """
        @details Returns exception status of the current device as integer [0:255].
        """
        ## @cond
        return self._excmem.getuint8(self._excoffset)
        ## @endcond

    def setexcstatus(self, value:int):
        """
        @details Set exception status `value` of the current device as integer [0:255].
        """
        ## @cond
        self._excmem.setuint8(self._excoffset, value)
        ## @endcond
    
    def getbyteorder(self)->int:
        """
        @details Returns byte order id of the current device as integer.
        """
        return self._byteorder
    
    def getregisterorder(self)->int:
        """
        @details Returns register order id of the current device as integer.
        """
        return self._registerorder
    
    def getpycycle(self)->int:
        """
        @details Returns count of cycles since python program started.
        """
        return self._python.getpycycle()
    
    ## @cond
    def _incpycycle(self):
        return self._python.incpycycle()
    ## @endcond

    def getmem0x(self)->_MemoryBlockBits:
        """
        @details Returns object that provide access to device `0x` memory.
        """
        return self._mem0x
    
    def getmem1x(self)->_MemoryBlockBits:
        """
        @details Returns object that provide access to device `1x` memory.
        """
        return self._mem1x

    def getmem3x(self)->_MemoryBlockRegs:
        """
        @details Returns object that provide access to device `3x` memory.
        """
        return self._mem3x
    
    def getmem4x(self)->_MemoryBlockRegs:
        """
        @details Returns object that provide access to device `4x` memory.
        """
        return self._mem4x
    
    def getint8(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return int8 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getint8(madr.offset())

    def setint8(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set int8 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value int8 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setint8(madr.offset(), value)

    def getuint8(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return uint8 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getuint8(madr.offset())

    def setuint8(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set uint8 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value uint8 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setuint8(madr.offset(), value)

    def getint16(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return int16 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getint16(madr.offset())

    def setint16(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set int16 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value int16 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setint16(madr.offset(), value)

    def getuint16(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return uint16 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getuint16(madr.offset())
    
    def setuint16(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set uint16 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value uint16 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setuint16(madr.offset(), value)

    def getint32(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return int32 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getint32(madr.offset())

    def setint32(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set int32 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value int32 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setint32(madr.offset(), value)

    def getuint32(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return uint32 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getuint32(madr.offset())
    
    def setuint32(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set uint32 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value uint32 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setuint32(madr.offset(), value)

    def getint64(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return int64 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getint64(madr.offset())

    def setint64(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set int64 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value int64 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setint64(madr.offset(), value)

    def getuint64(self, adr)->int:
        """
        @note Since v0.4.4

        @details Return uint64 value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getuint64(madr.offset())
    
    def setuint64(self, adr, value:int):
        """
        @note Since v0.4.4

        @details Set uint64 value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value uint64 value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setuint64(madr.offset(), value)

    def getfloat(self, adr)->float:
        """
        @note Since v0.4.4

        @details Return float value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getfloat(madr.offset())

    def setfloat(self, adr, value:float):
        """
        @note Since v0.4.4

        @details Set float value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value float value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setfloat(madr.offset(), value)

    def getdouble(self, adr)->float:
        """
        @note Since v0.4.4

        @details Return double value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getdouble(madr.offset())
    
    def setdouble(self, adr, value:float):
        """
        @note Since v0.4.4

        @details Set double value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value double value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setdouble(madr.offset(), value)

    def getstring(self, adr)->str:
        """
        @note Since v0.4.4

        @details Return string value from memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        return self._memdict[madr.type()].getstring(madr.offset())
    
    def setstring(self, adr, value:str):
        """
        @note Since v0.4.4

        @details Set string value into memory defined by address `adr`

        @param[in]  adr   address of memory. Can have type modbus.Address or int/str representation of address
        @param[in]  value string value to set
        """
        madr = adr if isinstance(adr, modbus.Address) else modbus.Address(adr)
        self._memdict[madr.type()].setstring(madr.offset(), value)


## @cond
class _MemoryPythonBlock:
    def __init__(self, shmid:str):
        shm = QSharedMemory(shmid)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{shmid}'")
        qptr = shm.data()
        self._memsize = shm.size()
        memptr = c_void_p(qptr.__int__())
        pcontrol = cast(memptr, POINTER(CPythonBlock))
        self._shm = shm
        self._pcontrol = pcontrol
        self._control = pcontrol.contents
        self._cyclecounter = 0

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def getpycycle(self):
        self._shm.lock()
        r = self._control.pycycle
        self._shm.unlock()
        return r

    def incpycycle(self):
        self._cyclecounter += 1
        self._shm.lock()
        self._control.pycycle = self._cyclecounter
        self._shm.unlock()
## @endcond

