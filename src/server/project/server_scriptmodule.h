#ifndef SERVER_SCRIPTMODULE_H
#define SERVER_SCRIPTMODULE_H

#include <QObject>

#include <server_global.h>

class mbServerScriptModule : public QObject
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString name      ;
        const QString comment   ;
        const QString sourceCode;

        Strings();
        static const Strings &instance();
    };

public:
    struct Defaults
    {
        const QString name;

        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbServerScriptModule(QObject *parent = nullptr);

public: // project
    inline QString name() const { return objectName(); }
    inline void setName(const QString& name) { setObjectName(name); Q_EMIT nameChanged(name); Q_EMIT changed(); }
    inline QString fileName() const { return name()+".py"; }
    inline QString comment() const { return m_comment; }
    inline void setComment(const QString& comment) { m_comment = comment; Q_EMIT changed(); }
    inline QString sourceCode() const { return m_code; }
    inline void setSourceCode(const QString& code) { m_code = code; }
    QString getSourceCode();

    MBSETTINGS settings() const;
    void setSettings(const MBSETTINGS &settings);

Q_SIGNALS:
    void nameChanged(const QString& newName);
    void changed();
    void beginToGetSourceCode();

protected:
    QString m_comment;
    QString m_code;
};

#endif // MBSERVERSCRIPTMODULE_H
