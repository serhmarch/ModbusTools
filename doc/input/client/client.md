\page sec_client Client

# About Modbus Client
The client implements access to a remote Modbus device. However, the client can work not only with one device, 
but with several devices that can be connected to a single network, which is especially useful when working 
with RTU and ASCII protocols, when access to all Modbus servers is realized through a single serial port. 
However, it can also be useful when using the TCP version of the protocol, for example, if you use the TCP->RTU bridge.

All work is done within a single project. The main entities in the project are Port, Device and DataViewItem. 
Port contains network settings for both TCP/IP and serial ports. Device contains settings for a single device 
(such as Modbus Unit Address, etc.).  The DataViewItem contains a single data unit to be read from the remote 
device and has many formats to represent the current data.

# Quickstart

![](client_view.png)

To start working with the program, you need to run `client.exe`. Then, by default, when you first start the program, 
an empty project is created, consisting of a single port, a device attached to it, and an empty DataView data list.

## Port

To change the port settings, you should double-click on the port name in the project tree 
(or use the menu `Port->Edit Port...`). 
The port must be selected in the project tree.  To add a new port, select `Port->New Port...`

![](client_port_dialog.png)

In this dialog box, you can set the network settings for the port.

## Device

To change the device settings, you need to double click left mouse button on the device name in the project tree 
(or use the menu `Device->Edit Device...`). The device must be selected in the project tree. To add a new device, 
select `Device->New Device...` or `Port->New Device...` to immediately bind this device to the selected port.

![](client_device_dialog.png)

In this dialog box, you can set the device settings, such as the Modbus Unit Address.

## DataView

To add items to the view, you can use the menu `Data->New Item(s)...` or press the `Insert` key.

![](client_dataviewitem_dialog.png)

In this window, you can select all the data parameters to be added to the project, 
such as the device to which the data relates, address, format, and number of such data elements. 
The addresses of the following elements will be automatically calculated as you add them. 
Data parameters can be edited individually directly in the DataView list.

## Runtime

To start the process of polling remote Modbus devices, click `Run` on the shortcut bar (or the menu `Runtime->Run`). 
After starting the system, project editing is not available (except for some DataViewItem parameters).

After you have finished working with the project, you can save it using `File->Save` or `File->Save As...`.

# Graphical User Interface

![](client_gui_schema.png)

The graphical interface consists of the following main elements:
* `Project View` - a project window that displays the structure of the project, consisting of such nodes as Port and Device
* `Data View(s)` - lists of data to be read/write
* `LogView` - a window for displaying information
* `Main menu` - access to all the features of the program
* `Tool bar` - access to the most frequently used commands
* `Status bar` - the current status of the program

## Project window

![](client_project_window.png)

It displays the network settings for this project, i.e., the ports (Port) through which the exchange with remote 
Modbus devices takes place, as well as the Modbus endpoints (devices) themselves. 
Many Modbus devices can be specified for one port, so you can not only poll a single device in the Modbus network, 
but also organize an exchange with all devices that are in the same network. 
There can also be several such networks in one project.

Project window can show main settings for Port (port type, host,port) and Device (Modbus Unit Address) 
displayed in `[]`-brackets right of Port/Device name. `[]`-view can be managed using 
`Tools->Settings->View->Use Port/Device name with settings`.

To create new port use menu/context menu `Port->New Port...`.

To edit selected port use menu/context menu `Port->Edit Port...`.

To delete selected port use menu/context menu `Port->Delete Port`.

To create new device for selected port use menu/context menu `Port->New Device...`. 
Also can be used `Device->New Device...` for this purpose and than appropriate port can be selected from list.

`Port->Clear All Devices` delete all devices from selected port.

Such settings as port type, port number, timeouts and others can be seen and modified using `Port Dialog` 
when create or edit port.

To import/export port use menu/context menu `Port->Import Port...` and `Port->Export Port...` respectively.
For such import/export `xml`-format is used.

