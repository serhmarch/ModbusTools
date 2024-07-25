#include "Modbus_unix.h"

#include <time.h>
#include <dirent.h>
#include <cstring>
#include <set>

namespace Modbus {

Timer timer()
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

void msleep(uint32_t msec) {
    struct timespec ts;
    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    nanosleep(&ts, NULL);
}

List<String> availableSerialPorts()
{
    std::set<String> portSet;

    DIR* dir = opendir("/dev");
    if (!dir)
        return List<String>();

    struct dirent* entry;
    while ((entry = readdir(dir)))
    {
        if (std::strncmp(entry->d_name, "ttyS", 4) == 0 ||
            std::strncmp(entry->d_name, "ttyUSB", 6) == 0 ||
            std::strncmp(entry->d_name, "ttyACM", 6) == 0)
            portSet.insert("/dev/" + String(entry->d_name));
    }

    closedir(dir);
    List<String> ports(portSet.begin(), portSet.end());
    return ports;
}

} // namespace Modbus
