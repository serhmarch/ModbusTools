#ifndef SERVER_DIALOGFINDREPLACE_H
#define SERVER_DIALOGFINDREPLACE_H

#include <gui/dialogs/core_dialogedit.h>

class QComboBox;

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
        const QString replaceComboBox;

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
    void replaceAll();

private:
    int getFindFlags();
    void processCombo(QComboBox *cmb);

private:
    Ui::mbServerDialogFindReplace *ui;
};

#endif // SERVER_DIALOGFINDREPLACE_H
