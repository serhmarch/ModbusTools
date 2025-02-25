# Build ModbusTools

This page describes the process of building a ModbusTools project from source files.

## Building ModbusTools from source on Windows

### Preparation

The ModbusTools project requires Qt version 5.8 or later.

1. Download qt-online-installer, follow the link and choose Windows version for installer:

https://www.qt.io/download-qt-installer

2. Run qt-online-installer and install all necessary components: Qt 5 (Qt5.15.2) framework.

Qt 5.8 or higher is needed so in `Select Components` window check `Archive` checkbox and push `Filter` button.
Then is better to check latest version of Qt5.15 framework (e.g. 5.15.2)

3. Download and install git for Windows or use WSL and install git for WSL.

4. Make dir for binaries, e.g. in `<user-home-dir>\bin\ModbusTools` using Windows command console:
   ```console
   >cd `<user-home-dir>`
   >mkdir bin\ModbusTools
   >cd bin\ModbusTools
   ```
5. Clone repository:
   ```console
   $ git clone --recursive https://github.com/serhmarch/ModbusTools.git
   ```

### Build using qmake

This example shows how to build `ModbusTools` for MSVC compiler using `qmake`.

1. Open Windows command console and initialize all MSVC compiler vars (e.g. include, lib) using `vcvarsall.bat` script.
   This script will be configured for x64 arch (use `...\vcvarsall.bat -help` to display all options):
   `console

   > "c:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
   > `

2. Run qmake to produce `Makefile`.
   This command will make `Release` version, for `Debug` replase with `"CONFIG+=debug"`:
   `console

   > C:\Qt\5.15.2\msvc2019_64\bin\qmake.exe path\to\ModbusTools.pro -spec win32-msvc "CONFIG+=release"
   > `

3. Produce all make files:

   ```console
   >C:\Qt\Tools\QtCreator\bin\jom\jom.exe -f Makefile qmake_all
   ```

4. Build project from Makefile's:
   ```console
   >C:\Qt\Tools\QtCreator\bin\jom\jom.exe
   ```

### Build using cmake

1.  Run cmake to generate project (make) files.
    `CMAKE_PREFIX_PATH` must be set to installed Qt framewrok files:
    `console
    > cmake -DCMAKE_PREFIX_PATH:PATH=C:/Qt/5.15.2/msvc2019_64 -S path\to\ModbusTools -B .
    > `
2.  Make binaries (+ debug|release config):
    ```console
    $ cmake --build .
    $ cmake --build . --config Debug
    $ cmake --build . --config Release
    ```
3.  Resulting bin files is located in `Debug` or `Release` directory.

#### Build using CMakePresets.json

```console
cmake --preset "Win64-MSVC"
cmake --build --preset "Win64-MSVC-Debug"
cmake --build --preset "Win64-MSVC-Release"
```

## Building ModbusTools from source on Linux

### Preparation

The ModbusTools project requires Qt version 5.8 or later.

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
    $ sudo apt-get install qtbase5-dev qttools5-dev
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
    $ git clone --recursive https://github.com/serhmarch/ModbusTools.git
    ```

7.  Create and/or move to directory for build output, e.g. `~/bin/ModbusTools`:
    ```console
    $ cd ~
    $ mkdir -p bin/ModbusTools
    $ cd bin/ModbusTools
    ```

### Build using qmake

1.  Run qmake to create Makefile for build:

    ```console
    $ qmake ~/src/ModbusTools/src/ModbusTools.pro -spec linux-g++
    ```

2.  To ensure Makefile was created print:
    ```console
    $ ls -l
    total 36
    -rw-r--r-- 1 march march 35001 May  6 18:41 Makefile
    ```
3.  Finaly to make current set of programs print:

    ```console
    $ make
    ```

4.  After build step move to `<build_folder>/bin` (`<build_folder>/modbus/bin`)to ensure everything is correct:
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
    -rwxr-xr-x 1 march march 907872 May  6 18:47 server
    $ cd ~/bin/ModbusTools/modbus/bin
    $ ls -l
    total 264
    lrwxrwxrwx 1 march march     18 Sep 27 16:24 libmodbus.so -> libmodbus.so.1.0.0
    lrwxrwxrwx 1 march march     18 Sep 27 16:24 libmodbus.so.1 -> libmodbus.so.1.0.0
    lrwxrwxrwx 1 march march     18 Sep 27 16:24 libmodbus.so.1.0 -> libmodbus.so.1.0.0
    -rwxr-xr-x 1 march march 269888 Sep 27 16:24 libmodbus.so.1.0.0
    ```

### Build using cmake

1.  Run cmake to generate project (make) files.

    ```console
    $ cmake -S ~/src/ModbusTools -B .
    ```

2.  Make binaries (+ debug|release config):
    ```console
    $ cmake --build .
    $ cmake --build . --config Debug
    $ cmake --build . --config Release
    ```
3.  Resulting bin files is located in `./bin` directory.

#### Build using CMakePresets.json

```console
cmake --preset "Linux-Debug"
cmake --build --preset "Linux-Debug"

cmake --preset "Linux-Release"
cmake --build --preset "Linux-Release"
```

## Build Docker Image

Make sure you have a Xserver running on your host machine.

1. Build the Docker image

   ```console
   docker build -t modbustool .
   ```

2. Run the docker image
   ```console
   docker run --platform=linux/amd64 -e DISPLAY=host.docker.internal:0 modbustools
   ```
