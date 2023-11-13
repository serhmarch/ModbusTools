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
#ifndef CLIENT_DOM_H
#define CLIENT_DOM_H

#include <project/core_dom.h>

// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------ DATA VIEW ------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbClientDomDataViewItem : public mbCoreDomDataViewItem
{
public:
    mbClientDomDataViewItem();
};

class mbClientDomDataView: public mbCoreDomDataView
{
public:
    mbClientDomDataView();
    mbCoreDomDataViewItem *newItem() const override { return new mbClientDomDataViewItem; }
};
// -----------------------------------------------------------------------------------------------------------------------
// -------------------------------------------------------- DEVICE -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbClientDomDevice : public mbCoreDomDevice
{
public:
    struct Strings : public mbCoreDomDevice::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    mbClientDomDevice();

private:
    mbClientDomDevice(const mbClientDomDevice &other);
    void operator =  (const mbClientDomDevice &other);
};


// -----------------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------- PORT --------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbClientDomPort : public mbCoreDomPort
{
public:
    struct Strings : public mbCoreDomPort::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    mbClientDomPort();

private:
    mbClientDomPort(const mbClientDomPort &other);
    void operator = (const mbClientDomPort &other);
};


// -----------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------- PROJECT -------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------

class mbClientDomPorts : public mbCoreDomPorts
{
public:
    mbClientDomPorts() : mbCoreDomPorts() {}
    mbCoreDomPort *newItem() const override { return new mbClientDomPort; }
};

class mbClientDomDevices : public mbCoreDomDevices
{
public:
    mbClientDomDevices() : mbCoreDomDevices() {}
    mbCoreDomDevice *newItem() const override { return new mbClientDomDevice; }
};

class mbClientDomDataViews : public mbCoreDomDataViews
{
public:
    mbClientDomDataViews() : mbCoreDomDataViews() {}
    mbCoreDomDataView *newItem() const override { return new mbClientDomDataView; }
};

class mbClientDomProject : public mbCoreDomProject
{
public:
    mbClientDomProject();
    ~mbClientDomProject();

private:
    mbClientDomProject(const mbClientDomProject& other);
    void operator = (const mbClientDomProject& other);
};

#endif // CLIENT_DOM_H