To create new device use menu/context menu `Device->New Device...`.
Appropriate port can be selected or new port can be created in newly opened Device dialog window.

To edit selected device use menu/context menu `Device->Edit Device...`. 
In newly opened Device dialog window port for current device can be chaged.

To delete selected device use menu/context menu `Device->Delete Device`.

Such settings as device unit addres, maximun amount of data to read/write and others can be seen and 
modified using `Device Dialog` when create or edit device.

To import/export device use menu/context menu `Device->Import Device...` and `Device->Export Device...` respectively. 
For such import/export `xml`-format is used.

Also current selected port is displayed in status bar with its statistics.

## DataView(s) windows

![](client_dataview_window.png)

In these windows, you can specify lists of variables that are read (written) to remote Modbus devices. 
There can be several such lists. These lists contain data for generating Modbus requests to the corresponding devices, 
data addresses, their polling period and display format.

Evere item has its own status and timestamp. The status mean quality of item. 
The timestamp represents the last date and time when a change of item's value or status was detected.

To create new DataView item(s) use menu/context menu `Data->New Item(s)...`. 
The addresses of the following elements will be automatically calculated as you add them. 

To edit selected DataView item(s) use menu/context menu `Data->Edit Item(s)...`. 

Menu/context menu `Data->Insert Item` or `Insert`-key inserts new item above selected or 
at the end of list if nothing selected. 
The addresses of the following elements will be automatically calculated as you add them.

To delete selected DataView item(s) use menu/context menu `Data->Delete Item` or `Delete`-key. 

There is copy/paste mechanism available in DataView item(s).

There is an import/export mechanism is also available in DataView. 
It can be import/export of selected item(s) in the list 
(`Data->Import Items...`/`Data->Export Items...`) and 
import/export of whole list (`Data->Import DataView...`/`Data->Export DataView...`). 
For such import/export `xml`-format is used.

DataView item parameters can also be edited individually directly in the DataView list.

## LogView window

![](client_logview_window.png)

Window for displaying information about the exchange process, debugging information, contents of Modbus network packets, etc. 
The information output can be customized using system settings dialog `Tools->Settings->Log/Timestamp`. 
Parameters descibed at `System settings`-dialog section.

## Menu
Main menu provides access to all the features of the program. It consists of:

### File
The `File` menu is intended for working with the project file and includes submenus:
* `New...` - open `Project`-dialog to create new project;
* `Open...` - open project from file that defined with standard open dialog. 
Standard file extension for client project is `.pjc`;
* `Save` - save project in previously defined file. Standard file extension for client project is `.pjc`;
* `Save As...` - save project in file that defined with standard save dialog. 
Standard file extension for client project is `.pjc`;
* `Edit...` - open `Project`-dialog to edit current project parameters;
* `Quit` - ends current application;

### Edit

The `Edit` menu provides standard operation to work with DataView items and clipboard such as 
`Cut`, `Copy`, `Paste` and other keyboard operations such as `Insert`, `Delete`, `Select All`.

### View
The `View` menu opens windows (if it were previously closed): `Project` and `LogView`.

### Port
The `Port` menu provides access to work with port and includes submenus:
* `New Port...` - open `Port`-dialog to create new port;
* `Edit Port...` - open `Port`-dialog to edit selected in `Project`-window port parameters;
* `Delete Port` - delete selected in `Project`-window port parameters;
* `New Device...` - open `Device`-dialog to create new device for selected in `Project`-window port and 
bind newly created device to selected port;
* `Clear All Devices` - delete all devices for selected port;
* `Import Port...` - import new port in project from file in `xml`-format;
* `Export Port...` - export selected port in project to file in `xml`-format;

### Device
The `Device` menu provides access to work with device and includes submenus:
* `New Device...` - open `Device`-dialog to create new device. Port for device can be choosen or create in Device dialog;
* `Edit Device...` - open `Device`-dialog to edit device parameters for active device. 
In this menu port for current device can be changed;
* `Delete Device` - delete active device;
* `Import Device...` - import new device in project from file in `xml`-format;
* `Export Device...` - export selected device in project to file in `xml`-format;

