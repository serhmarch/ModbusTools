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
#ifndef SERVER_RUNACTION_H
#define SERVER_RUNACTION_H

#include <QtMath>
#include <QVariant>

#include <mbcore.h>
#include <project/server_action.h>

class mbServerDevice;

class mbServerRunAction
{
public:
    mbServerRunAction(const MBSETTINGS &settings);
    virtual ~mbServerRunAction();

public:
    inline mbServerDevice *device() const { return m_device; }
    inline mb::Address address() const { return m_address; }
    virtual mb::DataType dataType() const = 0;
    inline int period() const { return m_period; }
    QVariant value() const;
    void setValue(const QVariant &value);
    void trySwap(void *d, int size);

public:
    virtual int init(qint64 time);
    virtual int exec(qint64 time);
    virtual int final(qint64 time);

protected:
    mbServerDevice *m_device;
    mb::Address m_address;
    int m_period;
    qint64 m_last;
    mb::DataOrder m_byteOrder;
    mb::DataOrder m_registerOrder;
};

template <typename T>
class mbServerRunActionT : public mbServerRunAction
{
public:
    mbServerRunActionT(const MBSETTINGS &settings) : mbServerRunAction(settings) {}
    mb::DataType dataType() const override { return mb::dataTypeFromT<T>(); }
};


template <typename T>
class mbServerRunActionIncrement : public mbServerRunActionT<T>
{
public:
    mbServerRunActionIncrement(const MBSETTINGS &settings) : mbServerRunActionT<T>(settings)
    {
        m_increment = settings.value(mbServerAction::Strings::instance().incrementValue).value<T>();
        m_min = settings.value(mbServerAction::Strings::instance().incrementMin).value<T>();
        m_max = settings.value(mbServerAction::Strings::instance().incrementMax).value<T>();
    }

public:
    int exec(qint64 time) override
    {
        if (time-this->m_last >= this->m_period)
        {
            QVariant v = this->value();
            T t = v.value<T>();
            mbServerRunAction::trySwap(&t, sizeof(t));
            t += m_increment;
            if ((t < m_min) || (t > m_max))
                t = m_min;
            mbServerRunAction::trySwap(&t, sizeof(t));
            this->setValue(t);
            this->m_last = time;
        }
        return 0;
    }

private:
    T m_increment;
    T m_min;
    T m_max;
};

template <typename T>
class mbServerRunActionSine : public mbServerRunActionT<T>
{
public:
    mbServerRunActionSine(const MBSETTINGS &settings) : mbServerRunActionT<T>(settings)
    {
        const mbServerAction::Strings &s = mbServerAction::Strings::instance();
        m_sinePeriod = settings.value(s.sinePeriod).toDouble();
        if (m_sinePeriod <= 0.0)
            m_sinePeriod = 1.0;
        m_phaseShift    = settings.value(s.sinePhaseShift   ).toLongLong();
        m_amplitude     = settings.value(s.sineAmplitude    ).toDouble();
        m_verticalShift = settings.value(s.sineVerticalShift).toDouble();
    }

public:
    int exec(qint64 time) override
    {
        if (((time-this->m_last) >= this->m_period))
        {
            qreal x = static_cast<qreal>(time-m_phaseShift)/m_sinePeriod;
            T v = static_cast<T>(m_amplitude*qSin(x*2*M_PI)+m_verticalShift);
            mbServerRunAction::trySwap(&v, sizeof(v));
            this->setValue(v);
            this->m_last = time;
        }
        return 0;
    }

private:
    qreal m_sinePeriod;
    qint64 m_phaseShift;
    qreal m_amplitude;
    qreal m_verticalShift;
};

template <typename T>
class mbServerRunActionRandom : public mbServerRunActionT<T>
{
public:
    mbServerRunActionRandom(const MBSETTINGS &settings) : mbServerRunActionT<T>(settings)
    {
        const mbServerAction::Strings &s = mbServerAction::Strings::instance();
        m_min   = settings.value(s.randomMin).toDouble();
        m_range = settings.value(s.randomMax).toDouble()-m_min;
    }

public:
    int exec(qint64 time) override
    {
        if (((time-this->m_last) >= this->m_period))
        {
            qreal x = static_cast<qreal>(RAND_MAX-qrand())/static_cast<qreal>(RAND_MAX); // koef is [0;1]
            T v = static_cast<T>(x*m_range+m_min);
            mbServerRunAction::trySwap(&v, sizeof(v));
            this->setValue(v);
            this->m_last = time;
        }
        return 0;
    }

private:
    qreal m_min;
    qreal m_range;
};

class mbServerRunActionCopy : public mbServerRunAction
{
public:
    mbServerRunActionCopy(const MBSETTINGS &settings);
    mb::DataType dataType() const override { return m_dataType; }

public:
    int exec(qint64 time) override;

private:
    typedef Modbus::StatusCode (mbServerDevice::*MethodRead )(uint bitOffset, uint bitCount, void* bites, uint *fact) const;
    typedef Modbus::StatusCode (mbServerDevice::*MethodWrite)(uint bitOffset, uint bitCount, const void* bites, uint *fact);
    MethodRead  m_methodRead;
    MethodWrite m_methodWrite;
    mb::DataType m_dataType;
    uint m_dst;
    uint m_src;
    uint m_dstCount;
    uint m_srcCount;
    QByteArray m_buffer;
};

mbServerRunAction *createRunActionIncrement(mb::DataType dataType, const MBSETTINGS &settings);
mbServerRunAction *createRunActionSine     (mb::DataType dataType, const MBSETTINGS &settings);
mbServerRunAction *createRunActionRandom   (mb::DataType dataType, const MBSETTINGS &settings);
mbServerRunAction *createRunActionCopy     (const MBSETTINGS &settings);

#endif // SERVER_RUNACTION_H
