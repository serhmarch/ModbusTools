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
#ifndef CORE_HELPUI_H
#define CORE_HELPUI_H

#include <QMainWindow>
#include <mbcore.h>

class QHelpEngine;

class mbCoreHelpUi : public QMainWindow
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings
    {
        const QString prefix;
        const QString wGeometry;
        const QString wState;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreHelpUi(const QString &relativeCollectionFile, QWidget *parent = nullptr);
    ~mbCoreHelpUi();

public:
    MBSETTINGS cachedSettings() const;
    void setCachedSettings(const MBSETTINGS &settings);

private:
    QHelpEngine *m_helpEngine;
};

#endif // CORE_HELPUI_H
