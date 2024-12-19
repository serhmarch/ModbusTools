#include "client_dialogsettings.h"

#include <QListWidget>
#include <QStackedWidget>

#include <core.h>
#include <gui/core_ui.h>

mbClientDialogSettings::Strings::Strings() //:
    //title(QStringLiteral("Settings")),
    //cachePrefix(QStringLiteral("Ui.Dialogs.SystemSettings."))
{
}

const mbClientDialogSettings::Strings &mbClientDialogSettings::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogSettings::mbClientDialogSettings(QWidget *parent) :
    mbCoreDialogSettings(parent)
{
}

void mbClientDialogSettings::fillForm(const MBSETTINGS &m)
{
    mbCoreDialogSettings::fillForm(m);
}

void mbClientDialogSettings::fillData(MBSETTINGS &m)
{
    mbCoreDialogSettings::fillData(m);
}
