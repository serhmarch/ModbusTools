------------------------------------------------------------------------------------------------------------
    Help system
------------------------------------------------------------------------------------------------------------
Libre Office Writer (+plugin Writer2xhtml) was used to create this help system.
Writer2xhtml generate HTML code for help system.

Table of content and Index was created by Qt Help, using project '.qhp' and collection '.qhcp' xml files:
>qhelpgenerator ModbusClient.qhcp 
>qhelpgenerator ModbusServer.qhcp 

As result .qhc (collection) and .qch (project) files was created.

To check correctness of help generation qt assistant can be used:
>assistant -collectionFile ModbusClient.qhc
>assistant -collectionFile ModbusServer.qhc

Assumed application dir view:
<application dir>:
    - server.exe
    - client.exe
    - help : <dir>
        - ModbusClient.qch
        - ModbusClient.qhc
        - ModbusServer.qch
        - ModbusServer.qhc
    ...
