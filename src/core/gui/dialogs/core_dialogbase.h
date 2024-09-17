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
#ifndef CORE_DIALOGBASE_H
#define CORE_DIALOGBASE_H

#include <QDialog>

#include <mbcore.h>

class MB_EXPORT mbCoreDialogBase : public QDialog
{
public:
    struct MB_EXPORT Strings
    {
        const QString wGeometry;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogBase(const QString &cachePrefix, QWidget *parent = nullptr);

public:
    virtual MBSETTINGS cachedSettings() const;
    virtual void setCachedSettings(const MBSETTINGS &);

protected:
    QString m_cachePrefix;
};

#endif // CORE_DIALOGBASE_H
