#ifndef SERVER_DEVICESCRIPTEDITOR_H
#define SERVER_DEVICESCRIPTEDITOR_H

#include "server_scripteditor.h"
#include <project/server_device.h>

class mbServerDevice;

class mbServerDeviceScriptEditor : public mbServerScriptEditor
{
    Q_OBJECT

public:
    mbServerDeviceScriptEditor(mbServerDevice *device, mbServerDevice::ScriptType scriptType, QWidget *parent = nullptr);

public:
    mbServerDevice *device() const { return m_device; }
    mbServerDevice::ScriptType scriptType() const { return m_scriptType; }
    QString name() const;

Q_SIGNALS:
    void nameChanged(const QString& newName);

private Q_SLOTS:
    void changeName(const QString& newName);

private:
    QString getName(const QString& deviceName) const;

private:
    mbServerDevice *m_device;
    mbServerDevice::ScriptType m_scriptType;
};

#endif // SERVER_DEVICESCRIPTEDITOR_H
