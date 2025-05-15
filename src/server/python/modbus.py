"""@package modbus
Module to work with Modbus protocol.
 
Includes class Address to work with modbus address.
"""

# Modbus memory types
Memory_Unknown = -1 ##< Memory type for invalid address
Memory_0x      = 0  ##< Memory type for coils
Memory_1x      = 1  ##< Memory type for input discretes
Memory_3x      = 3  ##< Memory type for input registers
Memory_4x      = 4  ##< Memory type for iholding registers

Notation_Default     = 0 ##< Default notation which is equal to Modbus notation
Notation_Modbus      = 1 ##< Standard Modbus address notation like `000001`, `100001`, `300001`, `400001`
Notation_IEC61131    = 2 ##< IEC-61131 address notation like `%%Q0`, `%%I0`, `%%IW0`, `%%MW0`
Notation_IEC61131Hex = 3 ##< IEC-61131 Hex address notation like `%%Q0000h`, `%%I0000h`, `%%IW0000h`, `%%MW0000h`


## @brief Python set that contains supported Modbus Address types
MemoryTypeSet = { Memory_0x, Memory_1x, Memory_3x, Memory_4x }

sIEC61131Prefix0x = "%Q"  ##< IEC-61131 address notation prefix for coils
sIEC61131Prefix1x = "%I"  ##< IEC-61131 address notation prefix for input discretes
sIEC61131Prefix3x = "%IW" ##< IEC-61131 address notation prefix for input registers
sIEC61131Prefix4x = "%MW" ##< IEC-61131 address notation prefix for holding registers

cIEC61131SuffixHex = 'h'  ##< Suffix for IEC-61131 Hex address notation


## @brief Python set that contains supported Modbus address IEC61131 prefixes
IEC61131PrefixMap = {
                Memory_0x: sIEC61131Prefix0x,
                Memory_1x: sIEC61131Prefix1x,
                Memory_3x: sIEC61131Prefix3x,
                Memory_4x: sIEC61131Prefix4x,
            }

