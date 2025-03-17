\mainpage ModbusTools

# Overview

ModbusTools are cross-platform (Windows, Linux) Modbus simulator tools (client and server) 
with GUI to work with standard Modbus Protocol. 
Modbus Tools are a free, open-source  tools with a simple user interface written in C++/Qt. 
It implements TCP, RTU and ASCII versions of Modbus Protocol.

Software implements such Modbus functions as:
* `1 ` (`0x01`) - `READ_COILS`
* `2 ` (`0x02`) - `READ_DISCRETE_INPUTS`
* `3 ` (`0x03`) - `READ_HOLDING_REGISTERS`
* `4 ` (`0x04`) - `READ_INPUT_REGISTERS`
* `5 ` (`0x05`) - `WRITE_SINGLE_COIL`
* `6 ` (`0x06`) - `WRITE_SINGLE_REGISTER`
* `7 ` (`0x07`) - `READ_EXCEPTION_STATUS`
* `15` (`0x0F`) - `WRITE_MULTIPLE_COILS`
* `16` (`0x10`) - `WRITE_MULTIPLE_REGISTERS`
* `17` (`0x11`) - `REPORT_SERVER_ID` (since v0.4)
* `22` (`0x16`) - `MASK_WRITE_REGISTER` (since v0.3)
* `23` (`0x17`) - `READ_WRITE_MULTIPLE_REGISTERS` (since v0.3)

ModbusTools work with Qt Framework version 5 (5.8 or later).
It based on `ModbusLib` cross platform library project:

https://github.com/serhmarch/ModbusLib

## Memory item addressing

Modbus Tools uses a 6-digit memory addressing type.
The left digit indicates the memory type (`0`, `1`, `3` or `4`) and
the right 5 digits indicate the address of the memory cell starting from 1 - [1:65536].

__*New in version 0.4*__:
> IEC61131-3 addressing type is available. There is a setting that allows you to choose between
standard Modbus (1 based) and IEC61131-3 (0 based) addressing types.

__*New in version 0.4.1*__:
> IEC61131-3 Hex (0 based) addressing type is available, e.g. `%I0000h`, `%Q001Fh`, `%IW0B7Ch`, `%MW1A01h`.

Memory types decribed below:

* `0x` - intended for coils (discrete outputs, boolean), corresponding modern PLC notation is `%%M` or `%%Q`
(e.g. `000001 `coil in modern notation can be accessed like `%%M0` or `%%Q0`), can have Read/Write access,
corresponding access functions are `READ_COILS `(which number is `1`) and `WRITE_MULTIPLE_COILS`(`15`, `0x0F`);

* `1x` - intended for discrete inputs (boolean), corresponding modern PLC notation is `%%I`
(e.g. `100001 `boolean in modern notation can be accessed like `%%I0`), has ReadOnly access,
corresponding access function is `READ_DISCRETE_INPUTS `(`2`);

* `3x` - intended for input registers (analog inputs, 16 bit word), corresponding modern PLC notation is `%%IW`
(e.g. `300001 `word in modern notation can be accessed like `%%IW0`), has ReadOnly access,
corresponding access function is `READ_INPUT_REGISTERS`(`4`);

* `4x` - intended for holding registers (analog outputs, 16 bit word), corresponding modern PLC notation is `%%MW`
(e.g. `400001 `word in modern notation can be accessed like `%%MW0`), , can have Read/Write access,
corresponding access functions are `READ_HOLDING_REGISTERS`(`3`) and `WRITE_MULTIPLE_REGISTERS`(`16`, `0x10`);

Addressing examples:
| Memory type       | Standard (1 based) | IEC 61131-3 (0 based)| IEC 61131-3 Hex (0 based)
|-------------------|--------------------|----------------------|---------------------------
| Coils             | `000001`           | `%Q0`                | `%Q0000h`                 
| Discrete inputs   | `100016`           | `%I15`               | `%I000Fh`                
| Input registers   | `300017`           | `%IW16`              | `%IW0010h`               
| Holding registers | `406658`           | `%MW6657`            | `%MW1A01h`               
