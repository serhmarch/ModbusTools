#ifndef MBCOREDIALOGSETTINGS_H
#define MBCOREDIALOGSETTINGS_H

#include "../core_dialogbase.h"

class QSplitter;
class QListWidget;
class QStackedWidget;
class mbCoreWidgetSettingsView;
class mbCoreWidgetSettingsLog;

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
    virtual void fillForm(const MBSETTINGS &m);
    virtual void fillData(MBSETTINGS &m);

protected:
    QSplitter      *m_splitter     ;
    QListWidget    *m_listWidget   ;
    QStackedWidget *m_stackedWidget;

    mbCoreWidgetSettingsView *m_view;
    mbCoreWidgetSettingsLog  *m_log ;
};

#endif // MBCOREDIALOGSETTINGS_H
