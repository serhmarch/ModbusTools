"""@package docstring
Documentation for this module.

More details.
"""

#from typing import Union

from PyQt5.QtCore import QSharedMemory

from ctypes import *
import struct

MB_DATAORDER_DEFAULT      = -1
MB_DATAORDER_LITTLEENDIAN = 0
MB_DATAORDER_BIGENDIAN    = 1

MB_REGISTERORDER_DEFAULT  = -1
MB_REGISTERORDER_R0R1R2R3 = 0
MB_REGISTERORDER_R3R2R1R0 = 1
MB_REGISTERORDER_R1R0R3R2 = 2
MB_REGISTERORDER_R2R3R0R1 = 3

class CDeviceBlock(Structure): 
    _fields_ = [("flags"             , c_ulong),
                ("cycle"             , c_ulong),
                ("count0x"           , c_ulong),
                ("count1x"           , c_ulong),
                ("count3x"           , c_ulong),
                ("count4x"           , c_ulong),
                ("exceptionStatusRef", c_ulong),
                ("byteOrder"         , c_long ),
                ("registerOrder"     , c_long ),
                ("stoDeviceName"     , c_ulong),
                ("stringTableSize"   , c_ulong)]

class CPythonBlock(Structure): 
    _fields_ = [("pycycle"           , c_ulong)]

class CMemoryBlockHeader(Structure): 
    _fields_ = [("changeCounter"     , c_ulong),
                ("changeByteOffset"  , c_ulong),
                ("changeByteCount"   , c_ulong),
                ("dummy"             , c_ulong)]


class _MbDevice:
    def __init__(self, memidprefix:str):
        memid_device = memidprefix + ".device"
        memid_python = memidprefix + ".python"
        memid_mem0x  = memidprefix + ".mem0x"
        memid_mem1x  = memidprefix + ".mem1x"
        memid_mem3x  = memidprefix + ".mem3x"
        memid_mem4x  = memidprefix + ".mem4x"
        shm = QSharedMemory(memid_device)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{memid_device}'")
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
        self._python = _MemoryPythonBlock(memid_python)
        self._mem0x  = _MemoryBlockBits(memid_mem0x, self._count0x)
        self._mem1x  = _MemoryBlockBits(memid_mem1x, self._count1x)
        self._mem3x  = _MemoryBlockRegs(memid_mem3x, self._count3x)
        self._mem4x  = _MemoryBlockRegs(memid_mem4x, self._count4x)
        # Exception status
        memtype = self._excstatusref // 10000
        self._excoffset = (self._excstatusref % 10000) - 1
        if   memtype == 0:
            self._excmem = self._mem0x
        elif memtype == 1:
            self._excmem = self._mem1x
        elif memtype == 3:
            self._excmem = self._mem3x
        elif memtype == 4:
            self._excmem = self._mem4x
        else:
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
    
    def getname(self)->str:
        return self._name
    
    def getflags(self):
        self._shm.lock()
        r = self._control.flags
        self._shm.unlock()
        return r

    def getcycle(self):
        self._shm.lock()
        r = self._control.cycle
        self._shm.unlock()
        return r

    def getcount0x(self):
        self._shm.lock()
        r = self._control.count0x
        self._shm.unlock()
        return r

    def getcount1x(self):
        self._shm.lock()
        r = self._control.count1x
        self._shm.unlock()
        return r

    def getcount3x(self):
        self._shm.lock()
        r = self._control.count3x
        self._shm.unlock()
        return r

    def getcount4x(self):
        self._shm.lock()
        r = self._control.count4x
        self._shm.unlock()
        return r

    def getexcstatus(self)->int:
        return self._excmem.getuint8(self._excoffset)

    def setexcstatus(self, value:int)->None:
        self._excmem.setuint8(self._excoffset, value)
    
    def getbyteorder(self):
        return self._byteorder
    
    def getregisterorder(self):
        return self._registerorder
    
    def getpycycle(self):
        return self._python.getpycycle()
    
    def _incpycycle(self):
        return self._python.incpycycle()
    
    def getmem0x(self):
        return self._mem0x
    
    def getmem1x(self):
        return self._mem1x

    def getmem3x(self):
        return self._mem3x
    
    def getmem4x(self):
        return self._mem4x
    

class _MemoryPythonBlock:
    def __init__(self, memid:str):
        shm = QSharedMemory(memid)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{memid}'")
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


