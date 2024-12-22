#include "server_scripthighlighter.h"

static QSet<QString> keywordsSet =
{
    "False"     ,
    "None"      ,
    "True"      ,
    "and"       ,
    "as"        ,
    "assert"    ,
    "asynch"    ,
    "await"     ,
    "break"     ,
    "class"     ,
    "continue"  ,
    "def"       ,
    "del"       ,
    "elif"      ,
    "else"      ,
    "except"    ,
    "finally"   ,
    "for"       ,
    "from"      ,
    "global"    ,
    "if"        ,
    "import"    ,
    "in"        ,
    "is"        ,
    "lambda"    ,
    "nonlocal"  ,
    "not"       ,
    "or"        ,
    "pass"      ,
    "raise"     ,
    "return"    ,
    "try"       ,
    "while"     ,
    "with"      ,
    "yield"
};

static const QString SingleQuotes("'''");
static const QString DoubleQuotes("\"\"\"");
static const int SizeQuotes = SingleQuotes.length();

mbServerScriptHighlighter::mbServerScriptHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    m_keywordFormat.setForeground(Qt::blue);

    m_numberFormat.setForeground(Qt::red);

    m_commentFormat.setForeground(Qt::gray);

    m_stringFormat.setForeground(Qt::darkGreen);

    m_symbolFormat.setForeground(Qt::magenta);
}

void mbServerScriptHighlighter::highlightBlock(const QString &text)
{
    m_pos = 0;
    m_len = text.length();
    if (isStateMultiLine())
    {
        m_begin = m_pos;
        bool finished = moveToStringEnd(text);
        setFormat(m_begin, getTokenLength(), m_stringFormat);
        if (!finished)
        {
            setStateMultiLine();
            return;
        }
    }
    setStateNormal();
    while (m_pos < m_len)
    {
        m_begin = m_pos;
        QChar sym = text.at(m_pos);
        if (sym.isLetter() || sym == '_')
        {
            parseWord(text);
            QString word = getToken(text);
            if (keywordsSet.contains(word))
                setFormat(m_begin, getTokenLength(), m_keywordFormat);
        }
        else if (sym.isDigit())
        {
            parseNumber(text);
            setFormat(m_begin, getTokenLength(), m_numberFormat);
        }
        else if (sym == '"' || sym == '\'')
        {
            bool finished = parseString(text);
            setFormat(m_begin, getTokenLength(), m_stringFormat);
            if (!finished)
            {
                setStateMultiLine();
                return;
            }
        }
        else if (sym == '#')
        {
            m_pos = m_len;
            setFormat(m_begin, getTokenLength(), m_commentFormat);
            return;
        }
        else
        {
            m_pos++;
            if (sym.isPunct() || sym.category() == QChar::Symbol_Math)
                setFormat(m_begin, 1, m_symbolFormat);
        }
    }
}

void mbServerScriptHighlighter::parseWord(const QString &text)
{
    QChar c;
    while ((++m_pos < m_len) && ((c = text.at(m_pos)).isLetterOrNumber() || c == '_'))
        ;
}

void mbServerScriptHighlighter::parseNumber(const QString &text)
{
    //QString prefix = text.mid(m_pos, 2);
    //if (prefix == "0b" || prefix == "0o" || prefix == "0x")
    //    m_pos += 2;
    while ((++m_pos < m_len) && text.at(m_pos).isLetterOrNumber())
        ;
}

bool mbServerScriptHighlighter::parseString(const QString &text)
{
    QString s = text.mid(m_pos, SizeQuotes);
    if (s == SingleQuotes || s == DoubleQuotes)
    {
        m_pos += SizeQuotes;
        m_quotes = s;
        return moveToStringEnd(text);
    }
    QChar quote = text.at(m_pos);
    QChar c;
    while ((++m_pos < m_len) && (c = text.at(m_pos)) != quote)
    {
        if (c == '\\')
            ++m_pos;
    }
    if (m_pos < m_len)
        ++m_pos;
    return true;
}

bool mbServerScriptHighlighter::moveToStringEnd(const QString &text)
{
    while ((++m_pos < m_len) && text.midRef(m_pos, SizeQuotes) != m_quotes)
    {
        if (text.at(m_pos) == '\\')
            ++m_pos;
    }
    if (m_pos < m_len)
    {
        m_pos += SizeQuotes;
        return true;
    }
    return false;
}
