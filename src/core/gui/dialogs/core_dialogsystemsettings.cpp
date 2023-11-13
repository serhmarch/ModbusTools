/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "core_dialogsystemsettings.h"
#include "ui_core_dialogsystemsettings.h"

#include <core.h>
#include <gui/core_ui.h>

mbCoreDialogSystemSettings::Strings::Strings() :
    title(QStringLiteral("Settings"))
{

}

const mbCoreDialogSystemSettings::Strings &mbCoreDialogSystemSettings::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogSystemSettings::mbCoreDialogSystemSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbCoreDialogSystemSettings)
{
    ui->setupUi(this);

    const mbCore::Defaults &d = mbCore::Defaults::instance();
    ui->chbUseTimestamp->setChecked(d.settings_useTimestamp);
    ui->lnFormat->setText(d.settings_formatDateTime);
    fillFormLogFlags(d.settings_logFlags);
    ui->chbUseNameWithSettings->setChecked(mbCoreUi::Defaults::instance().settings_useNameWithSettings);

}

mbCoreDialogSystemSettings::~mbCoreDialogSystemSettings()
{
    delete ui;
}

bool mbCoreDialogSystemSettings::editSystemSettings(const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm();
    int r = QDialog::exec();
    switch (r)
    {
    case QDialog::Accepted:
        fillData();
        return true;
    }
    return false;
}

void mbCoreDialogSystemSettings::fillForm()
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    mbCore *core = mbCore::globalCore();

    MBSETTINGS settings = core->settings();

    ui->chbUseTimestamp->setChecked(settings.value(sCore.settings_useTimestamp).toBool());
    ui->lnFormat->setText(settings.value(sCore.settings_formatDateTime).toString());

    mb::LogFlags flags = mb::enumValue<mb::LogFlags>(settings.value(sCore.settings_logFlags));
    fillFormLogFlags(flags);
    ui->chbUseNameWithSettings->setChecked(settings.value(sUi.settings_useNameWithSettings).toBool());
}

void mbCoreDialogSystemSettings::fillFormLogFlags(mb::LogFlags flags)
{
    ui->chbLogError  ->setChecked(flags & mb::Log_Error  );
    ui->chbLogWarning->setChecked(flags & mb::Log_Warning);
    ui->chbLogInfo   ->setChecked(flags & mb::Log_Info   );
    ui->chbLogTxRx   ->setChecked(flags & mb::Log_TxRx   );
}

void mbCoreDialogSystemSettings::fillData()
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    mbCore *core = mbCore::globalCore();

    mb::LogFlags flags = static_cast<mb::LogFlags>(0);
    fillDataLogFlags(flags);

    MBSETTINGS settings;
    settings[sCore.settings_logFlags      ] = static_cast<uint>(flags);
    settings[sCore.settings_useTimestamp  ] = ui->chbUseTimestamp->isChecked();
    settings[sCore.settings_formatDateTime] = ui->lnFormat->text();

    settings[sUi.settings_useNameWithSettings] = ui->chbUseNameWithSettings->isChecked();

    core->setSettings(settings);
}

void mbCoreDialogSystemSettings::fillDataLogFlags(mb::LogFlags &flags)
{
    flags = static_cast<mb::LogFlags>(0);
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogError  ->isChecked() * mb::Log_Error   ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogWarning->isChecked() * mb::Log_Warning ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogInfo   ->isChecked() * mb::Log_Info    ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogError  ->isChecked() * mb::Log_TxRx    ));
}