class _MemoryBlock:
    """Base class for the memory objects mem0x, mem1x, mem3x, mem4x.

       Class is abstract (can't be used directly). 
    """
    def __init__(self, memid:str, bytecount:int):
        shm = QSharedMemory(memid)
        res = shm.attach()
        if not res:
            raise RuntimeError(f"Cannot attach to Shared Memory with id = '{memid}'")
        qptr = shm.data()
        memptr = c_void_p(qptr.__int__())
        sz = shm.size()
        cbytes = bytecount if bytecount <= sz else sz
        self._shm = shm
        self._countbytes = cbytes
        ptrhead = cast(memptr, POINTER(CMemoryBlockHeader))
        self._head = ptrhead[0]
        self._pmembytes = cast(byref(ptrhead[1]),POINTER(c_ubyte*1))
        self._pmaskbytes = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_ubyte*1))

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def _recalcheader(self, byteoffset:int, bytecount:int)->None:
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

    def _getbytes(self, byteoffset:int, count:int, bytestype=bytes)->bytes:
        if 0 <= byteoffset < self._countbytes:
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            r = bytestype(cast(self._pmembytes[byteoffset], POINTER(c_ubyte*c))[0])
            self._shm.unlock()
            return r
        return bytestype()

    def getbytes(self, byteoffset:int, count:int)->bytes:
        """
        @details Function returns `bytes` object from device memory  starting with `byteoffset` and `count` bytes.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  bitoffset   Byte offset (0-based).
        @param[in]  bitcount    Count of bytes to read.
        """
        return self._getbytes(byteoffset, count, bytes)

    def getbytearray(self, byteoffset:int, count:int)->bytearray:
        """
        @details 
        Function returns `bytearray` object from device memory  starting with `byteoffset` and `count` bytes.
        Parameters are the same as getbytes() function.

        @sa getbytes()
        """
        return self._getbytes(byteoffset, count, bytearray)

    def setbytes(self, byteoffset:int, value:type[bytes|bytearray])->None:
        """
        @details 
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `byteoffset`.

        For bit memory `byteoffset` is calculated as `bitoffset // 8`.
        So bit with offset 0 is in byte with offset 0, bit with offset 8 is in byte with offset 1, etc.
        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  bitoffset   Byte offset (0-based).
        """
        if 0 <= byteoffset < self._countbytes:
            count = len(value)
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            memmove(self._pmembytes[byteoffset], value, c)
            memset(self._pmaskbytes[byteoffset], -1, c)
            self._recalcheader(byteoffset, c)
            self._shm.unlock()

    def getbitbytearray(self, bitoffset:int, bitcount:int)->bytearray:
        """
        @details 
        Function returns `bytearray` object from device memory starting with `bitoffset` and `count` bits.
        Parameters are the same as getbitbytes() function.

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

    def setbitbytes(self, bitoffset:int, bitcount:int, value:type[bytes|bytearray])->None:
        """
        @details
        Function set `value` (`bytes` or `bytearray`) array object into device memory
        starting with `bitoffset` and `count` bits.

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
                b = int.from_bytes([byarray[i], byarray[i+1]], byteorder='little')
                b &= notmask
                b |= v
                tb = b.to_bytes(2, byteorder='little')
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
                b = int.from_bytes([byarray[c], byarray[c+1]], byteorder='little')
                b &= notmask
                b |= v
                tb = b.to_bytes(2, byteorder='little')
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
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            vbyte = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return (vbyte & (1 << bitoffset % 8)) != 0
        return False

    def setbit(self, bitoffset:int, value:bool)->None:
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


