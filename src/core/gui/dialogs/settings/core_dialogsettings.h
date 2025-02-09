#ifndef CORE_DIALOGSETTINGS_H
#define CORE_DIALOGSETTINGS_H

#include "../core_dialogbase.h"

class QSplitter;
class QListWidget;
class QStackedWidget;
class mbCoreWidgetSettingsView;
class mbCoreWidgetSettingsDataView;
class mbCoreWidgetSettingsLog;

class MB_EXPORT mbCoreDialogSettings : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings : public mbCoreDialogBase::Strings
    {
        const QString title;
        const QString cachePrefix;
        const QString wSplitterState;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogSettings(QWidget *parent = nullptr);

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

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
    mbCoreWidgetSettingsDataView *m_dataView;
    mbCoreWidgetSettingsLog  *m_log ;
};

#endif // CORE_DIALOGSETTINGS_H
