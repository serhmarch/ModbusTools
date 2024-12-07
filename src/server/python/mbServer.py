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
        self._shm = shm
        self._mem = cast(memptr, POINTER(c_ushort))
        self._count = count if count <= regsize else regsize

    def __del__(self):
        try:
            self._shm.detach()
        except RuntimeError:
            pass
    
    def getuint16(self, offset:int)->int:
        if 0 <= offset < self._count:
            self._shm.lock()
            r = self._mem[offset]
            self._shm.unlock()
            return r
        return 0

    def setuint16(self, offset:int, value:int)->int:
        if 0 <= offset < self._count:
            self._shm.lock()
            self._mem[offset] = value
            self._shm.unlock()