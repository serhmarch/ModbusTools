/*!
 * \file   ModbusObject.h
 * \brief  The header file defines the class templates used to create signal/slot-like mechanism
 *
 * \author march
 * \date   May 2024
 */
#ifndef MODBUSOBJECT_H
#define MODBUSOBJECT_H

#include "Modbus.h"

/// \brief `ModbusMethodPointer`-pointer to class method template type
template <class T, class ReturnType, class ... Args>
using ModbusMethodPointer = ReturnType(T::*)(Args...);

/// \brief `ModbusFunctionPointer` pointer to function template type
template <class ReturnType, class ... Args>
using ModbusFunctionPointer = ReturnType (*)(Args...);

/// \brief `ModbusSlotBase` base template for slot (method or function)
template <class ReturnType, class ... Args>
class ModbusSlotBase
{
public:
    /// \details Virtual destructor of the class
    virtual ~ModbusSlotBase() {}

    /// \details Return pointer to object which method belongs to (in case of method slot) or
    /// `nullptr` in case of function slot
    virtual void *object() const { return nullptr; }

    /// \details Return pointer to method (in case of method slot) or function (in case of function slot)
    virtual void *methodOrFunction() const = 0;

    /// \details Execute method or function slot
    virtual ReturnType exec(Args ...  args) = 0;
};



/// \brief `ModbusSlotMethod` template class hold pointer to object and its method
template <class T, class ReturnType, class ... Args>
class ModbusSlotMethod : public ModbusSlotBase<ReturnType, Args ...>
{
public:
    /// \details Constructor of the slot.
    /// \param[in]  object      Pointer to object.
    /// \param[in]  methodPtr   Pointer to object's method.
    ModbusSlotMethod(T* object, ModbusMethodPointer<T, ReturnType, Args...> methodPtr) : m_object(object), m_methodPtr(methodPtr) {}

public:
    void *object() const override { return m_object; }
    void *methodOrFunction() const override { return reinterpret_cast<void*>(m_voidPtr); }

    ReturnType exec(Args ...  args) override
    {
        return (m_object->*m_methodPtr)(args...);
    }

private:
    T* m_object;
    union
    {
        ModbusMethodPointer<T, ReturnType, Args...> m_methodPtr;
        void *m_voidPtr;
    };
};


/// \brief `ModbusSlotFunction` template class hold pointer to slot function
template <class ReturnType, class ... Args>
class ModbusSlotFunction : public ModbusSlotBase<ReturnType, Args ...>
{
public:
    /// \details Constructor of the slot.
    /// \param[in]  funcPtr   Pointer to slot function.
    ModbusSlotFunction(ModbusFunctionPointer<ReturnType, Args...> funcPtr) : m_funcPtr(funcPtr) {}

public:
    void *methodOrFunction() const override { return m_voidPtr; }
    ReturnType exec(Args ...  args) override
    {
        return m_funcPtr(args...);
    }

private:
    union
    {
        ModbusFunctionPointer<ReturnType, Args...> m_funcPtr;
        void *m_voidPtr;
    };
};

class ModbusObjectPrivate;

/*! \brief The `ModbusObject` class is the base class for objects that use signal/slot mechanism.

    \details `ModbusObject` is designed to be a base class for objects that need to use 
    simplified Qt-like signal/slot mechanism. 
    User can connect signal of the object he want to listen to his own function or method
    of his own class and then it can be disconnected if he is not interesting of this signal anymore.
    Callbacks will be called in order which it were connected.

    `ModbusObject` has a map which key means signal identifier (pointer to signal) and 
    value is a list of callbacks functions/methods connected to this signal.

    `ModbusObject` has `objectName()` and `setObjectName` methods. This methods can be used to 
    simply identify object which is signal's source (e.g. to print info in console).

    \note `ModbusObject` class is not thread safe
 */

class MODBUS_EXPORT ModbusObject
{
public:
    /// \details Returns a pointer to the object that sent the signal. 
    /// This pointer is valid in thread where signal was occured only. 
    /// So this function must be called only within the slot that is a callback of signal occured.
    static ModbusObject *sender();

public:
    /// \details Constructor of the class.
    ModbusObject();

    /// \details Virtual destructor of the class.
    virtual ~ModbusObject();

public:
    /// \details Returns a pointer to current object's name string. 
    const Modbus::Char *objectName() const;

