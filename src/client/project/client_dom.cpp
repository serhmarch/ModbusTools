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
#include "client_dom.h"

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DATA VIEW ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbClientDomDataViewItem::mbClientDomDataViewItem() : mbCoreDomDataViewItem()
{
}

mbClientDomDataView::mbClientDomDataView() : mbCoreDomDataView()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbClientDomDevice::Strings::Strings() : mbCoreDomDevice::Strings()
{
}

const mbClientDomDevice::Strings &mbClientDomDevice::Strings::instance()
{
    static Strings s;
    return s;
}

mbClientDomDevice::mbClientDomDevice() : mbCoreDomDevice()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbClientDomPort::Strings::Strings() : mbCoreDomPort::Strings()
{
}

const mbClientDomPort::Strings &mbClientDomPort::Strings::instance()
{
    static Strings s;
    return s;
}

mbClientDomPort::mbClientDomPort() : mbCoreDomPort()
{
}

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

mbClientDomProject::mbClientDomProject() : mbCoreDomProject(new mbClientDomPorts,
                                                            new mbClientDomDevices,
                                                            new mbClientDomDataViews)
{
}

mbClientDomProject::~mbClientDomProject()
{
}
