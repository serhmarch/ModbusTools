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
#ifndef SERVER_GLOBAL_H
#define SERVER_GLOBAL_H

#include <mbcore.h>
#include <core_global.h>

namespace mb {

enum FindFlags
{
    FindBackward        = 0x00001,
    FindCaseSensitively = 0x00002,
    FindWholeWords      = 0x00004
};

} // namespace mb

#endif // SERVER_GLOBAL_H
