#ifndef SERVER_SCRIPTHIGHLIGHTER_H
#define SERVER_SCRIPTHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class mbServerScriptHighlighter : public QSyntaxHighlighter
{
public:
    mbServerScriptHighlighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    inline bool isStateMultiLine() const { return previousBlockState() != -1; }
    inline void setStateMultiLine() { setCurrentBlockState(0); }
    inline void setStateNormal() { setCurrentBlockState(-1); }

private:
    void parseWord(const QString &text);
    void parseNumber(const QString &text);
    bool parseString(const QString &text);
    bool moveToStringEnd(const QString &text);
    inline int getTokenLength() const { return m_pos - m_begin; }
    inline QString getToken(const QString &text) const { return text.mid(m_begin, getTokenLength()); }

private:
    QTextCharFormat m_keywordFormat;
    QTextCharFormat m_numberFormat;
    QTextCharFormat m_commentFormat;
    QTextCharFormat m_stringFormat;
    QTextCharFormat m_symbolFormat;

private:
    int m_pos;
    int m_len;
    int m_begin;
    QString m_quotes;

};

#endif // SERVER_SCRIPTHIGHLIGHTER_H