class _MemoryBlockBits(_MemoryBlock):
    """Class for the bit memory objects: mem0x, mem1x.

       More details. 
    """
    def __init__(self, memid:str, count:int):
        super().__init__(memid, (count+7)//8)
        c = self._countbytes * 8
        self._count = count if count <= c else c

    def __getitem__(self, index:int)->int:
        """
        @details
        Same as getbit() function but generates `IndexError` if `index` is out of range.
        """
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.getbit(index)
    
    def __setitem__(self, index:int, value:int)->None:
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
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint8(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [-128:127] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).

        @note If `bitoffset` is out of range, function returns `0`.
        """
        if 0 <= bitoffset < self._count-7:
            b = value.to_bytes(1, 'little', signed=True)
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
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint8(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:255] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-7:
            b = value.to_bytes(1, 'little', signed=False)
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
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint16(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [-32768:32767] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-15:
            b = value.to_bytes(2, 'little', signed=True)
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
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint16(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:65535] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-15:
            b = value.to_bytes(2, 'little', signed=False)
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
            b = self.getbitbytearray(bitoffset, 32)
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint32(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [-2147483648:2147483647] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = value.to_bytes(4, 'little', signed=True)
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
            b = self.getbitbytearray(bitoffset, 32)
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint32(self, bitoffset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:4294967295] into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = value.to_bytes(4, 'little', signed=False)
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
            b = self.getbitbytearray(bitoffset, 64)
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint64(self, bitoffset:int, value:int)->None:
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
            b = value.to_bytes(8, 'little', signed=True)
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
            b = self.getbitbytearray(bitoffset, 64)
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint64(self, bitoffset:int, value:int)->None:
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
            b = value.to_bytes(8, 'little', signed=False)
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
            b = self.getbitbytearray(bitoffset, 32)
            return struct.unpack('<f', b)
        return 0.0
    
    def setfloat(self, bitoffset:int, value:float)->None:
        """
        @details
        Function set float 32-bit value into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-31:
            b = struct.pack('<f', value)
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
            b = self.getbitbytearray(bitoffset, 64)
            return struct.unpack('<d', b)
        return 0.0
    
    def setdouble(self, bitoffset:int, value:float)->None:
        """
        @details
        Function set float 64-bit value into device memory starting with `bitoffset`.

        For bit memory `bitoffset` is offset of the bit/coil.
        
        @param[in]  bitoffset   Bit offset (0-based).
        @param[in]  value       Value to write.

        @note If `bitoffset` is out of range, function does nothing.
        """
        if 0 <= bitoffset < self._count-63:
            b = struct.pack('<d', value)
            self.setbitbytes(bitoffset, 64, b)


class _MemoryBlockRegs(_MemoryBlock):
    """Class for the register memory objects: mem3x, mem4x.

       More details. 
    """
    def __init__(self, memid:str, count:int):
        super().__init__(memid, count*2)
        c = self._countbytes // 2
        self._count = count if count <= c else c
        self._pmem = cast(self._pmembytes,POINTER(c_ushort*1))
        self._pmask = cast(self._pmaskbytes,POINTER(c_ushort*1))

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
    
    def getint8(self, byteoffset:int)->int:
        """
        @details
        Function returns integer value of [-128:127] from device memory starting with `byteoffset`.

        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).

        @note If `byteoffset` is out of range, function returns `0`.
        """
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            r = cast(self._pmembytes[byteoffset], POINTER(c_byte))[0]
            self._shm.unlock()
            return r
        return 0

    def setint8(self, byteoffset:int, value:int)->None:
        """
        @details
        Function set value of [-128:127] into device memory starting with `byteoffset`.

        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).

        @note If `byteoffset` is out of range, function does nothing.
        """
        self.setuint8(byteoffset, value)

    def getuint8(self, byteoffset:int)->int:
        """
        @details
        Function returns integer value of [0:255] from device memory starting with `byteoffset`.

        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).

        @note If `byteoffset` is out of range, function returns `0`.
        """
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            r = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint8(self, byteoffset:int, value:int)->None:
        """
        @details
        Function set value of [0:255] into device memory starting with `byteoffset`.

        For register memory `byteoffset` is calculated as `offset * 2`.
        So register with offset 0 is byte offset 0 and 1, 
        register with offset 1 is byte offset 2 and 3, etc.
        
        @param[in]  byteoffset  Byte offset (0-based).

        @note If `byteoffset` is out of range, function does nothing.
        """
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            self._pmembytes [byteoffset][0] = value
            self._pmaskbytes[byteoffset][0] = 0xFF
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()    
            
    def getint16(self, offset:int)->int:
        """
        @details
        Function returns integer value of [-32768:32767] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_short))[0]
            self._shm.unlock()
            return r
        return 0

    def setint16(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [-32768:32767] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        self.setuint16(offset, value)

    def getuint16(self, offset:int)->int:
        """
        @details
        Function returns integer value of [0:65535] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count:
            self._shm.lock()
            r = self._pmem[offset][0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint16(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:65535] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count:
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
            r = cast(self._pmem[offset], POINTER(c_long))[0]
            self._shm.unlock()
            return r
        return 0

    def setint32(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [-2147483648:2147483647] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        self.setuint32(offset, value)

    def getuint32(self, offset:int)->int:
        """
        @details
        Function returns integer value of [0:4294967295] from device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function returns `0`.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_ulong))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint32(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:4294967295] into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_ulong))[0] = value
            cast(self._pmask[offset], POINTER(c_ulong))[0] = 0xFFFFFFFF
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
            r = cast(self._pmem[offset], POINTER(c_longlong))[0]
            self._shm.unlock()
            return r
        return 0

    def setint64(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [-9223372036854775808:9223372036854775807]
        into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        self.setuint64(offset, value)

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
            r = cast(self._pmem[offset], POINTER(c_ulonglong))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint64(self, offset:int, value:int)->None:
        """
        @details
        Function set integer value of [0:18446744073709551615]
        into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-3:
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
            r = cast(self._pmem[offset], POINTER(c_float))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setfloat(self, offset:int, value:float)->None:
        """
        @details
        Function set float 32-bit value into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-1:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_float))[0] = value
            cast(self._pmask[offset], POINTER(c_ulong))[0] = 0xFFFFFFFF
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
            r = cast(self._pmem[offset], POINTER(c_double))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setdouble(self, offset:int, value:float)->None:
        """
        @details
        Function set float 64-bit value into device memory starting with `offset`.

        @param[in]  offset   Offset of the first register (0-based).

        @note If `offset` is out of range, function does nothing.
        """
        if 0 <= offset < self._count-3:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_double))[0] = value
            cast(self._pmask[offset], POINTER(c_ulonglong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()
