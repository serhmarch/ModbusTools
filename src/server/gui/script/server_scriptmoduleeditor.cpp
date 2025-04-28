#include "server_scriptmoduleeditor.h"

mbServerScriptModuleEditor::mbServerScriptModuleEditor(mbServerScriptModule *scriptModule,
                                                       const mbServerScriptEditor::Settings settings,
                                                       QWidget *parent) :
    mbServerBaseScriptEditor(settings, parent),
    m_scriptModule(scriptModule)
{
    connect(m_scriptModule, &mbServerScriptModule::nameChanged, this, &mbServerScriptModuleEditor::changeName);
    connect(m_scriptModule, &mbServerScriptModule::beginToGetSourceCode, this, &mbServerScriptModuleEditor::synchSourceCode);
}

QString mbServerScriptModuleEditor::name() const
{
    return getName(m_scriptModule->name());
}

QString mbServerScriptModuleEditor::getName(const QString &baseName) const
{
    return baseName+QStringLiteral(" : ScriptModule" );
}

void mbServerScriptModuleEditor::synchSourceCode()
{
    m_scriptModule->setSourceCode(this->toPlainText());
}