### Data
The `Data` menu provides access to work with DataViews and DataViewItems and includes submenus:
* `New Item(s)...` - open `DataViewItems`-dialog to create new item(s) for active DataView;
* `Edit Item(s)...` - open `DataViewItems`-dialog to edit selected item(s) for active DataView;
* `Insert Item` - insert new item to active DataView. Address and other parameters will be calculated automaticaly;
* `Delete Items` - delete selected items for active DataView;
* `Import Items...` - import new items in active DataView from file in `xml`-format;
* `Export Items...` - export selected items in active DataView to file in `xml`-format;
* `New DataView...` - open `DataView`-dialog to create new DataView;
* `Edit DataView...` - open `DataView`-dialog to edit active DataView;
* `Insert DataView` - create new DataView;
* `Delete DataView` - delete active DataView;
* `Import DataView...` - import new DataView in project from file in `xml`-format;
* `Export DataView...` - export active DataView to file in `xml`-format;

### Tools
The `Tools` menu provides access to work with various system settings and includes submenus:
* `Settings...` - open `SystemSettings`-dialog;
* `Send Message...` - individual Modbus message dialog;
* `Scanner...` - open Modbus Device Scanner dialog window;

### Runtime
The `Runtime` menu provides access to work with runtime execution and includes submenus:
* `Start/Stop` - run and stop current project for execution;

### Window
The `Window` menu provides access to work with MDI windows for DeviceViews and DataViews and includes submenus:
* `Show All` - show all MDI windows;
* `Show Active` - show active MDI window;
* `Close All` - close all MDI windows;
* `Close Active` - close active MDI window;
* `Cascade` - located all visible windows in cascade way;
* `Tile` - located all visible windows in tile way;

### Help
The `Help` menu provides access to work with current `Help`-system and includes submenus:
* `About` - show short info about application;
* `About Qt` - show info about current used Qt version;
* `Content` - show current help content.

## Tool bar

![](client_toolbar.png)

This picture show explanation of every action in tool bar. All these actions were previously descibed in `Menu` section.

## Status bar

![](client_statusbar.png)

Status bar displays current working port (e.g. `Port[TCP:localhost:502]`) and its statistics:
count of transmitted (`Tx`) and received (`Rx`) Modbus network packets.
Port can be changed by selection in `Project Window`.

This picture shows current status of runtime as well: `Stopped` or `Running`.

## Send Message window

![](client_sendmessage_window.png)

`Send Message` tool window is intended to send and review individual Modbus message. 
All supported Modbus functions can be used in this window.
In this window individual message parameters can be set such as data address and count, 
and message response data can be reviewed with different formats. Message transmit (Tx) and 
receive (Rx)  byte data can be viewed in corresponding `Tx` and `Rx`- labels.

Message data (bits and registers values)  separated by comma with specified format can be viewed in the Data window for read function and set for write functions.

* `Device` - device which message will belong to;

* `Function` - supported Modbus functions such as:
* `1 ` (`0x01`) - `READ_COILS`
* `2 ` (`0x02`) - `READ_DISCRETE_INPUTS`
* `3 ` (`0x03`) - `READ_HOLDING_REGISTERS`
* `4 ` (`0x04`) - `READ_INPUT_REGISTERS`
* `5 ` (`0x05`) - `WRITE_SINGLE_COIL`
* `6 ` (`0x06`) - `WRITE_SINGLE_REGISTER`
* `7 ` (`0x07`) - `READ_EXCEPTION_STATUS`
* `15` (`0x0F`) - `WRITE_MULTIPLE_COILS`
* `16` (`0x10`) - `WRITE_MULTIPLE_REGISTERS`
* `22` (`0x16`) - `MASK_WRITE_REGISTER` (since v0.3)
* `23` (`0x17`) - `WRITE_MULTIPLE_REGISTERS` (since v0.3)

