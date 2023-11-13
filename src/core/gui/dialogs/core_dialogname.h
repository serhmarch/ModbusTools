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
#ifndef CORE_DIALOGNAME_H
#define CORE_DIALOGNAME_H

#include <QDialog>

#include <mbcore.h>

namespace Ui {
class mbCoreDialogName;
}

class MB_EXPORT mbCoreDialogName : public QDialog
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString title;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreDialogName(QWidget *parent = nullptr);
    ~mbCoreDialogName();

public:
    QString getName(const QString &oldName = QString(), const  QString &title = QString());

private:
    void fillForm(const QString &name);
    void fillData(QString &name);

private:
    Ui::mbCoreDialogName *ui;
};

#endif // CORE_DIALOGNAME_H
