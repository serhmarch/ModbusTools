/*
    Modbus Tools

    Created: 2026
    Author: Afonso Nobrega, https://github.com/nobrega8

    Copyright (C) 2026  Afonso Nobrega

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
#ifndef CORE_MBMAP_H
#define CORE_MBMAP_H

#include <mbcore.h>

/*! \brief Reads device maps in the .mbmap format, version 2
    (https://github.com/nobrega8/mbmap-library).

    \details A map lists the registers of one device model: area, address,
    type, byte and word order, name and unit. Each register becomes the
    settings of one data view item, as importDataViewItemsCsv() makes them.
    Bits of a register (`bool` with `bit`, and bit fields) share one `Bin16`
    item whose comment names them.
 */
class MBTOOLS_EXPORT mbCoreMbMap
{
public:
    struct MBTOOLS_EXPORT Strings
    {
        const QString suffix;   ///< ".mbmap.json"
        Strings();
        static const Strings &instance();
    };

public:
    //! Returns true when \c file is named like a map (*.mbmap.json, any case)
    static bool isMapFile(const QString &file);

    /*! \brief Reads a map.
        \param data Content of the file, UTF-8, with or without a byte order mark
        \param name Set to "Vendor Model", for the data view's name
        \param error Set to the reason when \c data is not a map that can be read
        \return Settings of one data view item per register or shared register,
        in the order of the map; registers with errors are left out
     */
    static QList<MBSETTINGS> parse(const QByteArray &data, QString *name, QString *error);
};

#endif // CORE_MBMAP_H
