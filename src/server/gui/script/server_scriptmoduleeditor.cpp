#include "server_scriptmoduleeditor.h"

mbServerScriptModuleEditor::mbServerScriptModuleEditor(mbServerScriptModule *scriptModule,
                                                       const mbServerScriptEditor::Settings settings,
                                                       QWidget *parent) :
    mbServerBaseScriptEditor(settings, parent),
    m_scriptModule(scriptModule)
{
    connect(scriptModule, &mbServerScriptModule::beginToGetSourceCode, this, &mbServerScriptModuleEditor::synchSourceCode);
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
