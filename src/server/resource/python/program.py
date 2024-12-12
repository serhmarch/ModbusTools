#!/usr/bin/python

from os import sys, path
import argparse

_parser = argparse.ArgumentParser()
_parser.add_argument('-imp', '--importpath', type=str, default="")
_parser.add_argument('-ic', '--idctrl' , type=str, default="")
_parser.add_argument('-i3', '--idmem3x', type=str, default="")
_parser.add_argument('-i4', '--idmem4x', type=str, default="")
_args = _parser.parse_args()

_pathList = _args.importpath.split(";")

sys.path.insert(0, path.normpath(path.dirname(path.abspath(__file__))))
sys.path.extend(_pathList)

print("PathList from args:"+str(_pathList))

from mbserver import _MemoryControlBlock, _MemoryBlock

#sMemPrefix = "ModbusTools.Server.PORT1.PLC1."
#sControl = sMemPrefix+"control"
#sMem3x = sMemPrefix+"mem4x"
#sMem4x = sMemPrefix+"mem4x"

_sControl = _args.idctrl
_sMem3x   = _args.idmem3x
_sMem4x   = _args.idmem4x

print("ControlId: ", _sControl)
print("Memory3xId: ", _sMem3x)
print("Memory4xId: ", _sMem4x)

_ctrl = _MemoryControlBlock(_sControl)
mem3x = _MemoryBlock(_sMem3x, _ctrl.getcount3x())
mem4x = _MemoryBlock(_sMem4x, _ctrl.getcount4x())

#############################################
########### USER CODE: INITIALIZE ###########
#############################################

<%init%>

#############################################
############## USER CODE: LOOP ##############
#############################################

while (_ctrl.getflags() & 1):
<%loop%>
    _ctrl.setpycycle(_ctrl.getpycycle()+1)
    sleep(0.001)

#############################################
############ USER CODE: FINALIZE ############
#############################################

<%final%>
