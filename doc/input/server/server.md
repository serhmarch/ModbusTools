\page sec_server Server

# About Modbus Server {#sec_server_about}

The server implements Modbus server device and works like Modbus simulator. 
However, the server can  not only simulate single device, but can simulate several devices that can be connected to 
a single network, which is especially useful when working with RTU and ASCII protocols,
when access to all Modbus servers 
is realized through a single serial port. However, it can also be useful to simulate Modbus network using the 
TCP version of the protocol, for example, if you use the TCP->RTU bridge, Modbus server can replace this bridge with 
remote devices for testing purposes.

All work is performing within a single project. The main entities in the project are Port, Device, 
DataViewItem and Action.
Port contains network settings for both TCP/IP and serial ports. Device contains settings for a single device 
(such as Modbus Unit Address, memory size etc).  
The DataViewItem contains a single data unit to be read/write from the device and has many formats to represent 
the current data. Action provides simulation capabilities (automatic change of device memory values).

Since v0.4 version `server` application allows to extend logic of your Modbus device 
simulator using one the most popular programming language - Python.
All you need to use scripting is installed Python interpreter and `PyQt5` library.

# Quickstart {#sec_server_quickstart}

![](server_view.png)

To start working with the program, you need to run `server.exe`. Then, by default, when you first start the program, 
an empty project is created, consisting of a single port, a device attached to it.

## Port {#sec_server_quickstart_port}

To change the port settings, you should double-click on the port name in the project tree 
or use the menu `Port->Edit Port...`). 
The port must be selected in the project tree.  To add a new port, select `Port->New Port...`

![](server_port_dialog.png)

In this dialog box, you can set the network settings for the port.

## Device {#sec_server_quickstart_device}

To change the device settings you need to double click left mouse button on the device name in the project tree 
(or use the menu `Device->Edit Device...`). The device must be selected in the project tree. 
To add a new device, select `Device->New Device...` or `Port->New Device...` to immediately bind this device 
to the selected port or use `New Device` toolbar menu.
In ProjectView in `[]`-brackets there are references (unit address) to this device in port context. 
Single device can be bind to several ports using this reference and
each port can have different reference(s) to this device. 
To edit this reference(s) select `Port->Edit Device...` (reference must be selected in `ProjectView`). 
To add existing device to selected port click `Port->Add Existing...`

![](server_device_dialog.png)

In this dialog box, you can set the device settings, such as device name and different memory blocks size.

## DataView {#sec_server_quickstart_dataview}

To add new DataView List click `Data->New Data View...`. In this window you can set name and refresh period.

![](server_dataview_dialog.png)

To add items to the view, you can use the menu `Data->New Item(s)...` or press the `Insert` key.

![](server_dataviewitem_dialog.png)

In this window, you can select all the data parameters to be added to the project, 
such as the device to which the data relates, address, format, and number of such data elements. 
The addresses of the following elements will be automatically calculated as you add them. 
Data parameters can be edited individually directly in the DataView list.

## Runtime {#sec_server_quickstart_runtime}

To start the process of accepting remote Modbus requests, click `Run` on the tool bar  (or the menu `Runtime->Run`).
After starting the system, project adding/removing ports and devices is not available. 
DataView lists and items can be add and delete at runtime. Device parameters can be edited at runtime.

After you have finished working with the project, you can save it using `File->Save` or `File->Save As...`.

# Graphical User Interface {#sec_server_gui}

![](server_gui_schema.png)

The graphical interface consists of the following main elements:
* `Project View` - a project window that displays the structure of the project,
consisting of such nodes as `Port` and `Device`
* `Device View(s)` – view full memory content of the single device
* `Data View(s)` - lists of data to be read/write
* `Script Window` - editor's window(s) to create/edit script source
* `Simulation View` – list of simulation items, items that change device memory automatically
* `LogView` - a window for displaying information
* `Output` - a window for script standard output
* `Main menu` - access to all the features of the program
* `Tool bar` - access to the most frequently used commands
* `Status bar` - the current status of the program

## Project window {#sec_server_gui_project}

![](server_project_window.png)

