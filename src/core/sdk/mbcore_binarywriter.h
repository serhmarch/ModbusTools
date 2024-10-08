#ifndef MBCOREBINARYWRITER_H
#define MBCOREBINARYWRITER_H

#include <QRect>
#include <QByteArray>
#include <QString>

class mbCoreBinaryWriter
{
public:
    mbCoreBinaryWriter();

public:
    inline QByteArray data() const { return m_data; }

public:
    void write(int v);
    void write(const QRect &v);
    void write(const QByteArray &v);
    void write(const QString &v);

private:
    QByteArray m_data;
};

#endif // MBCOREBINARYWRITER_H
