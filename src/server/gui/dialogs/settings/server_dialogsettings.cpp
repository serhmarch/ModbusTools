#include "server_dialogsettings.h"

#include <QListWidget>
#include <QStackedWidget>

#include <server.h>

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
    const mbServer::Strings &s = mbServer::Strings::instance();
    mbCoreDialogSettings::fillForm(m);

    m_script->setScriptEnable           (m.value(s.settings_scriptEnable         ).toBool      ());
    m_script->setScriptUseOptimization  (m.value(s.settings_scriptUseOptimization).toBool      ());
    m_script->scriptSetManualExecutables(m.value(s.settings_scriptManual         ).toStringList());
    m_script->scriptSetDefaultExecutable(m.value(s.settings_scriptDefault        ).toString    ());
}

void mbServerDialogSettings::fillData(MBSETTINGS &m)
{
    const mbServer::Strings &s = mbServer::Strings::instance();
    mbCoreDialogSettings::fillData(m);
    m[s.settings_scriptEnable         ] = m_script->scriptEnable           ();
    m[s.settings_scriptUseOptimization] = m_script->scriptUseOptimization  ();
    m[s.settings_scriptManual         ] = m_script->scriptManualExecutables();
    m[s.settings_scriptDefault        ] = m_script->scriptDefaultExecutable();

}
