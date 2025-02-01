#include "server_devicescripteditor.h"

mbServerDeviceScriptEditor::mbServerDeviceScriptEditor(mbServerDevice *device,
                                                       mbServerDevice::ScriptType scriptType,
                                                       const mbServerScriptHighlighter::ColorFormats formats,
                                                       QWidget *parent) :
    mbServerScriptEditor(formats, parent), m_device(device),
    m_scriptType(scriptType)
{
    connect(m_device, &mbServerDevice::nameChanged, this, &mbServerDeviceScriptEditor::changeName);
}

QString mbServerDeviceScriptEditor::name() const
{
    return getName(m_device->name());
}

void mbServerDeviceScriptEditor::changeName(const QString &newName)
{
    Q_EMIT nameChanged(getName(newName));
}

QString mbServerDeviceScriptEditor::getName(const QString &deviceName) const
{
    QString sScriptType;
    switch (m_scriptType)
    {
    case mbServerDevice::Script_Init:  sScriptType = QStringLiteral(" : ScriptInit" ); break;
    case mbServerDevice::Script_Loop:  sScriptType = QStringLiteral(" : ScriptLoop" ); break;
    case mbServerDevice::Script_Final: sScriptType = QStringLiteral(" : ScriptFinal"); break;
    }
    return deviceName+sScriptType;
}
