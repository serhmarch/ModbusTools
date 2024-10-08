#ifndef MBCORE_BINARYREADER_H
#define MBCORE_BINARYREADER_H

#include <QRect>
#include <QByteArray>
#include <QString>

class mbCoreBinaryReader
{
public:
    mbCoreBinaryReader();
    mbCoreBinaryReader(const QByteArray &v);

public:
    inline QByteArray data() const { return m_data; }
    void setData(const QByteArray &v);
    inline bool isProcessing() const { return m_i < m_data.size(); }
    inline bool isEnd() const { return m_i >= m_data.size(); }

public:
    bool read(int &v);
    bool read(QRect &v);
    bool read(QByteArray &v);
    bool read(QString &v);

private:
    QByteArray m_data;
    int m_i;
};

#endif // MBCORE_BINARYREADER_H
