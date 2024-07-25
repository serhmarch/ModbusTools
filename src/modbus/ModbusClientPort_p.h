#ifndef MODBUSCLIENTPORT_P_H
#define MODBUSCLIENTPORT_P_H

#include "ModbusObject_p.h"

#include "ModbusObject.h"
#include "ModbusPort.h"

namespace ModbusClientPortPrivateNS {

enum State
{
    STATE_BEGIN = 0,
    STATE_UNKNOWN, // = STATE_BEGIN,
    STATE_WAIT_FOR_OPEN,
    STATE_OPENED,
    STATE_BEGIN_WRITE,
    STATE_WRITE,
    STATE_BEGIN_READ,
    STATE_READ,
    STATE_WAIT_FOR_CLOSE,
    STATE_CLOSED,
    STATE_END = STATE_CLOSED
};

} // namespace ModbusClientPortPrivateNS

using namespace ModbusClientPortPrivateNS;

class ModbusClientPortPrivate : public ModbusObjectPrivate
{
public:
    ModbusClientPortPrivate(ModbusPort *port)
    {
        this->state = STATE_UNKNOWN;
        this->unit = 0;
        this->func = 0;
        this->block = false;;
        this->currentClient = nullptr;
        this->port = port;
        this->repeats = 0;
        this->settings.repeatCount = 1;
        this->lastStatus = Modbus::Status_Uncertain;
        this->lastErrorStatus = Modbus::Status_Uncertain;
        this->isLastPortError = true;

        port->setServerMode(false);
    }

    ~ModbusClientPortPrivate()
    {
        delete this->port;
    }

public:
    inline bool isWriteBufferBlocked() const  { return block; }

    inline void blockWriteBuffer() { block = true; }

    inline void freeWriteBuffer() { block = false; }

    inline const Char *getName() const { return currentClient->objectName(); }

    inline StatusCode setPortError(StatusCode status)
    {
        lastStatus = status;
        lastErrorStatus = status;
        isLastPortError = true;
        return status;
    }

    inline StatusCode setPortStatus(StatusCode status)
    {
        if (StatusIsBad(status))
            return setPortError(status);
        lastStatus = status;
        return status;
    }

    inline StatusCode setError(StatusCode status, const String &text)
    {
        lastStatus      = status;
        lastErrorStatus = status;
        lastErrorText   = text;
        isLastPortError = true;
        return status;
    }

    inline StatusCode setError(StatusCode status, String &&text)
    {
        lastStatus      = status;
        lastErrorStatus = status;
        lastErrorText   = text;
        isLastPortError = true;
        return status;
    }

    inline StatusCode setGoodStatus()
    {
        lastStatus = Modbus::Status_Good;
        return lastStatus;
    }

public:
    ModbusPort *port;
    State state;
    uint8_t unit;
    uint8_t func;
    bool block;
    ModbusObject *currentClient;
    uint32_t repeats;
    uint8_t buff[MB_VALUE_BUFF_SZ];
    StatusCode lastStatus;
    StatusCode lastErrorStatus;
    String lastErrorText;
    bool isLastPortError;

    struct
    {
        uint32_t repeatCount;
    } settings;

};

inline ModbusClientPortPrivate *d_ModbusClientPort(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusClientPortPrivate*>(d_ptr); }

#endif // MODBUSCLIENTPORT_P_H