    /// \details Set name of current object. 
    void setObjectName(const Modbus::Char *name);

public:
    /// \details Connect `this` object's signal `signalMethodPtr` to the `object`s method `objectMethodPtr`.
    /// ```cpp
    /// class MyClass : public ModbusObject { public: void signalSomething(int a, int b) { emitSignal(&MyClass::signalSomething, a, b); } };
    /// class MyReceiver { public: void slotSomething(int a, int b) { doSomething(); } };
    /// MyClass c;
    /// MyReceiver r;
    /// c.connect(&MyClass::signalSomething, r, &MyReceiver::slotSomething);
    /// ```
    /// \note `SignalClass` template type refers to any class but it must be `this` or derived class. 
    /// It makes separate `SignalClass` to easely refers signal of the derived class.
    template <class SignalClass, class T, class ReturnType, class ... Args>
    void connect(ModbusMethodPointer<SignalClass, ReturnType, Args ...> signalMethodPtr, T *object, ModbusMethodPointer<T, ReturnType, Args ...> objectMethodPtr)
    {
        ModbusSlotMethod<T, ReturnType, Args ...> *slotMethod = new ModbusSlotMethod<T, ReturnType, Args ...>(object, objectMethodPtr);
        union {
            ModbusMethodPointer<SignalClass, ReturnType, Args ...> signalMethodPtr;
            void* voidPtr;
        } converter;
        converter.signalMethodPtr = signalMethodPtr;
        setSlot(converter.voidPtr, slotMethod);
    }

    /// \details Same as `ModbusObject::connect(ModbusMethodPointer, T*, ModbusMethodPointer)` 
    /// but connects `ModbusFunctionPointer` to current object's signal `signalMethodPtr`.
    template <class SignalClass, class ReturnType, class ... Args>
    void connect(ModbusMethodPointer<SignalClass, ReturnType, Args ...> signalMethodPtr, ModbusFunctionPointer<ReturnType, Args ...> funcPtr)
    {
        ModbusSlotFunction<ReturnType, Args ...> *slotFunc = new ModbusSlotFunction<ReturnType, Args ...>(funcPtr);
        union {
            ModbusMethodPointer<SignalClass, ReturnType, Args ...> signalMethodPtr;
            void* voidPtr;
        } converter;
        converter.signalMethodPtr = signalMethodPtr;
        setSlot(converter.voidPtr, slotFunc);
    }

    /// \details Disconnects function `funcPtr` from all signals of current object. 
    template <class ReturnType, class ... Args>
    inline void disconnect(ModbusFunctionPointer<ReturnType, Args ...> funcPtr)
    {
        disconnect(nullptr, funcPtr);
    }

    /// \details Disconnects function `funcPtr` from all signals of current object, but `funcPtr` is a void pointer.
    inline void disconnectFunc(void *funcPtr)
    {
        disconnect(nullptr, funcPtr);
    }

    /// \details Disconnects slot represented by pair `(object, objectMethodPtr)` from all signals of current object. 
    template <class T, class ReturnType, class ... Args>
    inline void disconnect(T *object, ModbusMethodPointer<T, ReturnType, Args ...> objectMethodPtr)
    {
        union {
            ModbusMethodPointer<T, ReturnType, Args ...> objectMethodPtr;
            void* voidPtr;
        } converter;
        converter.objectMethodPtr = objectMethodPtr;
        disconnect(object, converter.voidPtr);
    }

    /// \details Disconnect all slots of `T *object` from all signals of current object. 
    template <class T>
    inline void disconnect(T *object)
    {
        disconnect(object, nullptr);
    }


protected:
    /// \details Template method for emit signal. Must be called from within of the signal method. 
    template <class T, class ... Args>
    void emitSignal(const char *thisMethodId, ModbusMethodPointer<T, void, Args ...> thisMethod, Args ... args)
    {
        dummy = thisMethodId; // Note: present because of weird MSVC compiler optimization, 
                              // when diff signals can have same address
        //printf("Emit signal: %s\n", thisMethodId);
        union {
            ModbusMethodPointer<T, void, Args ...> thisMethod;
            void* voidPtr;
        } converter;
        converter.thisMethod = thisMethod;

        pushSender(this);
        int i = 0;
        while (void* itemSlot = slot(converter.voidPtr, i++))
        {
            ModbusSlotBase<void, Args...> *slotBase = reinterpret_cast<ModbusSlotBase<void, Args...> *>(itemSlot);
            slotBase->exec(args...);
        }
        popSender();
    }

private:
    void *slot(void *signalMethodPtr, int i) const;
    void setSlot(void *signalMethodPtr, void *slotPtr);
    void disconnect(void *object, void *methodOrFunc);

private:
    static void pushSender(ModbusObject *sender);
    static void popSender();

protected:
    /// \cond
    static const char* dummy; // Note: prevent weird MSVC compiler optimization
    ModbusObjectPrivate *d_ptr;
    ModbusObject(ModbusObjectPrivate *d);
    /// \endcond
};

#endif // MODBUSOBJECT_H
