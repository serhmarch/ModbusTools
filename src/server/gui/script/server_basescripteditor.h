#ifndef SERVER_BASESCRIPTEDITOR_H
#define SERVER_BASESCRIPTEDITOR_H

#include "editor/server_scripteditor.h"

class mbServerBaseScriptEditor : public mbServerScriptEditor
{
    Q_OBJECT

public:
    mbServerBaseScriptEditor(const mbServerScriptEditor::Settings settings,
                             QWidget *parent = nullptr);
public:
    virtual QString name() const = 0;

Q_SIGNALS:
    void nameChanged(const QString& newName);

protected Q_SLOTS:
    void changeName(const QString& newName);

protected:
    virtual QString getName(const QString& deviceName) const = 0;
};

#endif // SERVER_BASESCRIPTEDITOR_H