class Address:
    """
    @brief Modbus Data Address class. Represents Modbus Data Address.

    @details `Address` class is used to represent Modbus Data Address. It contains memory type and offset.
    E.g. `modbus.Address(modbus.Memory_4x, 0)` creates `400001` standard address.
    E.g. `modbus.Address(400001)` creates `Address` with type `Modbus::Memory_4x` and offset `0`, and
    `modbus.Address(1)` creates `modbus.Address` with type `modbus.Memory_0x` and offset `0`.
    Class provides convertions from/to string methods.

    Class supports next operators and standard functions:
    +, -, <, <=, >, >=, ==, !=, hash(), str(), int()
    """

    def __init__(self, value=None, offset=None):
        """
        @brief Constructor of the class.

        @details Can have next forms:
        * `Address()`  - creates invalid address class
        * `Address(Memory_4x, 0)`  - creates address for holding registers with `offset=0`
        * `Address("%MW0")`  - creates address for holding registers with `offset=0`
        * `Address("%Q0000h")`  - creates address for coils with `offset=0`
        * `Address("100001")`  - creates address for input discretes with `offset=0`
        * `Address(300001)`  - creates address for input registers with `offset=0`

        """
        self._type = Memory_Unknown
        self._offset = 0
        if value is None:
            pass
        elif isinstance(value, int) and offset is None:
            self.fromint(value)
        elif isinstance(value, str) and offset is None:
            self.fromstr(value)
        elif isinstance(value, int) and isinstance(offset, int):
            self.settype(value)
            self.setoffset(offset)
        else:
            raise ValueError("Invalid constructor parameters")

    def isvalid(self) -> bool:
        """
        @details Returns `True` if memory type is not `Modbus::Memory_Unknown`, `False` otherwise.
        """
        return self._type != Memory_Unknown

    def type(self) -> int:
        """
        @details Returns memory type of Modbus Data Address.
        """
        return self._type

    def settype(self, tp: int):
        """
        @details Set memory type of Modbus Data Address.
        """
        if tp not in MemoryTypeSet:
            raise ValueError(f"Invalid memory type: {tp}. Memory type must be [0,1,3,4]")
        self._type = tp

    def offset(self) -> int:
        """
        @details Returns memory offset of Modbus Data Address.
        """
        return self._offset

    def setoffset(self, offset: int):
        """
        @details Set memory offset of Modbus Data Address.
        """
        if not (0 <= offset <= 65535):
            raise ValueError(f"Invalid offset: {offset}. Offset must be in range [0:65535]")
        self._offset = offset

    def number(self) -> int:
        """
        @details Returns memory number (offset+1) of Modbus Data Address.
        """
        return self._offset + 1

    def setnumber(self, number: int):
        """
        @details Set memory number (offset+1) of Modbus Data Address.
        """
        self.setoffset(number - 1)

    def fromint(self, v: int):
        """
        @details Make modbus address from integer representaion
        """
        number = v % 100000
        if number < 1 or number > 65536:
            self._type = Memory_Unknown
            self._offset = 0
            raise ValueError(f"Invalid integer '{v}' to convert into Address: number part '{number}' must be [1:65536]")

        mem_type = v // 100000
        if mem_type in MemoryTypeSet:
            self._type = mem_type
            self.setoffset(number - 1)
        else:
            raise ValueError(f"Invalid integer '{v}' to convert into Address: memory type '{mem_type}' must be [0,1,3,4]")

    def toint(self) -> int:
        """
        @details Converts current Modbus Data Address to `int`,
        e.g. `Address(Memory_4x, 0)` will be converted to `400001`.
        """
        return (self._type * 100000) + self.number()

    def fromstr(self, s: str):
        """
        @details Make modbus address from string representaion
        """
        def dec_digit(c):
            return int(c) if c.isdigit() else -1

        def hex_digit(c):
            try:
                return int(c, 16)
            except ValueError:
                return -1

        if s.startswith('%'):
            i = 0
            if s.startswith(sIEC61131Prefix3x):
                self._type = Memory_3x
                i = len(sIEC61131Prefix3x)
            elif s.startswith(sIEC61131Prefix4x):
                self._type = Memory_4x
                i = len(sIEC61131Prefix4x)
            elif s.startswith(sIEC61131Prefix0x):
                self._type = Memory_0x
                i = len(sIEC61131Prefix0x)
            elif s.startswith(sIEC61131Prefix1x):
                self._type = Memory_1x
                i = len(sIEC61131Prefix1x)
            else:
                raise ValueError(f"Invalid str '{s}' to convert into Address")

            offset = 0
            suffix = s[-1]
            if suffix == cIEC61131SuffixHex:
                for c in s[i:-1]:
                    offset *= 16
                    d = hex_digit(c)
                    if d < 0:
                        return Address()
                    offset += d
            else:
                for c in s[i:]:
                    offset *= 10
                    d = dec_digit(c)
                    if d < 0:
                        return Address()
                    offset += d
            self.setoffset(offset)
        else:
            acc = 0
            for c in s:
                d = dec_digit(c)
                if d < 0:
                    return Address()
                acc = acc * 10 + d
            self.fromint(acc)

    def tostr(self, notation: int = Notation_Default) -> str:
        """
        @details Returns string repr of Modbus Data Address with specified notation:
        * `Notation_Modbus`      - `Address(Memory_4x, 0)` will be converted to `"400001"`.
        * `Notation_IEC61131`    - `Address(Memory_4x, 0)` will be converted to `"%MW0"`.
        * `Notation_IEC61131Hex` - `Address(Memory_4x, 0)` will be converted to `"%MW0000h"`.
        """
        def to_dec_string(n, width=0):
            return str(n).rjust(width, '0') if width else str(n)

        def to_hex_string(n):
            return format(n, 'X').rjust(4, '0')

        if not self.isvalid():
            return "Invalid address"

        if notation == Notation_IEC61131:
            return IEC61131PrefixMap.get(self._type, "") + to_dec_string(self._offset)
        elif notation == Notation_IEC61131Hex:
            return IEC61131PrefixMap.get(self._type, "") + to_hex_string(self._offset) + cIEC61131SuffixHex

        else:
            return to_dec_string(self.toint(), 6)

    def __int__(self):
        """
        @details Return the integer representation of the object by calling the toint() method.
        """
        return self.toint()

    def __lt__(self, other):
        """
        @details Return self.toint() < other.toint()
        """
        return self.toint() < other.toint()
    
    def __le__(self, other):
        """
        @details Return self.toint() <= other.toint()
        """
        return self.toint() <= other.toint()

    def __eq__(self, other):
        """
        @details Return self.toint() == other.toint()
        """
        return self.toint() == other.toint()

    def __ne__(self, other):
        """
        @details Return self.toint() != other.toint()
        """
        return self.toint() != other.toint()
    
    def __gt__(self, other):
        """
        @details Return self.toint() > other.toint()
        """
        return self.toint() > other.toint()

    def __ge__(self, other):
        """
        @details Return self.toint() >= other.toint()
        """
        return self.toint() >= other.toint()

    def __hash__(self):
        """
        @details Return the hash of the object.
        """
        return self.toint()

    def __add__(self, other: int):
        """
        @details Return a new Address object with the offset increased by the given integer.
        """
        return Address(self._type, self._offset + other)

    def __sub__(self, other: int):
        """
        @details Return a new Address object with the offset decreased by the given integer.
        """
        return Address(self._type, self._offset - other)

    def __iadd__(self, other: int):
        """
        @details Increase the offset by the given integer.
        """
        self.setoffset(self._offset + other)
        return self
    
    def __isub__(self, other: int):
        """
        @details Decrease the offset by the given integer.
        """
        self.setoffset(self._offset - other)
        return self
    
    def __repr__(self):
        """
        @details Return the string representation of the object.
        """
        return self.tostr(Notation_Default)

    def __str__(self):
        """
        @details Return the string representation of the object.
        """
        return self.tostr(Notation_Default)
