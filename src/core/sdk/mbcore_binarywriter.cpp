#include "mbcore_binarywriter.h"

mbCoreBinaryWriter::mbCoreBinaryWriter()
{

}

void mbCoreBinaryWriter::write(int v)
{
    m_data.append(reinterpret_cast<const char*>(&v), sizeof(v));
}

void mbCoreBinaryWriter::write(const QRect &v)
{
    write(v.x());
    write(v.y());
    write(v.width());
    write(v.height());
}

void mbCoreBinaryWriter::write(const QByteArray &v)
{
    write(v.size());
    m_data.append(v);
}

void mbCoreBinaryWriter::write(const QString &v)
{
    write(v.toUtf8());
}
