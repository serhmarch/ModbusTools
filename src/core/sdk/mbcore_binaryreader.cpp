#include "mbcore_binaryreader.h"

mbCoreBinaryReader::mbCoreBinaryReader()
{
    m_i = 0;
}

mbCoreBinaryReader::mbCoreBinaryReader(const QByteArray &v)
{
    setData(v);
}

void mbCoreBinaryReader::setData(const QByteArray &v)
{
    m_data = v;
    m_i = 0;
}

bool mbCoreBinaryReader::read(int &v)
{
    if ((sizeof(v) + m_i) <= m_data.size())
    {
        v = *reinterpret_cast<int*>(m_data.data()+m_i);
        m_i += sizeof(v);
        return true;
    }
    return false;;
}

bool mbCoreBinaryReader::read(QRect &v)
{
    int x, y, w, h;
    if (read(x) && read(y) && read(w) && read(h))
    {
        v.setRect(x, y, w, h);
        return true;
    }
    return false;
}

bool mbCoreBinaryReader::read(QByteArray &v)
{
    int sz;
    if (read(sz) && ((sz + m_i) <= m_data.size()))
    {
        v = m_data.mid(m_i, sz);
        m_i += sz;
        return true;
    }
    return false;
}

bool mbCoreBinaryReader::read(QString &v)
{
    QByteArray s;
    if (read(s))
    {
        v = QString::fromUtf8(s);
        return true;
    }
    return false;
}
