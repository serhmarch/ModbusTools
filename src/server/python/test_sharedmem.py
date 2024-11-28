#!/usr/bin/python

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


class __Memory:
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


sMemPrefix = "ModbusTools.Server.PORT1.PLC1."
sMemControl = sMemPrefix+"control"
sMem4x = sMemPrefix+"mem4x"

ctrl = __MemoryControlBlock(sMemControl)
mem4x = __Memory(sMem4x, ctrl.getcount4x())

#############################################
########### USER CODE: INITIALIZE ###########
#############################################

from os import sys, path

from datetime import datetime, timedelta
from random import randint
from time import sleep
from configparser import RawConfigParser

COMMBUFF_HEADER_ID_STATUS   = 0
COMMBUFF_HEADER_EV_COUNT    = 1
COMMBUFF_HEADER_SIZE        = 2

COMMBUFF_BLOCK_ID_INIT      = 255

INIT_DATETIME_BUFFER_IDX_DAY_OF_WEEK = 0
INIT_DATETIME_BUFFER_IDX_MONTH       = 1
INIT_DATETIME_BUFFER_IDX_DAY         = 2
INIT_DATETIME_BUFFER_IDX_YEAR        = 3
INIT_DATETIME_BUFFER_IDX_HOUR        = 4
INIT_DATETIME_BUFFER_IDX_MINUTE      = 5
INIT_DATETIME_BUFFER_IDX_SECOND      = 6

COMMAND_COMMBUFF_ACK        = 0x0000
COMMAND_INITBUFF_REQUEST    = 0x0001

EVENT_ANALOG_VALUE          = 0
EVENT_SUBSYS                = 1
EVENT_VALVE                 = 2
EVENT_ANALOG_PARAM          = 3

SERVICE_EVENT_COMMBUFF_FULL = 0
SERVICE_EVENT_INITBUFF_READ = 1
SERVICE_EVENT_PLC_RESTARTED = 2
SERVICE_EVENT_CLOCK_00      = 3
SERVICE_EVENT_CLOCK_12      = 4

VALVE_INDEX_SPECIAL = 255

CFG_DEFAULT_SubSystemTableCount         = 32
CFG_DEFAULT_SubSystemTableSize          = 16
CFG_DEFAULT_ValveCount                  = 127
CFG_DEFAULT_AnalogCount                 = 127
CFG_DEFAULT_AnalogEnableValue           = True
CFG_DEFAULT_CmdInitOffset               = 7385     
CFG_DEFAULT_CmdAckOffset                = 7386  
CFG_DEFAULT_EventBufferSize             = 1000
CFG_DEFAULT_CommBuffOffset              = 7139   
CFG_DEFAULT_CommBuffCount               = 100
CFG_DEFAULT_CommBuffEnableHeartBeat     = True 
CFG_DEFAULT_DataSubSystemTableOffset    = 2000
CFG_DEFAULT_DataValveOffset             = 2500
CFG_DEFAULT_DataAnalogOffset            = 3000
CFG_DEFAULT_InitDateTimeOffset          = 5340
CFG_DEFAULT_InitAnalogOffset            = 5347
CFG_DEFAULT_InitSubSystemTableOffset    = 5475
CFG_DEFAULT_InitValveOffset             = 5987

def subsysStatus(index, bitNum, bitValue):
    return((EVENT_SUBSYS  << 14) |
           ((bitValue&1)  << 13) |
           ((index&0x1F)  <<  8) |
           (bitNum&0xFF))
    
def valveStatus(index, bitNum, bitValue):
    return((EVENT_VALVE   << 14) |
           ((bitValue&1)  << 13) |
           ((bitNum&0x1F) <<  8) |
           (index&0xFF))
    
def analogStatus(index, bitNum, bitValue):
    return((EVENT_ANALOG_PARAM  << 14) |
           ((bitValue&1)        << 13) |
           ((bitNum&0x1F)       <<  8) |
           (index&0xFF))
    
def analogValue(value):
    return((EVENT_ANALOG_VALUE << 14) |
           (value&0x3FFF))

