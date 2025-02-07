# About Help

`Modbus Tools` for `client` and `server` applications is using `Qt Help Framework`.

Help system created using `doxygen` generator.
Help for `client` and `server` created separately in `doxygen` project files: 
`DoxyfileClient` and `DoxyfileServer`.

# Build Qt Help Files manualy

`doxygen` generator is intended to build `Qt Help` `index.qhp` project file. 
First of all need to create `DoxyfileClient`  with `QHP`-parameters 
(`DoxyfileServer` has same params but every entry for `client` must be replaced with `server`):

```conf
GENERATE_QHP       = yes
QCH_FILE           = ModbusClient
QHP_NAMESPACE      = "serhmarch.ModbusTools.Client"
QHP_VIRTUAL_FOLDER = "doc"
QHG_LOCATION       = ""
```

`QHG_LOCATION` (`qhelpgenerator` path) is empty because we need to create collections files manualy but later.
Must be created project collection file `ModbusClient.qhcp`:
```xml
<?xml version="1.0" encoding="utf-8" ?> 
<QHelpCollectionProject version="1.0">
    <docFiles>
        <generate>
            <file>
                <input>client/html/index.qhp</input>
                <output>ModbusClient.qch</output>
            </file>
        </generate>
        <register>
            <file>ModbusClient.qch</file>
        </register>
    </docFiles>
</QHelpCollectionProject>
```

Then `qhelpgenerator` must be executed, compiled help `*.qch` and compiled collection `*.qhc` files must be created:

```console
c:\Qt\5.15.2\msvc2019_64\bin\qhelpgenerator.exe "output/ModbusClient.qhcp"
c:\Qt\5.15.2\msvc2019_64\bin\qhelpgenerator.exe "output/ModbusServer.qhcp"
```

To check correctness of help generation Qt assistant can be used:
```console
c:\Qt\5.15.2\msvc2019_64\bin>assistant.exe -collectionFile ModbusClient.qhc
c:\Qt\5.15.2\msvc2019_64\bin>assistant.exe -collectionFile ModbusServer.qhc
```

# File System with help

Assumed application dir view:
* server.exe
* client.exe
* help : <dir>
    * ModbusClient.qch
    * ModbusClient.qhc
    * ModbusServer.qch
    * ModbusServer.qhc