It displays the network settings for this project, i.e., the ports (Port) through which the exchange with current 
Modbus devices takes place. Many Modbus devices can be specified for one port, so you can not only use 
a single device in the Modbus network, 
but also organize an exchange with all devices that are in the same network. 
There can also be several such networks in one project.

Single device can be bind to various ports using reference (unit address). It’s displayed in square brackets. 
For instance PLC1 belongs to Port (RTU) using units from 1 to 10 and 255.
And the same device is using in Port1 with unit 1 
(`[]`-view can be managed using `Tools->Settings->View->Use Port/Device name with settings`).

To create new port use menu/context menu `Port->New Port...`.

To edit selected port use menu/context menu `Port->Edit Port...`.

To delete selected port use menu/context menu `Port->Delete Port`.

To create new device for selected port use menu/context menu `Port->New Device...`.

To add existing device for selected port use menu/context menu `Port->Add Existing...`.

To edit device and its reference(s)/unit(s) for selected port use menu/context menu `Port->Edit Device...`.

To delete device reference for selected port use menu/context menu `Port->Delete Device`.

Such settings as port type, port number, timeouts and others can be seen and modified using “Port Dialog` 
when create or edit port.

To import/export port use menu/context menu `Port->Import Port...` and `Port->Export Port...` respectively. 
For such import/export ‘xml’-format is used.

To edit active device use menu/context menu `Device->Edit Device...`.

To delete active device use menu/context menu `Device->Delete Device`.

Such settings as device unit addres, maximun amount of data to read/write and others can be seen and modified 
using “Device Dialog` when create or edit device.

To import/export device use menu/context menu `Device->Import Device...` and
`Device->Export Device...` respectively. 
For such import/export `xml`-format is used.

Also current selected port is displayed in status bar with its statistics.

## Device window {#sec_server_gui_device}

![](server_device_window.png)

In this window all device memory content can be seen.

For `0x`- and `1x`-memory each cell content bit with value `0` or `1`. 
It can be toggled when double-click left mouse button on specified cell.

For `3x`- and `4x`-memory each cell content 16-bit value which can be seen in different formats. 
This format is specified using `Format` drop down list in tool bar. It can be:
* `Bin` – 16-bit binary format;
* `Oct` – 16-bit octal format;
* `Dec` – 16-bit signed decimal format;
* `UDec` – 16-bit unsigned decimal format;
* `Hex` – 16-bit hexadecimal format;

To create new device use menu/context menu `Device->New Device...`. 
In this case, the new device will not be bound to any of the ports.

To edit device without reference(s)/unit(s) for port it belongs use menu/context menu `Device->Edit Device...`. 
It’s applied for active device window.

To delete device use menu/context menu `Device->Delete Device`. It’s applied for active device window.

To show those device windows use `Windows->Show All Devices` or `Windows->Show All Windows`.

Such settings as memory size and others can be seen and modified using `Device Dialog` when create or edit device.

To import/export device use menu/context menu `Device->Import Device...` and
`Device->Export Device...` respectively. 
For such import/export `xml`-format is used.

## DataView(s) windows {#sec_server_gui_dataview}

![](server_dataview_window.png)

In these windows, you can specify lists of items that are representation of current project device’s memory. 
There can be several such lists. Item can have several formats and size and belongs to different devices.

Each DataView list have its period to refresh data. This period can be set when creating new DataView list 
(`Data->New DataView ...`) or modified using `Data->Edit DataView ...`.

To create new DataView item(s) use menu/context menu `Data->New Item(s)...`. 
The addresses of the following elements will be automatically calculated as you add them. 

To edit selected DataView item(s) use menu/context menu `Data->Edit Item(s)...`. 

Menu/context menu `Data->Insert Item` or `Insert`-key inserts new item above selected or at the end of list 
if nothing selected. 
The addresses of the following elements will be automatically calculated as you add them.

To delete selected DataView item(s) use menu/context menu `Data->Delete Item` or `Delete`-key. 

There is copy/paste mechanism available in DataView item(s).

There is an import/export mechanism is also available in DataView. 
It can be import/export of selected item(s) in the list 
(`Data->Import Items...`/`Data->Export Items...`, `xml` or `csv` can be choosen) and 
import/export of whole list  (`Data->Import DataView...`/`Data->Export DataView...`,
for such import/export `xml` is used).

