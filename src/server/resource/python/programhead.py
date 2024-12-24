#!/usr/bin/python

from os import sys, path
import argparse

_parser = argparse.ArgumentParser()
_parser.add_argument('-imp', '--importpath', type=str, default="")
_parser.add_argument('-i', '--memid' , type=str, default="")
_args = _parser.parse_args()

_pathList = _args.importpath.split(";")

sys.path.insert(0, path.normpath(path.dirname(path.abspath(__file__))))
sys.path.extend(_pathList)

print("PathList from args:"+str(_pathList))

from mbserver import _MemoryControlBlock, _MemoryBlockBits, _MemoryBlockRegs

#sMemPrefix = "ModbusTools.Server.PORT1.PLC1."
#sControl = sMemPrefix+"control"
#sMem3x = sMemPrefix+"mem4x"
#sMem4x = sMemPrefix+"mem4x"

_sId = _args.memid

_sControl = _sId + ".control"
_sMem0x   = _sId + ".mem0x"
_sMem1x   = _sId + ".mem1x"
_sMem3x   = _sId + ".mem3x"
_sMem4x   = _sId + ".mem4x"

print("ControlId: " , _sControl)
print("Memory0xId: ", _sMem0x  )
print("Memory1xId: ", _sMem1x  )
print("Memory3xId: ", _sMem3x  )
print("Memory4xId: ", _sMem4x  )

_ctrl = _MemoryControlBlock(_sControl)
mem0x = _MemoryBlockBits(_sMem0x, _ctrl.getcount0x())
mem1x = _MemoryBlockBits(_sMem1x, _ctrl.getcount1x())
mem3x = _MemoryBlockRegs(_sMem3x, _ctrl.getcount3x())
mem4x = _MemoryBlockRegs(_sMem4x, _ctrl.getcount4x())

