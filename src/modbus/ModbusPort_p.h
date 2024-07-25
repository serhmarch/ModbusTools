#ifndef MODBUSPORT_P_H
#define MODBUSPORT_P_H

#include "ModbusObject_p.h"

class ModbusPort;

namespace ModbusPortPrivateNS {

enum State
{
    STATE_BEGIN = 0,
    STATE_UNKNOWN = STATE_BEGIN,
    STATE_WAIT_FOR_OPEN,
    STATE_OPENED,
    STATE_PREPARE_TO_READ,
    STATE_WAIT_FOR_READ,
    STATE_WAIT_FOR_READ_ALL,
    STATE_PREPARE_TO_WRITE,
    STATE_WAIT_FOR_WRITE,
    STATE_WAIT_FOR_WRITE_ALL,
    STATE_WAIT_FOR_CLOSE,
    STATE_CLOSED,
    STATE_END = STATE_CLOSED
};

} // namespace ModbusPortPrivateNS

using namespace Modbus;
using namespace ModbusPortPrivateNS;

class ModbusPortPrivate
{
public:
    ModbusPortPrivate(bool blocking)
    {
        this->state = STATE_UNKNOWN;
        this->modeServer = false;
        this->modeSynch = blocking;
        this->clearChanged();
    }

    virtual ~ModbusPortPrivate()
    {
    }

public:
    inline bool isBlocking() const { return modeSynch; }
    inline bool isStateClosed() const { return state == STATE_CLOSED; }
    inline void setChanged(bool changed) { this->changed = changed; }
    inline void clearChanged() { setChanged(false); }
    inline StatusCode lastErrorStatus() { return errorStatus; }
    inline const Char *lastErrorText() { return errorText.data(); }
    inline StatusCode setError(StatusCode status, const String &text) { errorStatus = status; errorText = text; return status; }
    inline StatusCode setError(StatusCode status, String &&text) { errorStatus = status; errorText = text; return status; }

public:
    State state;
    bool changed;
    bool modeServer;
    bool modeSynch;
    StatusCode errorStatus;
    String errorText;
};

#endif // MODBUSPORT_P_H
