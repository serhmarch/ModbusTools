# 0.1.1

## virtual int parseArgs(int& argc, char **argv)

Changed cause of program crash in Linux:
virtual int parseArgs(int argc, char **argv); >> virtual int parseArgs(int& argc, char **argv);

## std::atomic<int> m_refCount;

Changed cause of program crash in Linux:
g++ incorrectly initialize default constuctor for std::atomic<int> for some reason.
`std::atomic<int> m_refCount;` was replaced by

```cpp 
typedef QAtomicInt RefCount_t;
...
    mb::RefCount_t m_refCount;                 
```

# 0.1.2

* Fixed server-side bug when saving/exporting device

# 0.1.3

* Fixed 'ByteArray'-format DataView issue

# 0.1.4

* Fixed a crash bug for the 'readCoils' and 'readDiscreteInputs' functions
  when the bit offset was not a multiple of 8

# 0.2.2

* Fixed an Action value updates - Not Following Defined Register and Byte Order

# 0.3.1

* Fixed a bug of client/server crash when create/open project
* Fixed a bug of client/server when change of port settings doesn't reflect in the port name in status bar 

# 0.3.2

* Fixed a bug when changing the log settings does not affect the LogView of client/server

# 0.3.3

* Fixed incorrect data offset for 'WriteSingleRegister'-function in 'SendMessage'-window
