#ifndef MODBUSSERVERRESOURCE_P_H
#define MODBUSSERVERRESOURCE_P_H

#include "ModbusPort.h"
#include "ModbusServerPort_p.h"

#define MBSERVER_SZ_VALUE_BUFF MB_VALUE_BUFF_SZ

class ModbusServerResourcePrivate : public ModbusServerPortPrivate
{
public:
    ModbusServerResourcePrivate(ModbusPort *port, ModbusInterface *device) :
        ModbusServerPortPrivate(device)
    {
        this->port = port;
        setPortError(port->lastErrorStatus());
        port->setServerMode(true);
    }

    ~ModbusServerResourcePrivate()
    {
        delete this->port;
    }

public:
    inline StatusCode lastPortErrorStatus() const { return port->lastErrorStatus(); }
    inline const Char *lastPortErrorText() const { return port->lastErrorText(); }
    inline const Char *getLastErrorText() const
    {
        if (isErrorPort)
            return lastPortErrorText();
        return errorText.data();
    }

    inline StatusCode setError(StatusCode status, const Char *text)
    {
        isErrorPort = false;
        return setErrorBase(status, text);
    }

    inline StatusCode setPortError(StatusCode status)
    {
        errorStatus = status;
        isErrorPort = true;
        return status;
    }

public:
    ModbusPort *port;
    uint8_t unit;
    uint8_t func;
    uint16_t offset;
    uint16_t count;
    uint8_t valueBuff[MBSERVER_SZ_VALUE_BUFF];
    bool isErrorPort;

};

inline ModbusServerResourcePrivate *d_ModbusServerResource(ModbusObjectPrivate *d_ptr) { return static_cast<ModbusServerResourcePrivate*>(d_ptr); }

#endif // MODBUSSERVERRESOURCE_P_H
