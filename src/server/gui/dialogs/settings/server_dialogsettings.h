#ifndef SERVER_DIALOGSETTINGS_H
#define SERVER_DIALOGSETTINGS_H

#include <gui/dialogs/settings/core_dialogsettings.h>

class mbServerWidgetSettingsScript;

class mbServerDialogSettings : public mbCoreDialogSettings
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogSettings::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    mbServerDialogSettings(QWidget *parent = nullptr);

protected:
    void fillForm(const MBSETTINGS &m) override;
    void fillData(MBSETTINGS &m) override;

protected:
    mbServerWidgetSettingsScript *m_script;
};

#endif // SERVER_DIALOGSETTINGS_H
