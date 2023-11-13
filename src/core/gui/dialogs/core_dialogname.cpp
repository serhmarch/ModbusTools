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
#include "core_dialogname.h"
#include "ui_core_dialogname.h"

mbCoreDialogName::Strings::Strings() :
    title(QStringLiteral("Name"))
{
}

const mbCoreDialogName::Strings &mbCoreDialogName::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogName::mbCoreDialogName(QWidget *parent) : QDialog(parent),
    ui(new Ui::mbCoreDialogName)
{
    ui->setupUi(this);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mbCoreDialogName::~mbCoreDialogName()
{

}

QString mbCoreDialogName::getName(const QString &oldName, const QString &title)
{
    QString r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(oldName);
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbCoreDialogName::fillForm(const QString &name)
{
    ui->lblOldName->setText(name);
    ui->lnNewName->setText(name);
}

void mbCoreDialogName::fillData(QString &name)
{
    name = ui->lnNewName->text().trimmed();
}