DataView item parameters can also be edited individually directly in the DataView list.

DataView columns can be customized using `Tools->Settings->DataView->Columns` globally or
`Data->Edit DataView->Columns` individually.

## Script Widnow

![](server_script_window.png)

Editor's window(s) to create/edit script source.

## Simulation window {#sec_server_gui_simulation}

![](server_simulation_window.png)

The `Simulation` window is designed to create and display elements of the simulation
of memory data of various devices.
Simulation element has such parameters as device it belongs, memory address,
data type on which memory size simulation depends,
period of change data and action type.

There are simulation types supported:
* `Increment` – each time increments memory with `value`-parameter (or decrements if `value` is negative);
* `Sine` – sine wave simulation with such sine parameters as
`Period`, `Phase Shift`, `Amplitude` and `Vertical shift`;
* `Random` – randomize value between `min` and `max`.
* `Copy` – copy memory blocks between one memory or even different memory types (`0x`, `1x`, `3x`, `4x`).
`Source` address means memory type and offset from which data is copied to
memory type and offset represented by `Address`.
`Size` means size of data array of type `Data Type` which will be copied.

There are data types supported for actions:
* `Bit`
* `Int8`
* `UInt8`
* `Int16`
* `UInt16`
* `Int32`
* `UInt32`
* `Int64`
* `UInt64`
* `Float32`
* `Double64`

To create new simulation action(s) use menu/context menu `Action->New Action(s)...`. 
The addresses of the following elements will be automatically calculated as you add them. 

To edit selected action(s) use menu/context menu `Action->Edit Action(s)...`. 

Menu/context menu `Action->Insert Action` inserts new element above selected or
at the end of list if nothing selected. 
The addresses of the following elements will be automatically calculated as you add them. 

To delete selected action(s) use menu/context menu `Action-> Action Item`.

There is an import/export mechanism is also available for actions.
It can be import/export of selected  action(s) in the list
`Action->Import Actions...`/`Action->Export Actions...`. For such import/export ‘xml’-format is used.

Each action parameters can also be edited individually directly in the `Action`-window.

## LogView window {#sec_server_gui_logview}

![](server_logview_window.png)

Window for displaying information about the exchange process, debugging information,
contents of Modbus network packets, etc. 
The information output can be customized using system settings dialog `Tools->Settings->Log`. 
Parameters descibed at `System settings`-dialog section.

`LogView` window has 2 buttons:
* `Clean` - clean up all messages from window;
* `Export` - export infomation into text file;

## Output window {#sec_server_gui_output}

![](server_output_window.png)

Window for displaying Python script standard output.

`Output` window has 1 buttons:
* `Clean` - clean up all messages from window;

## Menu {#sec_server_gui_menu}

Main menu provides access to all the features of the program. It consists of:

### File {#sec_server_gui_menu_file}

The `File` menu is intended for working with the project file and includes submenus:
* `New...` - open `Project`-dialog to create new project;
* `Open...` - open project from file that defined with standard open dialog. 
Standard file extension for server project is `.pjc`;
* `Recent` - contain list of recent opened projects and `Clear` button to clear the list. 
* `Save` - save project in previously defined file. Standard file extension for server project is `.pjc`;
* `Save As...` - save project in file that defined with standard save dialog.
Standard file extension for server project is `.pjc`;
* `Edit...` - open `Project`-dialog to edit current project parameters;
* `Info...` - open `ProjectInfo`-dialog;
* `Quit` - ends current application;

### Edit

The `Edit` menu provides standard operation to work with DataView items and
clipboard such as `Cut`, `Copy`, `Paste` and 
other keyboard operations such as `Insert`, `Delete`, `Select All`.

### View

The `View` menu opens windows (if it were previously closed): `Project`, `Simulation`, `LogView` and `Output`.

### Port

