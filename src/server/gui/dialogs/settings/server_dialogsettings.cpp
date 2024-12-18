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
    m_listWidget->addItem(QStringLiteral("View"));
    m_script = new mbServerWidgetSettingsScript(m_stackedWidget);
    m_stackedWidget->addWidget(m_script);
}

void mbServerDialogSettings::fillForm(const MBSETTINGS &m)
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    m_view->setUseNameWithSettings(m.value(sUi.settings_useNameWithSettings).toBool());

    m_log->setLogFlags(static_cast<mb::LogFlag>(m.value(sCore.settings_logFlags).toInt()));
    m_log->setFormatDateTime(m.value(sCore.settings_formatDateTime).toString());

}

void mbServerDialogSettings::fillData(MBSETTINGS &m)
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    m[sUi.settings_useNameWithSettings] = m_view->useNameWithSettings();

    m[sCore.settings_logFlags      ] = static_cast<int>(m_log->logFlags());
    m[sCore.settings_formatDateTime] = m_log->formatDateTime();
}
