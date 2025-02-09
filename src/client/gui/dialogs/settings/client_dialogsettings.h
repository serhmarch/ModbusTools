#ifndef CLIENT_DIALOGSETTINGS_H
#define CLIENT_DIALOGSETTINGS_H

#include <gui/dialogs/settings/core_dialogsettings.h>

class mbClientWidgetSettingsScript;

class mbClientDialogSettings : public mbCoreDialogSettings
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogSettings::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    mbClientDialogSettings(QWidget *parent = nullptr);

protected:
    void fillForm(const MBSETTINGS &m) override;
    void fillData(MBSETTINGS &m) override;
};

#endif // CLIENT_DIALOGSETTINGS_H