The `Port` menu provides access to work with port and includes submenus:
* `New Port...` - open `Port`-dialog to create new port;
* `Edit Port...` - open `Port`-dialog to edit selected in `Project`-window port parameters;
* `Delete Port` - delete selected in `Project`-window port parameters;
* `New Device...` - open `Device`-dialog to create new device for selected in `Project`-window port and 
bind newly created device to selected port;
* `Add Existing...` - open `Device`-dialog to add existring device for selected in `Project`-window port and 
set unit reference(s);
* `Edit Device...` - open `Device`-dialog to edit selected device for current port and edit unit reference(s);
* `Delete Device` - delete selected device from current port;
* `Import Port...` - import new port in project from file in `xml`-format;
* `Export Port...` - export selected port in project to file in `xml`-format;

### Device

The `Device` menu provides access to work with device and includes submenus:
* `New Device...` - open `Device`-dialog to create new device. 
Port for device can be choosen or create in Device dialog;
* `Edit Device...` - open `Device`-dialog to edit device parameters for active device. 
In this menu port for current device can be changed;
* `Delete Device` - delete active device;
* `Import Device...` - import new device in project from file in `xml`-format;
* `Export Device...` - export selected device in project to file in `xml`-format;
* `Zerro Memory` – zerro all memory values in current memory tab of active device;
* `Zerro Memory All` - zerro all memory values of all memory types of active device;
* `Import Memory...` - fill current memory values from file in `.csv` format;
* `Export memory` - dump current memory values to file in `.csv` format;
* `Script Init...` - open initialize script for current device;
* `Script Loop...` - open loop script for current device;
* `Script Final...` - open finalize script for current device;

### Data

The `Data` menu provides access to work with DataViews and DataViewItems and includes submenus:
* `New Item(s)...` - open `DataViewItems`-dialog to create new item(s) for active DataView;
* `Edit Item(s)...` - open `DataViewItems`-dialog to edit selected item(s) for active DataView;
* `Insert Item` - insert new item to active DataView. Address and other parameters will be calculated automaticaly;
* `Delete Items` - delete selected items for active DataView;
* `Import Items...` - import new items in active DataView from file in `xml` or `csv`-format;
* `Export Items...` - export selected items in active DataView to file in `xml` or `csv`-format;
* `New DataView...` - open `DataView`-dialog to create new DataView;
* `Edit DataView...` - open `DataView`-dialog to edit active DataView;
* `Insert DataView` - create new DataView;
* `Delete DataView` - delete active DataView;
* `Import DataView...` - import new DataView in project from file in `xml` or `csv`-format;
* `Export DataView...` - export active DataView to file in `xml` or `csv`-format;

### Tools
The `Tools` menu provides access to work with various system settings and includes submenus:
* `Settings...` - open `SystemSettings`-dialog;

### Simulation
The `Simulation` menu provides access to work with actions and includes submenus:
* `New Action(s)...` - open `Actions`-dialog to create new action(s);
* `Edit Action(s)...` - open `Actions`-dialog to edit selected action(s);
* `Insert Action` – insert new action to `Actions`-view window.
Address and other parameters will be calculated automaticaly;
* `Delete Actions` – delete selected actions;
* `Import Actions...` - import new actions from file in `xml`-format;
* `Export Actions...` - export selected items to file in `xml`-format;

### Runtime
The `Runtime` menu provides access to work with runtime execution and includes submenus:
* `Start/Stop` - run and stop current project for execution;

### Window
The "Window" menu provides access to work with MDI windows for DeviceViews and DataViews and includes submenus:
* `Show All Devices` - show all Device View MDI windows;
* `Show Active Device` - show active Device View MDI window;
* `Close All Devices` - close all Device View MDI windows;
* `Close Active Device` - close active Device View MDI window;
* `Show All DataViews` -  show all DataView MDI windows;
* `Show Active DataView` -  show active DataView MDI window;
* `Close All DataViews` -  close all DataView MDI windows;
* `Close Active DataView` – close active DataView MDI window;
* `Show All` – show all MDI windows;
* `Show Active` – show active MDI window;
* `Close All` – close all MDI windows;
* `Close Active` – close active MDI window;
* `Cascade` – located all visible windows in cascade way;
* `Tile` - located all visible windows in tile way;

### Help
The `Help` menu provides access to work with current `Help`-system and includes submenus:
* `About` - show short info about application;
* `About Qt` - show info about current used Qt version;
* `Content` - show current help content.

## Tool bar

![](server_toolbar.png)

This picture show explanation of every action in tool bar. All these actions 
were previously descibed in `Menu` section.

