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

void mbServerDeviceScriptEditor::synchSourceCode()
{
    switch (m_scriptType)
    {
    case mbServerDevice::Script_Init : m_device->setScriptInit (this->toPlainText()); break;
    case mbServerDevice::Script_Loop : m_device->setScriptLoop (this->toPlainText()); break;
    case mbServerDevice::Script_Final: m_device->setScriptFinal(this->toPlainText()); break;
    }
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
