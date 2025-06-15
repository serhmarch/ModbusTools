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

* Fixed 'String'-format of data view item displaying

# 0.2.0

* Make ModbusTools available for older version of Qt framework:
  starting with version 0.2.0 ModbusTools support Qt 5.8 version or later
* Add ModbusLib submodule project. It non-Qt project working with TCP sockets and serial port
  for Modbus protocol. It deletes dependencies to Qt5Network and Qt5SerialPort libs.
* Fixed `server` action (simulation) edit bugs

# 0.2.1

* Renew ModbusLib which fix flooding messages and open/close port bugs
* Add CMake support
* Fixed bugs

# 0.2.2

* Fixed bug

# 0.3.0

* Add Modbus Scanner for Modbus `client`
* Add new Modbus functions: MASK_WRITE_REGISTER, READ_WRITE_MULTIPLE_REGISTERS
* Improve 'Send Message' dialog window
* Add all encodings suppoted by Qt framework to the 'String' data format
* Add possibility to save DataViewItem values in values when saving project for `client`
* Upgrade status bar with Tx/Rx statistics
* Add `Copy` action type

# 0.3.1

* Make project builder more tolerant for unexpected attribute or element
* Fixed bugs

# 0.3.2

* Load previously saved project when starting the `client`/`server`
* Mdi Windows are saved/restored their geometry (when saving/opening project) for `client`/`server`
* Fixed bugs

# 0.3.3

* Improve client's Scanner Window
* Allow user right to enter port name if it's absent in list of serial ports in ComboBox
* Fixed bugs

# 0.3.4

* Added the ability to import/export DataView data list selected items in CSV format
* Fixed bugs

# 0.3.5

* When click `Insert` in the `DataView` window, the new value is added to the list, 
  but taking into account the address and size of the element format above.
* Added insert/delete hotkeys for port/device/actions elements (+copy/paste for simulation actions)
* Added a confirmation prompt when deleting items such as port and device
* Added import/export whole DataView list in csv-format
* Added import/export for simulation actions in csv-format
* Added min/max params for simulation action of Increment

# 0.3.6

* Added `ProjectInfo` dialog + update project xml file params
* Added detection of changes in the project and confirmation prompt 
  when exit and project is not saved

# 0.3.7

* Added menu `File->Recent` to quick open previous projects
* Added menu `File->Close` to close current project
* Fixed Client Scanner for Modbus TCP scanning

# 0.3.8

* Added `Export` button for LogView

# 0.3.9

* Add period from DataView list to DataView item for client

# 0.3.10

* Fixed WriteSingleCoil offset for client SendMessage window
* Added some new icons
* Fixed Server help files

# 0.4.0

List of changes of new v0.4 version of ModbusTools:

* Added scripting with Python for server application
* Added support for `REPORT_SERVER_ID` (17, 0x11) Modbus function
* Added editable columns capabilities for DataView list
* Added item address notation setting: 1-based (standard) and 0-based (IEC 61131-3)
* Extended log view flags to enable/disable certain type of log messages
* Extended `RegisterOrder` support:     
    * `R0R1R2R3`
    * `R3R2R1R0`
    * `R1R0R3R2`
    * `R2R3R0R1`
* Updated `System settings` dialog windows

# 0.4.1

* Added 'LoopPeriod' setting for server script execution
* Improved script editor and its settings
* Added support for IEC 61131-3 Hex item address notation
* Added support for broadcast mode for `0` unit
* Imroved 'SendMessage' dialog
* Added docker support (by Mattia De Vivo): https://hub.docker.com/r/serhmarch/modbustools
* Updated docs

# 0.4.2

* Fixed Odd and Even Parity mixed up
  [](https://github.com/serhmarch/ModbusTools/issues/34)
* Fixed the inability to set an address after 99 for 'SendMessage' and 'Item(s)' dialogs
  [](https://github.com/serhmarch/ModbusTools/issues/36)
* Fixed error message appearing for income packets not aimed for the configured unit addresses
  [](https://github.com/serhmarch/ModbusTools/discussions/28#discussioncomment-12639172)
* Added device byte and register order settings support for scripting
* Added get/set string functions for working with strings for scripting
* Improved default project for client and server

# 0.4.3

* Fixed TCP Server delay while connecting (within ModbusLib subproject)
* Added setting 'Max Connections' for max simultanious TCP server connections
* Added support for script modules for the ModbusTools server
* Added install command for cmake script
* Set max count of mem cells as 65536 instead of 65535

# 0.4.4

* Added 'Font' settings for LogView
* Added 'Font' settings for Output script window
* Added 'Enable Script' setting for server device
* When closing the script editor window, synchronized the code with the current project
* Fixed when opening the context menu on the Device node in ProjectView may open the wrong Device window
* Added python 'modbus.py' module to work with modbus protocol
* Extended `mbserver.py` script library
* Improved MDI window management
* Improved script text editor
* Added 'Import Project' functionality which unites current project with the imported one
