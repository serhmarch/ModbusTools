# 0.1.1

* Tested and added binaries for Ubuntu 22.04. 
* Fixed some bugs

# 0.1.2

* Fixed some bugs

# 0.1.3

* Enable Server side 'Edit'-menu commands (Paste,Insert,Delete) for DataView table in 'Run' mode
* Improve import of DataView items
* Fix some bugs

# 0.1.4

* Fixed bug

# 0.1.5

* Fix 'String'-format of data view item displaying

# 0.2.0

* Make ModbusTools available for older version of Qt framework:
  starting with version 0.2.0 ModbusTools support Qt 5.8 version or later
* Add ModbusLib submodule project. It non-Qt project working with TCP sockets and serial port
  for Modbus protocol. It deletes dependencies to Qt5Network and Qt5SerialPort libs.
* Fix server action (simulation) edit bugs

# 0.2.1

* Fix bugs

# 0.2.2

* Fixed bug

# 0.3.0

* Add Modbus Scanner
* Add new Modbus functions: MASK_WRITE_REGISTER, READ_WRITE_MULTIPLE_REGISTERS
* Improve 'Send Message' dialog window
* Add all encodings suppoted by Qt framework to the 'String' data format
* Add possibility to save DataViewItem values in values when saving project for Client
* Upgrade status bar with Tx/Rx statistic
* Add `Copy` action type