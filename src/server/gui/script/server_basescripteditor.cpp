#include "server_basescripteditor.h"

mbServerBaseScriptEditor::mbServerBaseScriptEditor(const Settings settings, QWidget *parent) : mbServerScriptEditor(settings, parent)
{

}

void mbServerBaseScriptEditor::changeName(const QString &newName)
{
    Q_EMIT nameChanged(getName(newName));
}