## Status bar

![](server_statusbar.png)

Status bar displays current working port (e.g. `Port[TCP:502]`) and its statistics:
count of transmitted (`Tx`) and received (`Rx`) Modbus network packets.
Port can be changed by selection in `Project Window`.

This picture shows current status of runtime as well: `Stopped` or `Running`.

# Dialogs

Server application have following list of dialog windows:
* `System Settings` – dialog for edit system settings;
* `Project` – dialog for create/edit current project settings;
* `ProjectInfo` – dialog for information about current project;
* `Port` – dialog for create/edit port settings;
* `Device` – dialog for create/edit device settings;
* `DataView` – dialog for create/edit DataView settings;
* `DataViewItem` – dialog for create/edit DataViewItem settings;
* `Action` – dialog for create/edit action settings.

## System Settings dialog

* `System Settings` - dialog shows all system settings that groups into separate windows.

### View 

![](server_settings_view.png)

* `Use Port/Device names with settings` - display settings info in `[]`-brackets. 
For port it displays port main settings, for device it displays device reference(s)
(Modbus device unit address(es) ).

* `Modbus adr. notation` - using this setting item address representation can be changed:
`Modbus (1-based)` or `IEC 61131-3 (0-based)`

### DataView 

![](server_settings_dataview.png)

* `Columns` - default DataView list columns can be customized: included/excluded, changed order.

### Log

![](server_settings_log.png)

* `Log Flags` - show log message categories that will be displayed in LogView;
* `Use timestamp` - display timestamp for log message in LogView;
* `DateTime Format` - set format for timestamp to be displayed in LogView. 

|Format        |Result         |
|--------------|---------------|
|dd.MM.yyyy    |21.05.2001     |
|ddd MMMM d yy |Tue May 21 01  |
|hh:mm:ss.zzz  |14:13:09.120   |
|hh:mm:ss.z    |14:13:09.12    |
|h:m:s ap      |2:13:9 pm      |          

Assumed that date and time are 21 May 2001 14:13:09.120.

### Script 

![](server_settings_scriptX.png)

#### Runtime

* `Enable Python Script` - enables/disables Python script execution. 
* `Use optimization` - enables/disables Python script source file generation. 

#### Editor

* `Generate usage examples in comments` - generate examples of script usage in comment in the head of script.
Maybe useful for first time using script. 
* `Font` - font setting for script source code displaying. 
* `Color` - color setting for script source code displaying. 
Included color schema for every time of script lexema (editing by mouse double-click)
and `Set Default` button.

#### Interpreters

Window for managing interpreter installed in operation system.
`Make Default` button set current interpreter that will be used to execute script.

## Project dialog

![](server_project_dialog.png)

`Project`-dialog displayed when creating/editing current project settings such as:
* `Name` - name of current project;
* `Author` - author of current project;
* `Comment` - some commentary for current project.

## ProjectInfo dialog

![](server_projectinfo_dialog.png)

`ProjectInfo`-dialog displayed when using menu `File`->`Info`.

## Port dialog

![](server_port_dialogx.png)

* `Port`-dialog displayed when creating/editing current port settings such as:
* `Name` - name of current port;
* `Type` - type of current port. Can be `TCP` for TCP/IP version and `RTU` or `ASC` for serial port version;
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

![](server_device_dialogx.png)

`Device`-dialog displayed when creating/editing current device settings such as:

