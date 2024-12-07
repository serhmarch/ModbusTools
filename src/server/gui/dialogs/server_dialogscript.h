#ifndef SERVER_DIALOGSCRIPT_H
#define SERVER_DIALOGSCRIPT_H

#include <mbcore.h>
#include <gui/dialogs/core_dialogsettings.h>

namespace Ui {
class mbServerDialogScript;
}

class mbServerDialogScript : public mbCoreDialogSettings
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogSettings::Strings
    {
        const QString title;
        const QString cachePrefix;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogScript(QWidget *parent = nullptr);
    ~mbServerDialogScript();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    MBSETTINGS getSettings(const MBSETTINGS &scripts, const QString &title = QString()) override;

private:
    void fillForm(const MBSETTINGS &script);
    void fillData(MBSETTINGS &script);

private:
    Ui::mbServerDialogScript *ui;
};

#endif // SERVER_DIALOGSCRIPT_H
