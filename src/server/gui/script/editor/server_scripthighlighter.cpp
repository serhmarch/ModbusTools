#include "server_scripthighlighter.h"

mbServerScriptHighlighter::Strings::Strings() :
    KeywordsSet
    ({
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
    }),

    Single3Quotes       (QStringLiteral("'''"    )),
    Double3Quotes       (QStringLiteral("\"\"\"" )),
    TextFormat          (QStringLiteral("Text"   )),
    KeywordFormat       (QStringLiteral("Keyword")),
    NumberFormat        (QStringLiteral("Number" )),
    CommentFormat       (QStringLiteral("Comment")),
    StringFormat        (QStringLiteral("String" )),
    SymbolFormat        (QStringLiteral("Symbol" )),
    SeparatorColors     (';'),
    SeparatorColorParams('=')
{

}

const mbServerScriptHighlighter::Strings &mbServerScriptHighlighter::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerScriptHighlighter::Defaults::Defaults() :
    colorFormats({
        {TextFormat   , Qt::black    },
        {KeywordFormat, Qt::blue     },
        {NumberFormat , Qt::red      },
        {CommentFormat, Qt::gray     },
        {StringFormat , Qt::darkGreen},
        {SymbolFormat , Qt::magenta  }
    })
{
}

const mbServerScriptHighlighter::Defaults &mbServerScriptHighlighter::Defaults::instance()
{
    static const Defaults d;
    return d;
}

static const int Size3Quotes = mbServerScriptHighlighter::Strings::instance().Single3Quotes.length();

mbServerScriptHighlighter::ColorFormatType mbServerScriptHighlighter::toColorFormatType(const QString &sformat, bool *ok)
{
    const Strings &s = Strings::instance();
    ColorFormatType f = KeywordFormat;
    bool okInner = true;
    if      (sformat == s.TextFormat   ) f = TextFormat   ;
    else if (sformat == s.KeywordFormat) f = KeywordFormat;
    else if (sformat == s.NumberFormat ) f = NumberFormat ;
    else if (sformat == s.CommentFormat) f = CommentFormat;
    else if (sformat == s.StringFormat ) f = StringFormat ;
    else if (sformat == s.SymbolFormat ) f = SymbolFormat ;
    else
        okInner = false;
    if (ok)
        *ok = okInner;
    return f;
}

QString mbServerScriptHighlighter::toString(ColorFormatType f)
{
    const Strings &s = Strings::instance();
    switch (f)
    {
    case TextFormat   : return s.TextFormat   ;
    case KeywordFormat: return s.KeywordFormat;
    case NumberFormat : return s.NumberFormat ;
    case CommentFormat: return s.CommentFormat;
    case StringFormat : return s.StringFormat ;
    case SymbolFormat : return s.SymbolFormat ;
    }

    return QString();
}

mbServerScriptHighlighter::ColorFormats mbServerScriptHighlighter::toColorFormats(const QString &sformats)
{
    const Strings &s = Strings::instance();
    ColorFormats res;
    QStringList ls = sformats.split(s.SeparatorColors);
    bool ok;
    Q_FOREACH (const QString &c, ls)
    {
        QStringList params = c.split(s.SeparatorColorParams);
        if (params.size() == 2)
        {
            ColorFormatType ft = toColorFormatType(params.at(0), &ok);
            if (ok)
            {
                QColor color;
                color.setNamedColor(params.at(1));
                res.insert(ft, color);
            }
        }
    }
    return res;
}

QString mbServerScriptHighlighter::toString(const ColorFormats &formats)
{
    const Strings &s = Strings::instance();
    QStringList ls;
    for (auto it = formats.begin(); it != formats.end(); ++it)
    {
        ls.append(toString(it.key()) + s.SeparatorColorParams + it.value().name());
    }
    return ls.join(s.SeparatorColors);
}

mbServerScriptHighlighter::mbServerScriptHighlighter(const ColorFormats &formats, QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    m_textFormat   .setForeground(formats.value(TextFormat   ));
    m_keywordFormat.setForeground(formats.value(KeywordFormat));
    m_numberFormat .setForeground(formats.value(NumberFormat ));
    m_commentFormat.setForeground(formats.value(CommentFormat));
    m_stringFormat .setForeground(formats.value(StringFormat ));
    m_symbolFormat .setForeground(formats.value(SymbolFormat ));
}

mbServerScriptHighlighter::ColorFormats mbServerScriptHighlighter::colorFormats() const
{
    ColorFormats res;
    res.insert(TextFormat   , m_textFormat   .foreground().color());
    res.insert(KeywordFormat, m_keywordFormat.foreground().color());
    res.insert(NumberFormat , m_numberFormat .foreground().color());
    res.insert(CommentFormat, m_commentFormat.foreground().color());
    res.insert(StringFormat , m_stringFormat .foreground().color());
    res.insert(SymbolFormat , m_symbolFormat .foreground().color());
    return res;
}

void mbServerScriptHighlighter::setColorFormats(const ColorFormats &f)
{
    ColorFormats::const_iterator it;
    ColorFormats::const_iterator end = f.end();

    it = f.find(TextFormat   ); if (it != end) m_textFormat   .setForeground(it.value());
    it = f.find(KeywordFormat); if (it != end) m_keywordFormat.setForeground(it.value());
    it = f.find(NumberFormat ); if (it != end) m_numberFormat .setForeground(it.value());
    it = f.find(CommentFormat); if (it != end) m_commentFormat.setForeground(it.value());
    it = f.find(StringFormat ); if (it != end) m_stringFormat .setForeground(it.value());
    it = f.find(SymbolFormat ); if (it != end) m_symbolFormat .setForeground(it.value());

}

void mbServerScriptHighlighter::highlightBlock(const QString &text)
{
    const Strings &s = Strings::instance();
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
            if (s.KeywordsSet.contains(word))
                setFormat(m_begin, getTokenLength(), m_keywordFormat);
            else
                setFormat(m_begin, getTokenLength(), m_textFormat);
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
            ++m_pos;
            if (sym.isPunct() || sym.category() == QChar::Symbol_Math)
                setFormat(m_begin, 1, m_symbolFormat);
            else
                setFormat(m_begin, 1, m_textFormat);
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
    const Strings &s = Strings::instance();
    QString stext = text.mid(m_pos, Size3Quotes);
    if (stext == s.Single3Quotes || stext == s.Double3Quotes)
    {
        m_pos += Size3Quotes;
        m_quotes = stext;
        return moveToStringEnd(text);
    }
    const QChar quote = text.at(m_pos);
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
    while ((m_pos < m_len) && text.midRef(m_pos, Size3Quotes) != m_quotes)
    {
        if (text.at(m_pos) == '\\')
            ++m_pos;
        ++m_pos;
    }
    if (m_pos < m_len)
    {
        m_pos += Size3Quotes;
        return true;
    }
    return false;
}
