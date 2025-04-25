#include "server_devicescripteditor.h"

mbServerDeviceScriptEditor::mbServerDeviceScriptEditor(mbServerDevice *device,
                                                       mbServerDevice::ScriptType scriptType,
                                                       const mbServerScriptEditor::Settings settings,
                                                       QWidget *parent) :
    mbServerBaseScriptEditor(settings, parent), m_device(device),
    m_scriptType(scriptType)
{
    connect(m_device, &mbServerDevice::nameChanged, this, &mbServerDeviceScriptEditor::changeName);
}

QString mbServerDeviceScriptEditor::name() const
{
    return getName(m_device->name());
}

QString mbServerDeviceScriptEditor::getName(const QString &baseName) const
{
    QString sScriptType;
    switch (m_scriptType)
    {
    case mbServerDevice::Script_Init:  sScriptType = QStringLiteral(" : ScriptInit" ); break;
    case mbServerDevice::Script_Loop:  sScriptType = QStringLiteral(" : ScriptLoop" ); break;
    case mbServerDevice::Script_Final: sScriptType = QStringLiteral(" : ScriptFinal"); break;
    }
    return baseName+sScriptType;
}
