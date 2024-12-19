#include "server_dialogsettings.h"

#include <QListWidget>
#include <QStackedWidget>

#include <core.h>
#include <gui/core_ui.h>

#include "server_widgetsettingsscript.h"

mbServerDialogSettings::Strings::Strings() //:
    //title(QStringLiteral("Settings")),
    //cachePrefix(QStringLiteral("Ui.Dialogs.SystemSettings."))
{
}

const mbServerDialogSettings::Strings &mbServerDialogSettings::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogSettings::mbServerDialogSettings(QWidget *parent) :
    mbCoreDialogSettings(parent)
{
    m_listWidget->addItem(QStringLiteral("Script"));
    m_script = new mbServerWidgetSettingsScript(m_stackedWidget);
    m_stackedWidget->addWidget(m_script);
}

void mbServerDialogSettings::fillForm(const MBSETTINGS &m)
{
    mbCoreDialogSettings::fillForm(m);
}

void mbServerDialogSettings::fillData(MBSETTINGS &m)
{
    mbCoreDialogSettings::fillData(m);
}
