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
#ifndef CLIENT_RUNDEVICE_H
#define CLIENT_RUNDEVICE_H

#include <QQueue>
#include <QReadWriteLock>

#include <client_global.h>

class mbClientRunItem;

class mbClientRunDevice
{
public:
    mbClientRunDevice(const Modbus::Settings &settings);
    virtual ~mbClientRunDevice();

public: // settings
    inline QString  name                       () const { QReadLocker _(&m_lock); return m_settings.name                     ; }
    inline uint8_t  unit                       () const { QReadLocker _(&m_lock); return m_settings.unit                     ; }
    inline uint16_t maxReadCoils               () const { QReadLocker _(&m_lock); return m_settings.maxReadCoils             ; }
    inline uint16_t maxReadDiscreteInputs      () const { QReadLocker _(&m_lock); return m_settings.maxReadDiscreteInputs    ; }
    inline uint16_t maxReadInputRegisters      () const { QReadLocker _(&m_lock); return m_settings.maxReadInputRegisters    ; }
    inline uint16_t maxReadHoldingRegisters    () const { QReadLocker _(&m_lock); return m_settings.maxReadHoldingRegisters  ; }
    inline uint16_t maxWriteMultipleCoils      () const { QReadLocker _(&m_lock); return m_settings.maxWriteMultipleCoils    ; }
    inline uint16_t maxWriteMultipleRegisters  () const { QReadLocker _(&m_lock); return m_settings.maxWriteMultipleRegisters; }

public:
    void pushItemsToRead(const QList<mbClientRunItem*> &itemsToRead);
    bool popItemsToRead(QList<mbClientRunItem*> &items);

public:
    void pushItemsToWrite(const QList<mbClientRunItem*> &items);
    void pushItemToWrite(mbClientRunItem *item);
    bool popItemsToWrite(QList<mbClientRunItem*> &items);

public:
    bool hasExternalMessage() const;
    void pushExternalMessage(const mbClientRunMessagePtr &message);
    bool popExternalMessage(mbClientRunMessagePtr *message);

private:
    void setSettings(const Modbus::Settings &settings);

private:
    mutable QReadWriteLock m_lock;

private:
    struct
    {
        QString  name                     ;
        uint8_t  unit                     ;
        uint16_t maxReadCoils             ;
        uint16_t maxReadDiscreteInputs    ;
        uint16_t maxReadInputRegisters    ;
        uint16_t maxReadHoldingRegisters  ;
        uint16_t maxWriteMultipleCoils    ;
        uint16_t maxWriteMultipleRegisters;
    } m_settings;

private:
    QList<mbClientRunItem*> m_itemsToRead;
    QQueue<mbClientRunItem*> m_itemsToWrite;
    QQueue<mbClientRunMessagePtr> m_externalMessages;
};

#endif // CLIENT_RUNDEVICE_H