`Send Message` dialog items:
* `Address` and `Count` - selected function`s parameters for read/write remote device`s data;
* `Format` - data format to view read data and to set write data;
* `Data (comma-separated)` - main field to view/set data. For read messages this field is read only. For write messages you can set data to be write into remote device separated by comma with specified format. The specified format determines the type of input/output;
* `Tx` - transmitted message byte data (depends on protocol);
* `Rx` - received message byte data (depends on protocol);
* `Status` - last message status;
* `Timestamp` - last message timestamp;
* `Send Period (msec)` - period of send message for `Send Periodically` mode.

Commands:
* `Send One` - command to send only single message;
* `Send Periodically` - command to set periodical mode with period `Send Period (msec)`. This mode can be finished using `Stop` button.
* `Stop` - finish to send messages periodically;
* `Close` - close window.

## Scanner window

![](client_scanner_window.png)

`Scanner` tool window is intended to scan Modbus network to determine which device 
with current settings are present in the network.

Scanner scans Modbus network in range `[UnitStart:UnitEnd]` with `tries` attempts.
The request can be customized using `Scanner Request`-dialog, 
which is called using `...` button in `Request` field.

There is a list of founded devices in central widget.
Founded devices can be add to the current project:
button `To Project` add selected devices, `All To Project` - all devices is added to the project.

Scanner's window parameters:
* `Type` - type of current port. Can be `TCP` for TCP/IP version and `RTU` or `ASC` for serial port version;
* `Timeout` - timeout for the single request;
* `Tries` - count of tries for the request;
* `Unit Start` - first unit address of scanning range;
* `Unit End` - last unit address of scanning range;
* `Request` - list of functions for the request. 
Can be edited in `Scanner Request`-dialog using `...` button.
* `Host` - IP/DNS address of remote device;
* `Port` - TCP/IP port number;
* `Baud Rate` - list of baud rates of serial port;
* `Data Bits` - list of data size of data bits for the serial port;
* `Parity` - list of parity of data: `No`,`Even`, `Odd`;
* `Stop Bits` - list of count of stop bits of serial port;

`Baud Rate`, `Data Bits`, `Parity`, `Stop Bits` lists define parameters combinations for the 
serial port scanning.

# Dialogs

Client application have following list of dialog windows:
* `System Settings` - dialog for edit system settings;
* `Project` - dialog for create/edit current project settings;
* `Port` - dialog for create/edit port settings;
* `Device` - dialog for create/edit device settings;
* `DataView` - dialog for create/edit DataView settings;
* `DataViewItem` - dialog for create/edit DataViewItem settings;
* `Send Message` - dialog for send individual Modbus-message.

## System Settings dialog

![](client_systemsettings_dialogx.png)

* `System Settings` - dialog shows all system settings that groups into tabs.

### View 

* `Use Port/Device names with settings` - display settings info in `[]`-brackets.
For port it displays port main settings, for device it displays device reference(s)
(Modbus device unit address(es) ).

### Log

* `LogFlags` - show log message categories that will be displayed in LogView;
* `Use timestamp` - display timestamp for log message in LogView;
* `Format` - set format for timestamp to be displayed in LogView. 

|Format        |Result         |
|--------------|---------------|
|dd.MM.yyyy    |21.05.2001     |
|ddd MMMM d yy |Tue May 21 01  |
|hh:mm:ss.zzz  |14:13:09.120   |
|hh:mm:ss.z    |14:13:09.12    |
|h:m:s ap      |2:13:9 pm      |          

Assumed that date and time are 21 May 2001 14:13:09.120.

## Project dialog

![client_project_dialog](client_project_dialog.png)

`Project`-dialog displayed when creating/editing current project settings such as:
* `Name` - name of current project;
* `Author` - author of current project;
* `Comment` - some commentary for current project.

## Port dialog

![](client_port_dialogx.png)

