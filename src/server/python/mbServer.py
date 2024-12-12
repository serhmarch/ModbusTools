import time

from PyQt5.QtCore import QSystemSemaphore, QSharedMemory

from ctypes import *

class CControlBlock(Structure): 
    _fields_ = [("flags"  , c_ulong),
                ("count0x", c_ulong),
                ("count1x", c_ulong),
                ("count3x", c_ulong),
                ("count4x", c_ulong),
                ("cycle"  , c_ulong),
                ("pycycle", c_ulong)]

class CMemoryBlockHeader(Structure): 
    _fields_ = [("changeCounter"    , c_ulong),
                ("changeByteOffset" , c_ulong),
                ("changeByteCount"  , c_ulong),
                ("dummy"            , c_ulong)]


class _MemoryControlBlock:
    def __init__(self, memid:str):
        shm = QSharedMemory(memid)
        res = shm.attach()
        qptr = shm.data()
        size = shm.size()
        memptr = c_void_p(qptr.__int__())
        pcontrol = cast(memptr, POINTER(CControlBlock))
        self._shm = shm
        self._pcontrol = pcontrol
        self._control = pcontrol.contents

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def getflags(self):
        self._shm.lock()
        r = self._control.flags
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

    def getcycle(self):
        self._shm.lock()
        r = self._control.cycle
        self._shm.unlock()
        return r

    def getpycycle(self):
        self._shm.lock()
        r = self._control.pycycle
        self._shm.unlock()
        return r

    def setpycycle(self, value):
        self._shm.lock()
        self._control.pycycle = value
        self._shm.unlock()


class _MemoryBlock:
    def __init__(self, memid:str, bytecount:int):
        self._countbytes = bytecount
        shm = QSharedMemory(memid)
        res = shm.attach()
        qptr = shm.data()
        memptr = c_void_p(qptr.__int__())
        cbytes = self._countbytes
        self._shm = shm
        ptrhead = cast(memptr, POINTER(CMemoryBlockHeader))
        self._head = ptrhead[0]
        self._pmembytes = cast(byref(ptrhead[1]),POINTER(c_ubyte*1))
        self._pmaskbytes  = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_ubyte*1))

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
                self._head.changeByteCount = self._head.changeByteOffset - byteoffset
            self._head.changeByteOffset = byteoffset
        if self._head.changeByteOffset + self._head.changeByteCount < rightedge:
            self._head.changeByteCount = rightedge - self._head.changeByteOffset
        self._head.changeCounter += 1

    def _getbytes(self, byteoffset:int, count:int, bytetype=bytes)->bytes:
        if 0 <= byteoffset < self._countbytes:
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            r = bytetype(cast(self._pmembytes[byteoffset], POINTER(c_ubyte*c))[0])
            self._shm.unlock()
            return r
        return bytetype()

    def getbytes(self, byteoffset:int, count:int)->bytes:
        if 0 <= byteoffset < self._countbytes:
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            r = bytes(cast(self._pmembytes[byteoffset], POINTER(c_ubyte*c))[0])
            self._shm.unlock()
            return r
        return bytes()

    def setbytes(self, byteoffset:int, value:bytes)->None:
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
        byteoffset = bitoffset // 8
        rbyteoffset = (bitoffset+bitcount-1) // 8
        bytecount = rbyteoffset-byteoffset+1
        byarray = self._getbytes(byteoffset, bytecount, bytearray)
        shift = bitoffset % 8
        rem = bitcount % 8
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
            b = byarray[-1]
            b = (b >> shift) & mask
            byarray[-1] = b
        return byarray

    def getbitbytes(self, bitoffset:int, bitcount:int)->bytes:
        return bytes(self.getbitbytearray(bitoffset, bitcount))

    def setbitbytes(self, bitoffset:int, bitcount:int, value:bytes)->None:
        byteoffset = bitoffset // 8
        rbyteoffset = (bitoffset+bitcount-1) // 8
        bytecount = rbyteoffset-byteoffset+1
        byarray = self._getbytes(byteoffset, bytecount, bytearray)
        shift = bitoffset % 8
        c = bitcount // 8 #len(byarray)-1
        rem = bitcount % 8
        if shift:
            mask = 0xFF << shift
            notmask = ~mask
            for i in range(c):
                v = value[i] << shift
                b = int.from_bytes([byarray[i], byarray[i+1]], byteorder='little')
                b &= notmask
                b |= v
                tb = b.to_bytes(byteorder='little')
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
                tb = b.to_bytes(byteorder='little')
                byarray[c]   = tb[0]
                byarray[c+1] = tb[1]
            else:
                b = byarray[c] & notmask
                b |= v
                byarray[c] = b
        self.setbytes(byteoffset, byarray)

    def getbit(self, bitoffset:int)->int:
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            vbyte = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return (vbyte & (1 << bitoffset % 8)) != 0
        return 0

    def setbit(self, bitoffset:int, value:bool)->int:
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            if value:
                self._pmembytes[byteoffset][0] |= (1 << bitoffset % 8)
            else:
                self._pmembytes[byteoffset][0] &= ~(1 << bitoffset % 8)
            self._pmaskbytes[byteoffset][0] |= (1 << bitoffset % 8)
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()

