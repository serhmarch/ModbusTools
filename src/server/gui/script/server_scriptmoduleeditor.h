#ifndef SERVER_SCRIPTMODULEEDITOR_H
#define SERVER_SCRIPTMODULEEDITOR_H

#include "server_basescripteditor.h"
#include <project/server_scriptmodule.h>

class mbServerScriptModuleEditor : public mbServerBaseScriptEditor
{
    Q_OBJECT

public:
    mbServerScriptModuleEditor(mbServerScriptModule *scriptModule,
                               const mbServerScriptEditor::Settings settings,
                               QWidget *parent = nullptr);

public:
    inline mbServerScriptModule *scriptModule() const { return m_scriptModule; }
    QString name() const override;

private:
    QString getName(const QString& baseName) const override;

private Q_SLOTS:
    void synchSourceCode();

private:
    mbServerScriptModule *m_scriptModule;
};

#endif // SERVER_SCRIPTMODULEEDITOR_H
