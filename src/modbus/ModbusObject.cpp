#include "ModbusObject.h"
#include "ModbusObject_p.h"

#include <list>

thread_local std::list<ModbusObject*> thl_senders;
const char *ModbusObject::dummy = nullptr; // Note: prevent weird MSVC compiler optimization

ModbusObject *ModbusObject::sender()
{
    if (thl_senders.size())
        return thl_senders.front();
    return nullptr;
}

void ModbusObject::pushSender(ModbusObject *sender)
{
    thl_senders.push_front(sender);
}

void ModbusObject::popSender()
{
    thl_senders.pop_front();
}

ModbusObject::ModbusObject() :
    ModbusObject(new ModbusObjectPrivate)
{
}

ModbusObject::ModbusObject(ModbusObjectPrivate *d) :
    d_ptr(d)
{
}

ModbusObject::~ModbusObject()
{
    for (ModbusObjectPrivate::const_iterator it = d_ptr->begin(); it != d_ptr->end(); it++)
    {
        for (void *ptr : it->second)
        {
            delete reinterpret_cast<ModbusSlotBase<void>*>(ptr);
        }
    }
    delete d_ptr;
}

const Char *ModbusObject::objectName() const
{
    return d_ptr->objectName.data();
}

void ModbusObject::setObjectName(const Modbus::Char *name)
{
    d_ptr->objectName = name;
}

void *ModbusObject::slot(void *signalMethodPtr, int i) const
{
    ModbusObjectPrivate::const_iterator it = d_ptr->find(signalMethodPtr);
    if (it != d_ptr->end())
    {
        std::list<void*>::const_iterator lit = it->second.begin();
        std::advance(lit, i);
        if (lit != it->second.end())
            return *lit;
    }
    return nullptr;
}

void ModbusObject::setSlot(void *signalMethodPtr, void *slotPtr)
{
    (*d_ptr)[signalMethodPtr].push_back(slotPtr);
}

void ModbusObject::disconnect(void *object, void *methodOrFunc)
{
    for (ModbusObjectPrivate::iterator it = d_ptr->begin(); it != d_ptr->end(); )
    {
        for (std::list<void*>::iterator i = it->second.begin(); i != it->second.end(); )
        {
            ModbusSlotBase<void> *callback = reinterpret_cast<ModbusSlotBase<void>*>(*i);
            bool del = false;
            if (object)
            {
                del = ((callback->object() == object) && ((methodOrFunc == nullptr) || (callback->methodOrFunction() == methodOrFunc)));
            }
            else if (callback->methodOrFunction() == methodOrFunc)
            {
                del = true;
            }
            if (del)
            {
                i = it->second.erase(i);
                delete callback;
                continue;
            }
            i++;
        }
        if (it->second.size() == 0)
        {
            it = d_ptr->erase(it);
            continue;
        }
        it++;
    }
}
