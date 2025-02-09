# Objects for access corresponding device memory: mem0x, mem1x, mem3x, mem4x.
#
# Every object has set of get/set function to work with different data types:
#  * mem0x, mem1x: `get<datatype>(bitoffset:int)->(int, float)` and `set<datatype>(bitoffset:int,value:(int, float))`
#  * mem3x, mem4x: `get<datatype>(regoffset:int)->(int, float)` and `set<datatype>(regoffset:int,value:(int, float))`
#
# <datatype>: int8, uint8, int16, uint16, int32, uint32, int64, uint64, float, double.
#
# Examples:
#  v = mem0x.getint8(0)
#  mem1x.setint16(1, -1)
#  mem3x.setuint16(0, 65535)
#  mem3x.setfloat(0, 3.14)
#  mem4x.setdouble(10, 2.71828)

