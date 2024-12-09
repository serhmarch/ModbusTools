import time

from PyQt5.QtCore import QSystemSemaphore, QSharedMemory

from ctypes import *

#shm = QSharedMemory("ModbusTools.Server.PORT1.PLC1")
#res=shm.attach()
#qptr=shm.data()
#memsz=shm.size()
#cptr=c_void_p(qptr.__int__())
#arr=ptr.asarray() # sip.array(unsigned char, <len>)
#vInt = arr[3]<<24|arr[2]<<16|arr[1]<<8|arr[0]
#class MemoryHeader(Structure):
#    _fields_ = [("flags"  , c_ulong),
#                ("memsize", c_ulong),
#                ("cycle"  , c_ulong),
#                ("pycycle", c_ulong)]
#                
#pheader = cast(cptr, POINTER(MemoryHeader))
#header = pheader[0]
#arr = cast(byref(pheader[1]), POINTER(c_ushort))

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

#__P__ControlBlock = POINTER(__ControlBlock)

class __MemoryControlBlock:
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


class __MemoryBlock:
    def __init__(self, memid:str, count:int):
        shm = QSharedMemory(memid)
        res = shm.attach()
        qptr = shm.data()
        regsize = shm.size() // 2 
        memptr = c_void_p(qptr.__int__())
        c = count if count <= regsize else regsize
        self._shm = shm
        ptrhead = cast(memptr, POINTER(CMemoryBlockHeader))
        self._head = ptrhead[0]
        ptrmem = cast(byref(ptrhead[1]),POINTER(c_ushort*c))
        self._mem  = ptrmem[0]
        ptrmask = cast(byref(ptrmem[1]),POINTER(c_ushort*c))
        self._mask  = ptrmask[0]
        self._count = c

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

    def getuint16(self, offset:int)->int:
        if 0 <= offset < self._count:
            self._shm.lock()
            r = self._mem[offset]
            self._shm.unlock()
            return r
        return 0

    def setuint16(self, offset:int, value:int)->None:
        print(f"setuint16({offset=}, {value=})")
        if 0 <= offset < self._count:
            self._shm.lock()
            self._mem [offset] = value
            self._mask[offset] = 0xFFFF
            self._recalcheader(offset*2, 2)
            self._shm.unlock()