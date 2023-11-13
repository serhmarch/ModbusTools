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
#include "core_global.h"

namespace mb {

MBSETTINGS parseExtendedAttributesStr(const QString &str)
{
    MBSETTINGS res;
    QStringList lsPairs = str.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    Q_FOREACH(QString sPair, lsPairs)
    {
        QStringList keyValue = sPair.split(QLatin1Char('='), Qt::KeepEmptyParts);
        if (keyValue.count() == 2)
        {
            QString key = keyValue.at(0).trimmed();
            QString value = keyValue.at(1).trimmed();
            res.insert(key, value);
        }
    }
    return res;
}

} // namespace mb
