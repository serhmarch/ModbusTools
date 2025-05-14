#ifndef SERVER_DEVICESCRIPTEDITOR_H
#define SERVER_DEVICESCRIPTEDITOR_H

#include "server_basescripteditor.h"
#include <project/server_device.h>

class mbServerDevice;

class mbServerDeviceScriptEditor : public mbServerBaseScriptEditor
{
    Q_OBJECT

public:
    mbServerDeviceScriptEditor(mbServerDevice *device,
                               mbServerDevice::ScriptType scriptType,
                               const mbServerScriptEditor::Settings settings,
                               QWidget *parent = nullptr);

public:
    inline mbServerDevice *device() const { return m_device; }
    inline mbServerDevice::ScriptType scriptType() const { return m_scriptType; }
    QString name() const override;

public Q_SLOTS:
    void synchSourceCode();

private:
    QString getName(const QString& baseName) const override;

private:
    mbServerDevice *m_device;
    mbServerDevice::ScriptType m_scriptType;
};

#endif // SERVER_DEVICESCRIPTEDITOR_H
