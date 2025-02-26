FROM ubuntu:22.04

WORKDIR /app
# tells apt not to prompt for user input, debconf for package configuration, and setting it to noninteractive ensures that any configuration questions are skipped or assigned default values.
ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y build-essential git cmake
RUN apt install -y qtbase5-dev qttools5-dev

RUN git clone --recursive https://github.com/serhmarch/ModbusTools.git

WORKDIR /app/bin
# build
RUN cmake -S /app/ModbusTools -B .
RUN cmake --build .
RUN cmake --build . --config Debug
RUN cmake --build . --config Release

RUN yes | rm -r /app/ModbusTools

# install vnc server and vnc proxy to make the vnc server accessible via web
RUN apt install -y xvfb x11vnc fluxbox novnc supervisor

COPY <<'EOF' supervisord.conf
[supervisord]
nodaemon=true
logfile=/dev/null  
loglevel=debug      

[program:Xvfb]
command=/usr/bin/Xvfb :1 -screen 0 1600x900x16
priority=1

[program:x11vnc]
command=/usr/bin/x11vnc -display :1 -ncache 0 -forever -shared -nopw
priority=2

[program:fluxbox]
command=/usr/bin/fluxbox
environment=DISPLAY=:1
priority=3

[program:novnc]
command=/usr/bin/websockify --web=/usr/share/novnc/ --cert=/home/ubuntu/novnc.pem 6080 localhost:5900
priority=4

[program:server]
command=/app/bin/server
environment=DISPLAY=:1
priority=5

[program:client]
command=/app/bin/client
environment=DISPLAY=:1
priority=6
EOF

EXPOSE 6080

CMD ["/usr/bin/supervisord", "-c", "supervisord.conf"]