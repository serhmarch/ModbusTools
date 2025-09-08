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
#include "server_dialogscriptmodule.h"
#include "ui_server_dialogscriptmodule.h"

#include <core.h>
#include <project/server_scriptmodule.h>

mbServerDialogScriptModule::Strings::Strings() : mbCoreDialogEdit::Strings(),
    title(QStringLiteral("Script Module")),
    cachePrefix(QStringLiteral("Ui.Dialogs.ScriptModule."))
{
}

const mbServerDialogScriptModule::Strings &mbServerDialogScriptModule::Strings::instance()
{
    static const mbServerDialogScriptModule::Strings s;
    return s;
}

mbServerDialogScriptModule::mbServerDialogScriptModule(QWidget *parent) :
    mbCoreDialogEdit(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbServerDialogScriptModule)
{
    ui->setupUi(this);
    
    // ----------------------------------------------------------------------------------
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbServerDialogScriptModule::~mbServerDialogScriptModule()
{
    delete ui;
}

MBSETTINGS mbServerDialogScriptModule::cachedSettings() const
{
    const mbServerScriptModule::Strings &vs = mbServerScriptModule::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS m = mbCoreDialogEdit::cachedSettings();
    m[prefix+vs.name     ] = ui->lnName    ->text();
    m[prefix+vs.author   ] = ui->lnAuthor  ->text();
    m[prefix+vs.comment  ] = ui->txtComment->toPlainText();
    return m;
}

void mbServerDialogScriptModule::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogEdit::setCachedSettings(m);

    const mbServerScriptModule::Strings &vs = mbServerScriptModule::Strings::instance();
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.name   ); if (it != end) ui->lnName    ->setText     (it.value().toString());
    it = m.find(prefix+vs.author ); if (it != end) ui->lnAuthor  ->setText     (it.value().toString());
    it = m.find(prefix+vs.comment); if (it != end) ui->txtComment->setPlainText(it.value().toString());
}

MBSETTINGS mbServerDialogScriptModule::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;

    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    if (settings.count())
        fillForm(settings);
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbServerDialogScriptModule::fillForm(const MBSETTINGS &m)
{
    const mbServerScriptModule::Strings &vs = mbServerScriptModule::Strings::instance();
    
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(vs.name   ); if (it != end) ui->lnName    ->setText     (it.value().toString());
    it = m.find(vs.author ); if (it != end) ui->lnAuthor  ->setText     (it.value().toString());
    it = m.find(vs.comment); if (it != end) ui->txtComment->setPlainText(it.value().toString());
}

void mbServerDialogScriptModule::fillData(MBSETTINGS &settings)
{
    const mbServerScriptModule::Strings &s = mbServerScriptModule::Strings::instance();
    
    settings[s.name   ] = ui->lnName    ->text();
    settings[s.author ] = ui->lnAuthor  ->text();
    settings[s.comment] = ui->txtComment->toPlainText();
}
