#!/usr/bin/python

from os import sys, path
import argparse

parser = argparse.ArgumentParser()    
parser.add_argument('-imp', '--importpath', type=str, default="") 
args = parser.parse_args()   

pathList = args.importpath.split(";")

sys.path.insert(0, path.normpath(path.dirname(path.abspath(__file__))))
sys.path.extend(pathList)

print("PathList from args:"+str(pathList))

from mbServer import __MemoryControlBlock, __MemoryBlock

sMemPrefix = "ModbusTools.Server.PORT1.PLC1."
sMemControl = sMemPrefix+"control"
sMem4x = sMemPrefix+"mem4x"

ctrl = __MemoryControlBlock(sMemControl)
mem4x = __MemoryBlock(sMem4x, ctrl.getcount4x())

#############################################
########### USER CODE: INITIALIZE ###########
#############################################

<%init%>

#############################################
############## USER CODE: LOOP ##############
#############################################

while (ctrl.getflags() & 1):
<%loop%>
    ctrl.setpycycle(ctrl.getpycycle()+1)
    sleep(0.001)

#############################################
############ USER CODE: FINALIZE ############
#############################################

<%final%>
