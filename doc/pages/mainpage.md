\mainpage ModbusTools

[TOC]

# Modbus Tools

## Overview

ModbusTools are cross-platform (Windows, Linux) Modbus simulator tools (client and server) 
with GUI to work with standard Modbus Protocol. 
Modbus Tools are a free, open-source  tools with a simple user interface written in C++/Qt. 
It implements TCP, RTU and ASCII versions of Modbus Protocol.

Software implements such Modbus functions as:
* `1`  (`0x01`) - `READ_COILS`
* `2`  (`0x02`) - `READ_DISCRETE_INPUTS`
* `3`  (`0x03`) - `READ_HOLDING_REGISTERS`
* `4`  (`0x04`) - `READ_INPUT_REGISTERS`
* `5`  (`0x05`) - `WRITE_SINGLE_COIL`
* `6`  (`0x06`) - `WRITE_SINGLE_REGISTER`
* `7`  (`0x07`) - `READ_EXCEPTION_STATUS`
* `15` (`0x0F`) - `WRITE_MULTIPLE_COILS`
* `16` (`0x10`) - `WRITE_MULTIPLE_REGISTERS`
* `22` (`0x16`) - `MASK_WRITE_REGISTER`
* `23` (`0x17`) - `WRITE_MULTIPLE_REGISTERS`

ModbusTools work with Qt Framework version 5 (5.8 or later).
It based on `ModbusLib` cross platform library project:

https://github.com/serhmarch/ModbusLib

