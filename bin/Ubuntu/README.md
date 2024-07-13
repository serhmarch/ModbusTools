# Building ModbusTools from source on Ubuntu

The ModbusTools project requires Qt version 5.15. Therefore, at least Ubuntu 22.04 LTS is required.
Ubuntu 20.04 has qttools with Qt version 5.12.


1.  Update package list:
    ```console
    $ sudo apt-get update
    ```

2.  Install main build tools like g++, make etc:
    ```console
    $ sudo apt-get install build-essential
    ```

3.  Install Qt tools:
    ```console
    $ sudo apt-get install qtbase5-dev qttools5-dev libqt5serialport5-dev
    ```

4.  Check for correct instalation:
    ```console
    $ whereis qmake
    qmake: /usr/bin/qmake
    $ whereis libQt5Core*
    libQt5Core.prl: /usr/lib/x86_64-linux-gnu/libQt5Core.prl
    libQt5Core.so: /usr/lib/x86_64-linux-gnu/libQt5Core.so
    libQt5Core.so.5: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5
    libQt5Core.so.5.15: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.15
    libQt5Core.so.5.15.3: /usr/lib/x86_64-linux-gnu/libQt5Core.so.5.15.3
    $ whereis libQt5Help*
    libQt5Help.prl: /usr/lib/x86_64-linux-gnu/libQt5Help.prl
    libQt5Help.so: /usr/lib/x86_64-linux-gnu/libQt5Help.so
    libQt5Help.so.5: /usr/lib/x86_64-linux-gnu/libQt5Help.so.5
    libQt5Help.so.5.15: /usr/lib/x86_64-linux-gnu/libQt5Help.so.5.15
    libQt5Help.so.5.15.3: /usr/lib/x86_64-linux-gnu/libQt5Help.so.5.15.3
    $ whereis libQt5SerialPort*
    libQt5SerialPort.prl: /usr/lib/x86_64-linux-gnu/libQt5SerialPort.prl
    libQt5SerialPort.so: /usr/lib/x86_64-linux-gnu/libQt5SerialPort.so
    libQt5SerialPort.so.5: /usr/lib/x86_64-linux-gnu/libQt5SerialPort.so.5
    libQt5SerialPort.so.5.15: /usr/lib/x86_64-linux-gnu/libQt5SerialPort.so.5.15
    libQt5SerialPort.so.5.15.3: /usr/lib/x86_64-linux-gnu/libQt5SerialPort.so.5.15.3
    ```

5.  Install git:
    ```console
    $ sudo apt-get install git
    ```

6.  Create project directory, move to it and clone repository:
    ```console
    $ cd ~
    $ mkdir src
    $ cd src
    $ git clone https://github.com/serhmarch/ModbusTools.git
    ```

7.  Create and/or move to directory for build output, e.g. `~/bin/ModbusTools`:
    ```console
    $ cd ~
    $ mkdir -p bin/ModbusTools
    $ cd bin/ModbusTools
    ```

8.  Run qmake to create Makefile for build:
    ```console
    $ qmake ~/src/ModbusTools/src/ModbusTools.pro -spec linux-g++
    ```

9.  To ensure Makefile was created print:
    ```console
    $ ls -l
    total 36
    -rw-r--r-- 1 march march 35001 May  6 18:41 Makefile
    ```
10. Finaly to make current set of programs print:
    ```console
    $ make
    ```

11. After build step move to `<build_folder>/bin` to ensure everything is correct:
    ```console
    $ cd bin
    $ pwd
    ~/bin/ModbusTools/bin
    $ ls -l
    total 2672
    -rwxr-xr-x 1 march march 643128 May  6 18:45 client
    lrwxrwxrwx 1 march march     16 May  6 18:44 libcore.so -> libcore.so.0.1.4
    lrwxrwxrwx 1 march march     16 May  6 18:44 libcore.so.0 -> libcore.so.0.1.4
    lrwxrwxrwx 1 march march     16 May  6 18:44 libcore.so.0.1 -> libcore.so.0.1.4
    -rwxr-xr-x 1 march march 993376 May  6 18:44 libcore.so.0.1.4
    lrwxrwxrwx 1 march march     18 May  6 18:43 libmodbus.so -> libmodbus.so.0.1.0
    lrwxrwxrwx 1 march march     18 May  6 18:43 libmodbus.so.0 -> libmodbus.so.0.1.0
    lrwxrwxrwx 1 march march     18 May  6 18:43 libmodbus.so.0.1 -> libmodbus.so.0.1.0
    -rwxr-xr-x 1 march march 183336 May  6 18:43 libmodbus.so.0.1.0
    -rwxr-xr-x 1 march march 907872 May  6 18:47 server
    ```