def currentTimestamp():
    d = datetime.now()
    if d.hour < 12:
        return int(timedelta(hours=d.hour, minutes=d.minute, seconds=d.second).total_seconds())
    else:
        return int(timedelta(hours=d.hour-12, minutes=d.minute, seconds=d.second).total_seconds())

class ModbusAddress:
    def __init__(self, mem = 4, offset = 0):
        self.mem = mem
        self.offset = offset
        
def toModbusAddress(adr):
    if adr < 400001 or adr > 465536:
        raise ValueError(f"Modbus Address '{adr}' not supported")
    return ModbusAddress(adr / 100000, adr - 400001)
        
class Event:
    def __init__(self, id=0, timestamp=0):
        self.id = id
        self.timestamp = timestamp

class EventBuffer:
    def __init__(self, buffsize=CFG_DEFAULT_EventBufferSize):
        self._queue = [Event() for _ in range(buffsize)]
        self._ptr = 0
        self._count = 0
        self._sz = len(self._queue)
    
    def count(self):
        return self._count
        
    def clear(self):
        self._count = 0
        
    def pushSubSystemStatus(self, index, bitNum, value):
        id = subsysStatus(index, bitNum, value)
        timestamp = currentTimestamp()
        self.push(id, timestamp)
        
    def pushValveStatus(self, index, bitNum, value):
        id = valveStatus(index, bitNum, value)
        timestamp = currentTimestamp()
        self.push(id, timestamp)
        
    def pushAnalogStatus(self, index, bitNum, value):
        id = analogStatus(index, bitNum, value)
        timestamp = currentTimestamp()
        self.push(id, timestamp)
        
    def pushAnalogValue(self, index, value):
        # TODO: push pre-status event with index-param
        #id = analogValue(index, bitNum, value)
        #timestamp = currentTimestamp()
        #self.push(id, timestamp)
        pass
        
    def push(self, id, timestamp):
        queue = self._queue
        e = queue[self._ptr]
        e.id = id
        e.timestamp = timestamp
        sz = self._sz
        self._ptr = (self._ptr + 1) % sz
        self._count += (self._count < sz)
 
    def pop(self):
        c = self._count
        if c > 0:
            sz = self._sz
            i = (self._ptr - c + sz) % sz
            e = self._queue[i]
            self._count -= 1
            return Event(e.id, e.timestamp)
        return None

class ConfigPLC:
    def __init__(self):
        self.CFG_SubSystemTableCount        = CFG_DEFAULT_SubSystemTableCount     
        self.CFG_SubSystemTableSize         = CFG_DEFAULT_SubSystemTableSize      
        self.CFG_ValveCount                 = CFG_DEFAULT_ValveCount              
        self.CFG_AnalogCount                = CFG_DEFAULT_AnalogCount             
        self.CFG_AnalogEnableValue          = CFG_DEFAULT_AnalogEnableValue       
        self.CFG_CmdInitOffset              = CFG_DEFAULT_CmdInitOffset           
        self.CFG_CmdAckOffset               = CFG_DEFAULT_CmdAckOffset            
        self.CFG_EventBufferSize            = CFG_DEFAULT_EventBufferSize      
        self.CFG_CommBuffOffset             = CFG_DEFAULT_CommBuffOffset         
        self.CFG_CommBuffCount              = CFG_DEFAULT_CommBuffCount          
        self.CFG_CommBuffEnableHeartBeat    = CFG_DEFAULT_CommBuffEnableHeartBeat 
        self.CFG_DataSubSystemTableOffset   = CFG_DEFAULT_DataSubSystemTableOffset
        self.CFG_DataValveOffset            = CFG_DEFAULT_DataValveOffset         
        self.CFG_DataAnalogOffset           = CFG_DEFAULT_DataAnalogOffset        
        self.CFG_InitDateTimeOffset         = CFG_DEFAULT_InitDateTimeOffset      
        self.CFG_InitSubSystemTableOffset   = CFG_DEFAULT_InitAnalogOffset        
        self.CFG_InitValveOffset            = CFG_DEFAULT_InitSubSystemTableOffset
        self.CFG_InitAnalogOffset           = CFG_DEFAULT_InitValveOffset         

