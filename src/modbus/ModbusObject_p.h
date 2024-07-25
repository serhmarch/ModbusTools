#ifndef MODBUSOBJECT_P_H
#define MODBUSOBJECT_P_H

#include <list>
#include <unordered_map>

#include "Modbus.h"

using namespace Modbus;

class ModbusObjectPrivate
{
public:
    typedef std::list<void*> SignalSlots;
    typedef std::unordered_map<void*, SignalSlots> SignalHash;
    using const_iterator = SignalHash::const_iterator;
    using iterator = SignalHash::iterator;

public:
    virtual ~ModbusObjectPrivate()
    {
    }

public:
    inline SignalHash::const_iterator begin() const { return signalHash.begin(); }
    inline SignalHash::const_iterator end() const { return signalHash.end(); }
    inline SignalHash::const_iterator find(void *signal) const { return signalHash.find(signal); }

    inline SignalHash::const_iterator cbegin() const { return signalHash.cbegin(); }
    inline SignalHash::const_iterator cend() const { return signalHash.cend(); }

    inline SignalHash::iterator begin() { return signalHash.begin(); }
    inline SignalHash::iterator end() { return signalHash.end(); }
    inline SignalHash::iterator find(void *&signal) { return signalHash.find(signal); }

    inline iterator erase( iterator pos ) { return signalHash.erase(pos); }
    inline iterator erase( const_iterator pos ) { return signalHash.erase(pos); }
    inline iterator erase( const_iterator first, const_iterator last ) { return signalHash.erase(first, last); }

    inline SignalSlots& operator[]( void* &signal ) { return signalHash[signal]; }
    inline SignalSlots& operator[]( void *&& signal ) { return signalHash[signal]; }

public:
    String objectName;
    SignalHash signalHash;
};

#endif // MODBUSOBJECT_P_H
