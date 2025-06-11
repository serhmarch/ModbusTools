#ifndef SERVER_DIALOGFINDREPLACE_H
#define SERVER_DIALOGFINDREPLACE_H

#include <gui/dialogs/core_dialogedit.h>

namespace Ui {
class mbServerDialogFindReplace;
}

class mbServerWindowManager;

class mbServerDialogFindReplace : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct Strings : public mbCoreDialogBase::Strings
    {
        const QString cachePrefix;
        const QString findComboBox;

        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogFindReplace(QWidget *parent = nullptr);
    ~mbServerDialogFindReplace();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    void execFindReplace(bool replace = false);

public Q_SLOTS:
    void findNext();
    void findPrevious();
    void replace();

private:
    int getFindFlags();
    void processFindCombo();

private:
    Ui::mbServerDialogFindReplace *ui;
};

#endif // SERVER_DIALOGFINDREPLACE_H