#class Config:
#    def __init__(self):
#        self.CFG_Port = None
#        self.PLCs = []
#        
#    def read(self, file):
#        parser = RawConfigParser()
#        parser.read(file)
#        section = "Main"
#        self.CFG_Port = parser.getint(section, "Port")
#        c = parser.getint(section, "PLC")
#        for i in range(c):
#            section = "PLC"+str(i+1)
#            cfg = ConfigPLC()
#            cfg.CFG_SubSystemTableCount         = parser.getint(section, "SubSystemTableCount")
#            cfg.CFG_SubSystemTableSize          = parser.getint(section, "SubSystemTableSize")
#            cfg.CFG_ValveCount                  = parser.getint(section, "ValveCount")
#            cfg.CFG_AnalogCount                 = parser.getint(section, "AnalogCount")
#            cfg.CFG_AnalogEnableValue           = parser.getint(section, "AnalogEnableValue")
#            cfg.CFG_CmdInitOffset               = parser.getint(section, "CmdInitAddress")     
#            cfg.CFG_CmdAckOffset                = parser.getint(section, "CmdAckAddress")     
#            cfg.CFG_CommBuffOffset              = parser.getint(section, "CommBuffAddress")   
#            cfg.CFG_CommBuffCount               = parser.getint(section, "CommBuffCount")  
#            cfg.CFG_CommBuffEnableHeartBeat     = parser.getint(section, "CommBuffEnableHeartBeat")  
#            cfg.CFG_EventBufferSize             = parser.getint(section, "EventBufferSize")  
#            cfg.CFG_DataSubSystemTableOffset    = parser.getint(section, "DataSubSystemTableAddress")
#            cfg.CFG_DataValveOffset             = parser.getint(section, "DataValveAddress")
#            cfg.CFG_DataAnalogOffset            = parser.getint(section, "DataAnalogAddress")
#            cfg.CFG_InitDateTimeOffset          = parser.getint(section, "InitDateTimeAddress")
#            cfg.CFG_InitSubSystemTableOffset    = parser.getint(section, "InitSubSystemTableAddress")
#            cfg.CFG_InitValveOffset             = parser.getint(section, "InitValveAddress")         
#            cfg.CFG_InitAnalogOffset            = parser.getint(section, "InitAnalogAddress")
#            
#            cfg.CFG_CmdInitOffset               = toModbusAddress(cfg.CFG_CmdInitOffset           ).offset 
#            cfg.CFG_CmdAckOffset                = toModbusAddress(cfg.CFG_CmdAckOffset            ).offset 
#            cfg.CFG_CommBuffOffset              = toModbusAddress(cfg.CFG_CommBuffOffset          ).offset 
#            cfg.CFG_DataSubSystemTableOffset    = toModbusAddress(cfg.CFG_DataSubSystemTableOffset).offset 
#            cfg.CFG_DataValveOffset             = toModbusAddress(cfg.CFG_DataValveOffset         ).offset 
#            cfg.CFG_DataAnalogOffset            = toModbusAddress(cfg.CFG_DataAnalogOffset        ).offset 
#            cfg.CFG_InitDateTimeOffset          = toModbusAddress(cfg.CFG_InitDateTimeOffset      ).offset 
#            cfg.CFG_InitSubSystemTableOffset    = toModbusAddress(cfg.CFG_InitSubSystemTableOffset).offset 
#            cfg.CFG_InitValveOffset             = toModbusAddress(cfg.CFG_InitValveOffset         ).offset 
#            cfg.CFG_InitAnalogOffset            = toModbusAddress(cfg.CFG_InitAnalogOffset        ).offset 
#            
#            self.PLCs.append(cfg)