class _MemoryBlockBits(_MemoryBlock):
    def __init__(self, memid:str, count:int):
        super().__init__(memid, (count+7)//8)
        self._count = count

    def __getitem__(self, index:int)->int:
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.getbit(index)
    
    def __setitem__(self, index:int, value:int)->None:
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        self.setbit(index, value)
    
    def getint8(self, bitoffset:int)->int:
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 8)
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint8(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-7:
            b = value.to_bytes(1, 'little', signed=True)

    def getuint8(self, bitoffset:int)->int:
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 8)
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint8(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-7:
            b = value.to_bytes(1, 'little', signed=False)

    def getint16(self, bitoffset:int)->int:
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 16)
            return int.from_bytes(b, byteorder='little', signed=True)
        return 0
    
    def setint16(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-7:
            b = value.to_bytes(1, 'little', signed=True)

    def getuint16(self, bitoffset:int)->int:
        if 0 <= bitoffset < self._count-7:
            b = self.getbitbytearray(bitoffset, 16)
            return int.from_bytes(b, byteorder='little', signed=False)
        return 0
    
    def setuint16(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-7:
            b = value.to_bytes(1, 'little', signed=False)

class _MemoryBlockRegs:
    def __init__(self, memid:str, count:int):
        shm = QSharedMemory(memid)
        res = shm.attach()
        qptr = shm.data()
        regsize = shm.size() // 2 
        memptr = c_void_p(qptr.__int__())
        c = count if count <= regsize else regsize
        cbytes = c * 2
        self._count = c
        self._countbytes = c * 2
        self._shm = shm
        ptrhead = cast(memptr, POINTER(CMemoryBlockHeader))
        self._head = ptrhead[0]
        self._pmem      = cast(byref(ptrhead[1]),POINTER(c_ushort*1))
        self._pmembytes = cast(byref(ptrhead[1]),POINTER(c_ubyte*1))
        self._pmask       = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_ushort*1))
        self._pmaskbytes  = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_ubyte*1))
        #ptrmask = cast(byref(cast(byref(ptrhead[1]),POINTER(c_byte*cbytes))[1]),POINTER(c_byte))

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
                self._head.changeByteCount = self._head.changeByteOffset - byteoffset
            self._head.changeByteOffset = byteoffset
        if self._head.changeByteOffset + self._head.changeByteCount < rightedge:
            self._head.changeByteCount = rightedge - self._head.changeByteOffset
        self._head.changeCounter += 1

    def __getitem__(self, index:int)->int:
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.getuint16(index)
    
    def __setitem__(self, index:int, value:int)->int:
        if index < 0 or index >= self._count:
            raise IndexError("Memory index out of range")
        return self.setuint16(index, value)
    
    def getbytes(self, byteoffset:int, count:int)->bytes:
        if 0 <= byteoffset < self._countbytes:
            if byteoffset+count > self._countbytes:
                c = self._countbytes - byteoffset
            else:
                c = count
            self._shm.lock()
            r = bytes(cast(self._pmembytes[byteoffset], POINTER(c_ubyte*c))[0])
            self._shm.unlock()
            return r
        return bytes()

    def setbytes(self, byteoffset:int, value:bytes)->None:
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


    def getbit(self, bitoffset:int)->int:
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            vbyte = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return (vbyte & (1 << bitoffset % 8)) != 0
        return 0

    def setbit(self, bitoffset:int, value:bool)->None:
        byteoffset = bitoffset // 8
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            if value:
                self._pmembytes[byteoffset][0] |= (1 << bitoffset % 8)
            else:
                self._pmembytes[byteoffset][0] &= ~(1 << bitoffset % 8)
            self._pmaskbytes[byteoffset][0] |= (1 << bitoffset % 8)
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()

    def getint8(self, byteoffset:int)->int:
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            r = cast(self._pmembytes[byteoffset], POINTER(c_byte))[0]
            self._shm.unlock()
            return r
        return 0

    def setint8(self, byteoffset:int, value:int)->None:
        self.setuint8(byteoffset, value)

    def getuint8(self, byteoffset:int)->int:
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            r = self._pmembytes[byteoffset][0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint8(self, byteoffset:int, value:int)->None:
        if 0 <= byteoffset < self._countbytes:
            self._shm.lock()
            self._pmembytes [byteoffset][0] = value
            self._pmaskbytes[byteoffset][0] = 0xFF
            self._recalcheader(byteoffset, 1)
            self._shm.unlock()    
            
    def getint16(self, offset:int)->int:
        if 0 <= offset < self._count:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_short))[0]
            self._shm.unlock()
            return r
        return 0

    def setint16(self, offset:int, value:int)->None:
        self.setuint16(offset, value)

    def getuint16(self, offset:int)->int:
        if 0 <= offset < self._count:
            self._shm.lock()
            r = self._pmem[offset][0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint16(self, offset:int, value:int)->None:
        #print(f"setuint16({offset=}, {value=})")
        if 0 <= offset < self._count:
            self._shm.lock()
            self._pmem [offset][0] = value
            self._pmask[offset][0] = 0xFFFF
            self._recalcheader(offset*2, 2)
            self._shm.unlock()

    def getint32(self, offset:int)->int:
        if 0 <= offset < self._count-1:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_long))[0]
            self._shm.unlock()
            return r
        return 0

    def setint32(self, offset:int, value:int)->None:
        self.setuint32(offset, value)

    def getuint32(self, offset:int)->int:
        if 0 <= offset < self._count-1:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_ulong))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint32(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-1:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_ulong))[0] = value
            cast(self._pmask[offset], POINTER(c_ulong))[0] = 0xFFFFFFFF
            self._recalcheader(offset*2, 4)
            self._shm.unlock()

    def getint64(self, offset:int)->int:
        if 0 <= offset < self._count-3:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_longlong))[0]
            self._shm.unlock()
            return r
        return 0

    def setint64(self, offset:int, value:int)->None:
        self.setuint64(offset, value)

    def getuint64(self, offset:int)->int:
        if 0 <= offset < self._count-3:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_ulonglong))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setuint64(self, offset:int, value:int)->None:
        if 0 <= offset < self._count-3:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_ulonglong))[0] = value
            cast(self._pmask[offset], POINTER(c_ulonglong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()

    def getfloat(self, offset:int)->int:
        if 0 <= offset < self._count-1:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_float))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setfloat(self, offset:int, value:float)->None:
        if 0 <= offset < self._count-1:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_float))[0] = value
            cast(self._pmask[offset], POINTER(c_ulong))[0] = 0xFFFFFFFF
            self._recalcheader(offset*2, 4)
            self._shm.unlock()

    def getdouble(self, offset:int)->int:
        if 0 <= offset < self._count-3:
            self._shm.lock()
            r = cast(self._pmem[offset], POINTER(c_double))[0]
            self._shm.unlock()
            return r
        return 0
    
    def setdouble(self, offset:int, value:float)->None:
        if 0 <= offset < self._count-3:
            self._shm.lock()
            cast(self._pmem [offset], POINTER(c_double))[0] = value
            cast(self._pmask[offset], POINTER(c_ulonglong))[0] = 0xFFFFFFFFFFFFFFFF
            self._recalcheader(offset*2, 8)
            self._shm.unlock()
