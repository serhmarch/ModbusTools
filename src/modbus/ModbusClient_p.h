#ifndef MODBUSCLIENT_P_H
#define MODBUSCLIENT_P_H

#include <string>

#include <string.h>
#include <cstring>

#include "ModbusObject_p.h"

#include "ModbusClientPort.h"

class ModbusClientPrivate : public ModbusObjectPrivate
{
public:
    uint8_t unit;
    ModbusClientPort *port;
    Modbus::StatusCode lastStatus;
    Modbus::StatusCode lastErrorStatus;
    Modbus::String lastErrorText;
};

inline ModbusClientPrivate *d_ModbusClient(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusClientPrivate*>(d_ptr); }

#endif // MODBUSCLIENT_P_H