* `Device` – list of the devices in project. Displayed only when `Port-> Add Existing…` menu activated;
* `Units` – device address/units references for selected port. Can be list as comma-separated and/or as range. 
For instance, `1-5,7-9, 11,15` means that this device can be accessed by remote client for current port 
using device addresses/units of 1,2,3,4,5,7,8,9,11,15;
* `Name` – name of current device;
* `Count 0x`, `Count 1x`, `Count 3x`, `Count 4x` – memory size of coils (0x), discrete inputs (1x), 
input (3x) and holding (4x) registers respectively;
* `Delay (msec)` – response delay for current device in milliseconds;
* `Save Data` – save devices memory content when save project;
* `Read Only` – set current device not to allow Modbus write messages for current device;
* `Read Coils`, `Read Discrete Inputs`, `Read Holding Registers`, `Read Input Registers`, `Write Mulptiple Coils`,
`Write Multiple Registers` – maximum value of the quantity parameter for the corresponding messages;
* `Exception Status` – address of memory that considering as exception status. Can be any type of memory;
* `Byte order` - default byte order within 16 bit register for device's items;
* `Register order` – default register order used for 32-bit size items and higher by default for current device;
* `Byte Array Format` – default byte array format items (used `ByteArray` as its format) for current device. 
Can be `Bin`, `Oct`, `Dec`, `UDec` and `Hex`; 
* `Byte Array Separator` – default separator for byte array format items
(used `ByteArray` as its format) for current device. 
May contain escape sequences: `\s` – space, `\r`-carriage return, `\n` – new line, `\t` – tab, `\\` - for `\`;
* `String Length Type` – default type of string length calculation for string format items: 
`ZerroEnded` – first `\0` means end of string and data right of it is not displayed, 
`Full Length` – display all string data with all `\0` characters;
* `String Encoding` – default string encoding for current device.
Encoding can be selected from list of supported encodings from Qt Framework (`QTextCodec`-class).

## DataView dialog

![](server_dataview_dialog.png)

`DataView` - dialog display information about created/edit DataView:
* `Name` - name of the current DataView;
* `Period` - period of values refreshing.
* `Adr. notation` - using this setting item address representation can be changed:
`Modbus (1-based)`,`IEC 61131-3 (0-based)` or `Default` (according to system setting)
* `Columns` - DataView list columns can be customized: included/excluded, changed order.
If `Use Default Columns` is checked then system setting `DataView columns` order 
setting will be used.

## DataViewItem dialog

![](server_dataviewitem_dialogx.png)

* `DataViewItem`- dialog for create/edit DataViewItem settings.
* `Device` - name of device current item belongs to;
* `Address` - memory address current item refers to;
* `Count` - count of items to be create/deleted (disable when editing item(s) ). 
Address of next item are calculated automaticaly according to it address and size;
* `Format` - item format, that describes item size. Can be `Bool`, `Bin16`, `Oct16`, `Dec16`, `UDec16`, `Hex16`, 
`Bin32`, `Oct32`, `Dec32`, `UDec32`, `Hex32`, `Bin64`, `Oct64`, `Dec64`, `UDec64`, `Hex64`, 
`Float`, `Double`, `ByteArray`, `String`;
* `Bytes` - size of current item in bytes. It's enable to edit only for `ByteArray` and `String` format;
* `Byte order` - byte order within 16 bit register current items;
* `Register order` - register order used for 32-bit size items and higher.
If `Default` current device register order is used;
* `Byte Array Format` - byte array format for item. Can be `Bin`, `Oct`, `Dec`, `UDec` and `Hex`.
If `Default` current device byte array format is used; 
* `Byte Array Separator` - separator for byte array format items (used `ByteArray` as its format). 
May contain escape sequences: `\s` - space, `\r`-carriage return, `\n` - new line, `\t` - tab, `\\` - for `\`. 
If `Default` current device byte array separator is used;
* `String Length Type` - type of string length calculation for string format items: 
`ZerroEnded` - first `\0` means end of string and data right of it is not displayed, 
`Full Length` - display all string data with all `\0` characters. 
If `Default` current device string length type is used;
* `String Encoding` - string encoding for current item.
Encoding can be selected from list of supported encodings from Qt Framework (`QTextCodec`-class).

### Byte order {#mbtools_byteorder}

Byte order changes byte order within 16-bit register for view:
* `LessSignifiedFirst` - little-endian
* `MostSignifiedFirst` - big-endian

### Register order {#mbtools_registerorder}

This setting defines register order for 2 register (32 bit) or 4 register (64 bit) 
DataView item's type.

For example 4 sequential registers

 `R0`     | `R1`     | `R2`     | `R3`
----------|----------|----------|----------
 `0x0000` | `0x1111` | `0x2222` | `0x3333`

will be displayed for `Hex64` format DataView item for different register order:

Register Order  | Value
----------------|-----------------------
 `R0R1R2R3`     | `0x3333222211110000`
 `R3R2R1R0`     | `0x0000111122223333`
 `R1R0R3R2`     | `0x2222333300001111`
 `R2R3R0R1`     | `0x1111000033332222`

## Simulation action dialog

![](server_simulation_dialogx.png)

`Action`-dialog for create/edit simulation actions settings.
* `Device` – name of device current action belongs to;
* `Address` – memory address current action refers to;
* `DataType` – data type of action, that describes item size.
Can be `Bit`, `Int8`, `UInt8`, `Int16`, `UInt16`,
`Int32`, `UInt32`, `Int64`, `UInt64`, `Float`, `Double`;
* `Count` – count of action to be create/deleted (disable when editing action(s) ). 
Address of next action are calculated automaticaly according to it address and size;
* `Type` – type of simulation action. There are action types supported:
    * `Increment` – each time increments memory with `value`-parameter (or decrements if `value` is negative);
    * `Sine` – sine wave simulation with such sine parameters as
`Period`, `Phase Shift`, `Amplitude` and `Vertical shift`;
    * `Random` – randomize value between `min` and `max`;
    * `Copy` – copy value from `Source` to `Address` array of `DataType`  with size `Size`;
* `Byte order` – byte order of current action;
* `Register order` – register order used for 32-bit size action and higher;

# Scripting

`server` application gives you access to the device's internal Modbus memory and
provides `Output` window where standard output is redirected.
The rest is the power of Python, its standard library, 3rd party libraries,
and your own libraries and scripts.

Every device has its own set of scripts: `Init`, `Loop` and `Final`.
Those scripts accessable through device menu or contex menu for device.

![](server_scripting.png)

`Init` script performs once at program start (when push `Start` button).
It intended for making python `import` instruction, create objects, files etc.
Modules, objects and files created within will be accessable from `Loop` and `Final` scripts.

`Loop` script performs cyclic until program not stopped.
It has implicit cycle so user don't have to cycle his program manualy.

`Final` script performs once at program stop (when push `Stop` button).
It intended for release resources previously created in `Init` and `Loop` scripts, save files etc.

Standard global objects for access corresponding device memory and its types: 
* `mem0x` (`mbserver._MemoryBlockBits`)
* `mem1x` (`mbserver._MemoryBlockBits`)
* `mem3x` (`mbserver._MemoryBlockRegs`)
* `mem4x` (`mbserver._MemoryBlockRegs`)

Every object has set of get/set function to work with different data types:
 * `mem0x`, `mem1x`: `get<datatype>(bitoffset:int)->int` and `set<datatype>(bitoffset:int,value:int)`
 * `mem3x`, `mem4x`: `get<datatype>(regoffset:int)->int` and `set<datatype>(regoffset:int,value:int)`

`<datatype>`: `int8`, `uint8`, `int16`, `uint16`, `int32`, `uint32`, `int64`, `uint64`, `float`, `double`.

Examples:
```python
 v = mem0x.getint8(0)
 mem1x.setint16(1, -1)
 mem3x.setuint16(0, 65535)
 mem4x.setdouble(10, 2.71828)
```

Also index operation is supported.
In case of discrete memory (`mem0x`, `mem1x`) it work with `boolean` values
and for registers memory (`mem3x`, `mem4x`) it work with `uint16` values:

```
b0 = mem0x[0]
mem1x[38] = True
mem3x[100] = 65535
if mem4x[0] > 32768:
    mem4x[0] = 0 
```

Scripting gives you access into current device settings by global object `mbdevice`
which has type `mbserver._MbDevice`. Example of usage:

```
 print("Device name is " + mbdevice.getname())
 print("Device exception status is " + hex(mbdevice.getexcstatus()))
 print("Device count of coils is " + hex(mbdevice.getcount0x()))
 print("Device count of discrete inputs is " + hex(mbdevice.getcount1x()))
 print("Device count of input registers is " + hex(mbdevice.getcount3x()))
 print("Device count of holding registers is " + hex(mbdevice.getcount4x()))
```

## Python `import` directories

You need to use Python modules in case of big simulation projects.
Or maybe you want to use you own Python modules for different devices within
single project or for different projects.
By default server add directory `<your_bin_folder>/script/server` into import path.
Also current folder where `*.pjs` project file is located added into import path.
