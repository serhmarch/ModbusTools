FROM ubuntu:22.04 as build

WORKDIR /app
# tells apt not to prompt for user input, debconf for package configuration, and setting it to noninteractive ensures that any configuration questions are skipped or assigned default values.
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y build-essential
RUN apt install -y qtbase5-dev qttools5-dev
RUN apt install -y git
RUN apt install -y cmake

RUN git clone --recursive https://github.com/serhmarch/ModbusTools.git

WORKDIR /app/bin
# build
RUN cmake -S /app/ModbusTools -B .
RUN cmake --build .
RUN cmake --build . --config Debug
RUN cmake --build . --config Release

COPY <<'EOF' ./entrypoint.sh
#!/bin/bash
./server &
./client 
EOF
RUN chmod +x ./entrypoint.sh

# we need the display env var to be set to the proper host's Xserver in order to be able to run a GUI app in the docker container
ENV DISPLAY=

ENTRYPOINT ["/bin/bash", "-c", "./entrypoint.sh"]