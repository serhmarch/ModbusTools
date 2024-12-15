#ifndef MBCOREDIALOGSETTINGS_H
#define MBCOREDIALOGSETTINGS_H

#include "../core_dialogbase.h"

class QListWidget;
class QStackedWidget;

class mbCoreDialogSettings : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings : public mbCoreDialogBase::Strings
    {
        const QString title;
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogSettings(QWidget *parent = nullptr);

public:
    bool editSettings(const QString& title = QString());

protected:
    QListWidget    *m_listWidget   ;
    QStackedWidget *m_stackedWidget;
};

#endif // MBCOREDIALOGSETTINGS_H
