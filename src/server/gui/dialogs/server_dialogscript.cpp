#include "server_dialogscript.h"
#include "ui_server_dialogscript.h"

#include <server.h>
#include <project/server_project.h>
#include <project/server_device.h>

mbServerDialogScript::Strings::Strings() :
    title      (QStringLiteral("Script")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Script."))
{
}

const mbServerDialogScript::Strings &mbServerDialogScript::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogScript::mbServerDialogScript(QWidget *parent) :
    mbCoreDialogEdit(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbServerDialogScript)
{
    ui->setupUi(this);

    ui->tabWidget->setCurrentWidget(ui->tabLoop);
}

mbServerDialogScript::~mbServerDialogScript()
{
    delete ui;
}

MBSETTINGS mbServerDialogScript::cachedSettings() const
{
    MBSETTINGS m = mbCoreDialogEdit::cachedSettings();
    return m;
}

void mbServerDialogScript::setCachedSettings(const MBSETTINGS &settings)
{
    mbCoreDialogEdit::setCachedSettings(settings);
}

MBSETTINGS mbServerDialogScript::getSettings(const MBSETTINGS &scripts, const QString &title)
{
    MBSETTINGS r;

    mbServerProject *project = mbServer::global()->project();
    if (!project)
        return r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(scripts);

    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbServerDialogScript::fillForm(const MBSETTINGS &script)
{
    const mbServerDevice::Strings &s = mbServerDevice::Strings::instance();

    ui->txtInit ->setPlainText(script.value(s.scriptInit ).toString());
    ui->txtLoop ->setPlainText(script.value(s.scriptLoop ).toString());
    ui->txtFinal->setPlainText(script.value(s.scriptFinal).toString());

}

void mbServerDialogScript::fillData(MBSETTINGS &script)
{
    const mbServerDevice::Strings &s = mbServerDevice::Strings::instance();

    script[s.scriptInit ] = ui->txtInit ->toPlainText();
    script[s.scriptLoop ] = ui->txtLoop ->toPlainText();
    script[s.scriptFinal] = ui->txtFinal->toPlainText();
}