`Port`-dialog displayed when creating/editing current port settings such as:
* `Name` - name of current port;
* `Type` - type of current port. Can be `TCP` for TCP/IP version and `RTU` or `ASC` for serial port version;
* `Host` - IP/DNS address of remote device;
* `Port` - TCP/IP port number;
* `Timeout` - timeout for TCP/IP connection;
* `Serial Port` - name of serial port used;
* `Baud Rate` - baud rate of serial port;
* `Data Bits` - count of data bits of serial port;
* `Parity` - parity of data: `No`,`Even`, `Odd`;
* `Stop Bits` - count of stop bits of serial port;
* `Flow Control` - flow control of serial port;
* `Timeout (first byte)` - timeout waiting first byte for serial port;
* `Timeout (inter byte)` - timeout waiting next bytes after first bytes was received, 
after this timeout completes Modbus packet consider finished and return to process.

## Device dialog

![](client_device_dialogx.png)

`Device`-dialog displayed when creating/editing current device. 
In this dialog port for current device can be changed or created new port. Main device settings:
* `Name` - name of current device;
* `Unit` - Modbus unit address of remote device;
* `Read Coils`, `Read Discrete Inputs`, `Read Holding Registers`, `Read Input Registers`, `Write Mulptiple Coils`, `Write Multiple Registers` - maximum value of the quantity parameter for the corresponding messages;
* `Exception Status` - address of memory that considering as exception status. Can be any type of memory;
* `Register order` - default register order used for 32-bit size items and higher by default for current device;
* `Byte Array Format` - default byte array format items (used `ByteArray` as its format) for current device. Can be `Bin`, `Oct`, `Dec`, `UDec` and `Hex`; 
* `Byte Array Separator` - default separator for byte array format items (used `ByteArray` as its format) for current device. May contain escape sequences: `\s` - space, `\r`-carriage return, `\n` - new line, `\t` - tab, `\\` - for `\`;
* `String Length Type` - default type of string length calculation for string format items: `ZerroEnded` - first `\0` means end of string and data right of it is not displayed, `Full Length` - display all string data with all `\0` characters;
* `String Encoding` – default string encoding for current device.
Encoding can be selected from list of supported encodings from Qt Framework (`QTextCodec`-class).

## DataView dialog

![](client_dataview_dialog.png)

`DataView` - dialog disaplay information about created/edit DataView such as name and default period.

## DataViewItem dialog

![](client_dataviewitem_dialogx.png)

`DataViewItem`- dialog for create/edit DataViewItem settings.
* `Device` - name of device current item belongs to;
* `Address` - memory address current item refers to;
* `Count` - count of items to be create/deleted (disable when editing item(s) ). Address of next item are calculated automaticaly according to it address and size;
* `Format` - item format, that describes item size. Can be `Bool`, `Bin16`, `Oct16`, `Dec16`, `UDec16`, `Hex16`, `Bin32`, `Oct32`, `Dec32`, `UDec32`, `Hex32`, `Bin64`, `Oct64`, `Dec64`, `UDec64`, `Hex64`, `Float`, `Double`, `ByteArray`, `String`;
* `Bytes` - size of current item in bytes. It's enable to edit only for `ByteArray` and `String` format;
* `Period` - period for updating item values (milliseconds);
* `Byte order` - byte order current items;
* `Register order` - register order used for 32-bit size items and higher. If `Default` current device register order is used;
* `Byte Array Format` - byte array format for item. Can be `Bin`, `Oct`, `Dec`, `UDec` and `Hex`. If `Default` current device byte array format is used; 
* `Byte Array Separator` - separator for byte array format items (used `ByteArray` as its format). May contain escape sequences: `\s` - space, `\r`-carriage return, `\n` - new line, `\t` - tab, `\\` - for `\`. If `Default` current device byte array separator is used;
* `String Length Type` - type of string length calculation for string format items: `ZerroEnded` - first `\0` means end of string and data right of it is not displayed, `Full Length` - display all string data with all `\0` characters. If `Default` current device string length type is used;
* `String Encoding` - string encoding for current item. Can be `Utf8`, `Utf16`, `Latin1`. If `Default` current device string encoding is used.
