#include "server_scriptmodule.h"

mbServerScriptModule::Strings::Strings() :
    name      (QStringLiteral("name")),
    author    (QStringLiteral("author")),
    comment   (QStringLiteral("comment")),
    sourceCode(QStringLiteral("sourceCode"))
{
}

const mbServerScriptModule::Strings &mbServerScriptModule::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerScriptModule::Defaults::Defaults() :
    name(QStringLiteral("module"))
{
}

const mbServerScriptModule::Defaults &mbServerScriptModule::Defaults::instance()
{
    static const Defaults d;
    return d;
}


mbServerScriptModule::mbServerScriptModule(QObject *parent)
    : QObject{parent}
{

}

QString mbServerScriptModule::getSourceCode()
{
    Q_EMIT beginToGetSourceCode();
    return sourceCode();
}

MBSETTINGS mbServerScriptModule::settings() const
{
    const auto &s = Strings::instance();
    MBSETTINGS res;
    res[s.name      ] = name      ();
    res[s.author    ] = author    ();
    res[s.comment   ] = comment   ();
    res[s.sourceCode] = sourceCode();
    return res;
}

void mbServerScriptModule::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();

    it = settings.find(s.name);
    if (it != end)
    {
        QString v = it.value().toString();
        setName(v);
    }

    it = settings.find(s.author);
    if (it != end)
    {
        QString v = it.value().toString();
        setAuthor(v);
    }

    it = settings.find(s.comment);
    if (it != end)
    {
        QString v = it.value().toString();
        setComment(v);
    }

    it = settings.find(s.sourceCode);
    if (it != end)
    {
        QString v = it.value().toString();
        setSourceCode(v);
    }
}
