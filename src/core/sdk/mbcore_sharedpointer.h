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
#ifndef MBCORE_SHAREDPOINTER_H
#define MBCORE_SHAREDPOINTER_H

#include <cstddef>
#include <utility>

namespace mb {

template <class T>
class SharedPointer
{
public:
    // constructors
    SharedPointer()
    {
        m_node = nullptr;
    }

    SharedPointer(std::nullptr_t)
    {
        m_node = nullptr;
    }

    template <class X>
    SharedPointer(X *ptr)
    {
        if (ptr)
            ptr->incRef();
        m_node = ptr;
    }

    SharedPointer(const SharedPointer &other)
    {
        if (other.m_node)
            other.m_node->incRef();
        m_node = other.m_node;
    }

    template <class X>
    SharedPointer(const SharedPointer<X> &other)
    {
        if (other.m_node)
            other.m_node->incRef();
        m_node = other.m_node;
    }

    SharedPointer(SharedPointer &&other)
    {
        m_node = other.m_node;
        other.m_node = nullptr;
    }

    template <class X>
    SharedPointer(SharedPointer<X> &&other)
    {
        m_node = other.m_node;
        other.m_node = nullptr;
    }

    ~SharedPointer()
    {
        if (m_node)
            m_node->decRef();
    }


public:
    // basic accessor functions
    inline T *data() const { return m_node; }
    inline T *get() const { return data(); }
    inline bool isNull() const { return m_node == nullptr; }
    inline operator bool() const { return m_node != nullptr; }
    inline bool operator!() const { return isNull(); }
    inline T &operator*() const { return *m_node; }
    inline T *operator->() const { return m_node; }
    inline operator T*() const { return data(); }
    inline void swap(SharedPointer<T> &other)
    {
        T *n = m_node;
        m_node = other.m_node;
        other.m_node = n;
    }

    inline SharedPointer &operator=(const SharedPointer &other)
    {
        SharedPointer copy(other);
        swap(copy);
        return *this;
    }

    template <class X>
    inline SharedPointer &operator=(const SharedPointer<X> &other)
    {
        SharedPointer copy(other);
        swap(copy);
        return *this;
    }

    inline SharedPointer &operator=(const SharedPointer &&other)
    {
        SharedPointer moved(std::move(other));
        swap(moved);
        return *this;
    }

    template <class X>
    inline SharedPointer &operator=(const SharedPointer<X> &&other)
    {
        SharedPointer moved(std::move(other));
        swap(moved);
        return *this;
    }

    inline void clear()
    {
        if (m_node)
            m_node->decRef();
    }

    inline void reset() { clear(); }

    inline void reset(T *t)
    {
        SharedPointer<T> other(t);
        swap(other);
    }

    // casts:
    template <class X> inline SharedPointer<X> staticCast()  const { return SharedPointer<X>(static_cast<X*>(m_node)); }
    template <class X> inline SharedPointer<X> dynamicCast() const { return SharedPointer<X>(dynamic_cast<X*>(m_node)); }
    template <class X> inline SharedPointer<X> constCast()   const { return SharedPointer<X>(const_cast<X*>(m_node)); }

private:
    T *m_node;
};

template<class T, class X> inline bool operator==(const SharedPointer<T> &ptr1, const SharedPointer<X> &ptr2) { return ptr1.m_node == ptr2.m_node; }
template<class T, class X> inline bool operator!=(const SharedPointer<T> &ptr1, const SharedPointer<X> &ptr2) { return ptr1.m_node != ptr2.m_node; }
template<class T, class X> inline bool operator==(const SharedPointer<T> &ptr1, const X *ptr2) { return ptr1.m_node == ptr2; }
template<class T, class X> inline bool operator!=(const SharedPointer<T> &ptr1, const X *ptr2) { return ptr1.m_node != ptr2; }
template<class T, class X> inline bool operator==(const T *ptr1, const SharedPointer<X> &ptr2) { return ptr1 == ptr2.m_node; }
template<class T, class X> inline bool operator!=(const T *ptr1, const SharedPointer<X> &ptr2) { return ptr1 != ptr2.m_node; }

} // mb

#endif // MBCORE_SHAREDPOINTER_H
