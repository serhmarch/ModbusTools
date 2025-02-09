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
#ifndef CORE_GLOBAL_H
#define CORE_GLOBAL_H

class QComboBox;
class QSpinBox;

#include <mbcore.h>

namespace mb {

MB_EXPORT MBSETTINGS parseExtendedAttributesStr(const QString &str);

/// \details Return list of baud rates
MB_EXPORT QVariantList availableBaudRate();

/// \details Return list of data bits
MB_EXPORT QVariantList availableDataBits();

/// \details Return list of `Parity` values
MB_EXPORT QVariantList availableParity();

/// \details Return list of `StopBits` values
MB_EXPORT QVariantList availableStopBits();

/// \details Return list of `FlowControl` values
MB_EXPORT QVariantList availableFlowControl();

/// \details
MB_EXPORT mb::Address getModbusAddress(QComboBox *cmb, QSpinBox *sp, mb::AddressNotation notation);

/// \details
MB_EXPORT void setModbusAddress(QComboBox *cmb, QSpinBox *sp, mb::Address adr, mb::AddressNotation notation);

/// \details
MB_EXPORT void fillModbusAddressUi(QComboBox *cmb, QSpinBox *sp, mb::Address address, mb::AddressNotation notation);

} // namespace mb

#endif // CORE_GLOBAL_H
