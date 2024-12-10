#ifndef SERVER_DEVICESCRIPTEDITOR_H
#define SERVER_DEVICESCRIPTEDITOR_H

#include "server_scripteditor.h"
#include <project/server_device.h>

class mbServerDevice;

class mbServerDeviceScriptEditor : public mbServerScriptEditor
{
public:
    mbServerDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType, QWidget *parent = nullptr) :
        mbServerScriptEditor(parent), m_device(device), m_scriptType(scriptType) {}

public:
    mbServerDevice *device() const { return m_device; }
    mbServerDevice::ScriptType scriptType() const { return m_scriptType; }

private:
    mbServerDevice *m_device;
    mbServerDevice::ScriptType m_scriptType;
};

#endif // SERVER_DEVICESCRIPTEDITOR_H
