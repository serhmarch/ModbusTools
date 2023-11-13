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
#include "core_dialogproject.h"
#include "ui_core_dialogproject.h"

#include <project/core_project.h>

mbCoreDialogProject::Strings::Strings() :
    title(QStringLiteral("Project"))
{
}

const mbCoreDialogProject::Strings &mbCoreDialogProject::Strings::instance()
{
    static const mbCoreDialogProject::Strings s;
    return s;
}

mbCoreDialogProject::mbCoreDialogProject(QWidget *parent) :
    mbCoreDialogSettings(parent),
    ui(new Ui::mbCoreDialogProject)
{
    ui->setupUi(this);
    
    // ----------------------------------------------------------------------------------
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbCoreDialogProject::~mbCoreDialogProject()
{
    delete ui;
}

MBSETTINGS mbCoreDialogProject::getSettings(const MBSETTINGS &settings, const QString &title)
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

void mbCoreDialogProject::fillForm(const MBSETTINGS &settings)
{
    const mbCoreProject::Strings &s = mbCoreProject::Strings::instance();
    
    ui->lnName    ->setText     (settings.value(s.name   ).toString());
    ui->lnAuthor  ->setText     (settings.value(s.author ).toString());
    ui->txtComment->setPlainText(settings.value(s.comment).toString());
}

void mbCoreDialogProject::fillData(MBSETTINGS &settings)
{
    const mbCoreProject::Strings &s = mbCoreProject::Strings::instance();
    
    settings[s.name   ] = ui->lnName    ->text();
    settings[s.author ] = ui->lnAuthor  ->text();
    settings[s.comment] = ui->txtComment->toPlainText();
}