class PLC:
    """Chain PLC class"""
    
    def __init__(self, cfg:ConfigPLC):
        self.Name = "PLC"
        self.CFG_SubSystemTableCount        = cfg.CFG_SubSystemTableCount        
        self.CFG_SubSystemTableSize         = cfg.CFG_SubSystemTableSize         
        self.CFG_ValveCount                 = cfg.CFG_ValveCount        
        self.CFG_AnalogCount                = cfg.CFG_AnalogCount       
        self.CFG_AnalogEnableValue          = cfg.CFG_AnalogEnableValue 
        self.CFG_CmdInitOffset              = cfg.CFG_CmdInitOffset        
        self.CFG_CmdAckOffset               = cfg.CFG_CmdAckOffset
        self.CFG_CommBuffOffset             = cfg.CFG_CommBuffOffset     
        self.CFG_CommBuffCount              = cfg.CFG_CommBuffCount  
        self.CFG_CommBuffEnableHeartBeat    = cfg.CFG_CommBuffEnableHeartBeat  
        self.CFG_EventBufferSize            = cfg.CFG_EventBufferSize 
        self.CFG_DataSubSystemTableOffset   = cfg.CFG_DataSubSystemTableOffset  
        self.CFG_DataValveOffset            = cfg.CFG_DataValveOffset   
        self.CFG_DataAnalogOffset           = cfg.CFG_DataAnalogOffset  
        self.CFG_InitDateTimeOffset         = cfg.CFG_InitDateTimeOffset
        self.CFG_InitSubSystemTableOffset   = cfg.CFG_InitSubSystemTableOffset  
        self.CFG_InitValveOffset            = cfg.CFG_InitValveOffset   
        self.CFG_InitAnalogOffset           = cfg.CFG_InitAnalogOffset
        #----------------------------------------------------------------
        self._ExcStatusMem    = mem4x
        self._ExcStatusOffset = 0
        self.SubSystemRegSize = self.CFG_SubSystemTableCount * self.CFG_SubSystemTableSize
        self.ValveRegSize = self.CFG_ValveCount * 2
        self.AnalogRegSize = self.CFG_AnalogCount
        self.CommBuffEvCount = (self.CFG_CommBuffCount-COMMBUFF_HEADER_SIZE) // 2

        self._eventbuff = EventBuffer(self.CFG_EventBufferSize)
        self._NextBlockId = 0
        self._LastBlockId = 0
        self._LastHour = -1
        self._tsregs = [0] * self.SubSystemRegSize
        self._vsregs = [0] * self.ValveRegSize
        self._asregs = [0] * self.AnalogRegSize
        self._avregs = [0] * self.AnalogRegSize
        
    def printConfig(self):
        print(f"""{self.Name}     
----------------------------------------
{self.CFG_SubSystemTableCount       = }
{self.CFG_SubSystemTableSize        = }
{self.CFG_ValveCount                = }
{self.CFG_AnalogCount               = }
{self.CFG_AnalogEnableValue         = }
{self.CFG_CmdInitOffset             = }     
{self.CFG_CmdAckOffset              = }     
{self.CFG_CommBuffOffset            = }   
{self.CFG_CommBuffCount             = }   
{self.CFG_CommBuffEnableHeartBeat   = }  
{self.CFG_EventBufferSize           = } 
{self.CFG_DataSubSystemTableOffset  = }
{self.CFG_DataValveOffset           = }
{self.CFG_DataAnalogOffset          = }
{self.CFG_InitDateTimeOffset        = }
{self.CFG_InitSubSystemTableOffset  = }
{self.CFG_InitValveOffset           = }
{self.CFG_InitAnalogOffset          = }
----------------------------------------""")
    def getexcstatus(self):
        return self._ExcStatusMem.getuint16(self._ExcStatusOffset)

    def setexcstatus(self, value):
        return self._ExcStatusMem.setuint16(self._ExcStatusOffset, value)

    def run(self):
        self.heartBeat()
        self.checkDateTime()
        self.checkSubSystemChange()
        self.checkValveChange()
        self.checkAnalogChange()
        self.processCommBuff()
   
    def heartBeat(self):
        if self.CFG_CommBuffEnableHeartBeat:
            v = mem4x.getuint16(self.CFG_CommBuffOffset-1)
            v = (v + 1) % 65536
            mem4x.setuint16(self.CFG_CommBuffOffset-1, v)

    def checkDateTime(self):
        d = datetime.now()
        if self._LastHour != d.hour:
            if d.hour == 12:
                self.pushServiceEvent(SERVICE_EVENT_CLOCK_12)
            elif d.hour == 0:
                self.pushServiceEvent(SERVICE_EVENT_CLOCK_00)            
            self._LastHour = d.hour
            
    def checkSubSystemChange(self):
        for i in range(self.SubSystemRegSize):
            table = i // self.CFG_SubSystemTableSize
            reg = i % self.CFG_SubSystemTableSize
            nv = mem4x.getuint16(self.CFG_DataSubSystemTableOffset+i)
            ov = self._tsregs[i]
            self._tsregs[i] = nv
            for j in range(16):
                m = 1 << j
                nb = (nv & m) != 0
                ob = (ov & m) != 0
                if nb != ob:
                    self.pushSubSystemStatus(table, reg*16+15-j, nb)

    def checkValveChange(self):
        for i in range(self.ValveRegSize):
            index = i // 2
            reg = i % 2
            nv = mem4x.getuint16(self.CFG_DataValveOffset+i)
            ov = self._vsregs[i]
            self._vsregs[i] = nv
            for j in range(16):
                m = 1 << j
                nb = (nv & m) != 0
                ob = (ov & m) != 0
                if nb != ob:
                    self.pushValveStatus(index, reg*16+15-j, nb)

    def checkAnalogChange(self):
        # TODO: check AnalogValues
        for i in range(self.AnalogRegSize):
            nv = mem4x.getuint16(self.CFG_DataAnalogOffset+i)
            ov = self._asregs[i]
            self._asregs[i] = nv
            for j in range(16):
                m = 1 << j
                nb = (nv & m) != 0
                ob = (ov & m) != 0
                if nb != ob:
                    self.pushAnalogStatus(i, 15-j, nb)

    def processCommBuff(self):
        cmdInit = mem4x.getuint16(self.CFG_CmdInitOffset) == COMMAND_INITBUFF_REQUEST
        cmdAck  = mem4x.getuint16(self.CFG_CmdAckOffset)  == COMMAND_COMMBUFF_ACK
        blockId = 0
        evcount = 0
        blockStatus = 0
        if cmdInit:
            print(f"{self.Name}.COMMAND_INITBUFF_REQUEST")
            mem4x.setuint16(self.CFG_CmdInitOffset, 0)
            self.fillInitBuff()
            offset = self.CFG_CommBuffOffset + COMMBUFF_HEADER_SIZE
            mem4x.setuint16(offset  , valveStatus(VALVE_INDEX_SPECIAL, SERVICE_EVENT_INITBUFF_READ, 1))
            mem4x.setuint16(offset+1, currentTimestamp())
            evcount = 1
            blockId = COMMBUFF_BLOCK_ID_INIT
            self._NextBlockId = 0
        elif cmdAck:
            #print(f"{self.Name}.COMMAND_COMMBUFF_ACK")
            ebuff = self._eventbuff
            self.setexcstatus(0)
            if ebuff.count() > 0:            
                for i in range(self.CommBuffEvCount):
                    e = ebuff.pop()
                    if not e:
                        break
                    evcount += 1
                    offset = self.CFG_CommBuffOffset + COMMBUFF_HEADER_SIZE + i*2
                    mem4x.setuint16(offset, e.id)
                    mem4x.setuint16(offset+1, e.timestamp)        
                blockId = self._NextBlockId
                self._NextBlockId = (self._NextBlockId+1) % 255
            elif self._LastBlockId == COMMBUFF_BLOCK_ID_INIT:
                blockId = self._NextBlockId
                #self._NextBlockId = (self._NextBlockId+1) % 255 # Changed 22.11.2024
            else:
                blockId = self._LastBlockId # Changed 22.11.2024
                t = (blockStatus & 0xFF) | (blockId << 8) # Changed 22.11.2024
                mem4x.setuint16(self.CFG_CommBuffOffset+COMMBUFF_HEADER_ID_STATUS, t) # Changed 22.11.2024
                mem4x.setuint16(self.CFG_CommBuffOffset+COMMBUFF_HEADER_EV_COUNT, evcount) # Changed 22.11.2024
                return
        else:
            return
        print(f"{self.Name}.New BlockId: "+str(blockId))
        self._LastBlockId = blockId
        t = (blockStatus & 0xFF) | (blockId << 8)
        mem4x.setuint16(self.CFG_CommBuffOffset+COMMBUFF_HEADER_ID_STATUS, t)
        mem4x.setuint16(self.CFG_CommBuffOffset+COMMBUFF_HEADER_EV_COUNT, evcount)
        mem4x.setuint16(self.CFG_CmdAckOffset, evcount) # if evcount == 0 next cycle 'elif cmdAck' will play
        self.setexcstatus(evcount if evcount < 64 else 63)
        
    def fillInitBuff(self):
        self._eventbuff.clear()
        d = datetime.now()
        offset = self.CFG_InitDateTimeOffset
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_DAY_OF_WEEK  , 0           )
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_MONTH        , d.month     )
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_DAY          , d.day       )
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_YEAR         , d.year % 100)
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_HOUR         , d.hour      )
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_MINUTE       , d.minute    )
        mem4x.setuint16(offset+INIT_DATETIME_BUFFER_IDX_SECOND       , d.second    )
        #offset = InitBuffOffset + INITBUFF_HEADER_SIZE
        for i in range(self.SubSystemRegSize):
            mem4x.setuint16(self.CFG_InitSubSystemTableOffset+i, mem4x.getuint16(self.CFG_DataSubSystemTableOffset+i))
        for i in range(self.ValveRegSize):
            mem4x.setuint16(self.CFG_InitValveOffset+i, mem4x.getuint16(self.CFG_DataValveOffset+i))
        for i in range(self.AnalogRegSize):
            mem4x.setuint16(self.CFG_InitAnalogOffset+i, mem4x.getuint16(self.CFG_DataAnalogOffset+i))

    def pushSubSystemStatus(self, index, bitNum, value):
        print(f"{self.Name}.pushSubSystemStatus({index=}, {bitNum=}, {value=})")
        self._eventbuff.pushSubSystemStatus(index, bitNum, value)
        
    def pushValveStatus(self, index, bitNum, value):
        print(f"{self.Name}.pushValveStatus({index=}, {bitNum=}, {value=})")
        self._eventbuff.pushValveStatus(index, bitNum, value)
        
    def pushAnalogStatus(self, index, bitNum, value):
        print(f"{self.Name}.pushAnalogStatus({index=}, {bitNum=}, {value=})")
        self._eventbuff.pushAnalogStatus(index, bitNum, value)
        
    def pushAnalogValue(self, index, value):
        print(f"{self.Name}.pushAnalogValue({index=}, {value=})")
        self._eventbuff.pushAnalogValue(index, value)

    def pushServiceEvent(self, evnum):
        print(f"{self.Name}.pushServiceEvent({evnum=})")
        self._eventbuff.pushValveStatus(VALVE_INDEX_SPECIAL, evnum, 1)


cfg = ConfigPLC()
plc = PLC(cfg)

#############################################
############## USER CODE: LOOP ##############
#############################################

print("Python begins to work!!!")

testoffset = 32
while (ctrl.getflags() & 1):
    plc.run()
    v = mem4x.getuint16(testoffset)
    mem4x.setuint16(testoffset, v+1)
    ctrl.setpycycle(ctrl.getpycycle()+1)
    if (v % 200 == 0):
        print("Hello from Python!!!")
    time.sleep(0.001)

#############################################
############ USER CODE: FINALIZE ############
#############################################

print("Bye-Bye")
