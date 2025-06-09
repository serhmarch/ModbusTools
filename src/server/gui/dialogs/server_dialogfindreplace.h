#ifndef SERVER_DIALOGFINDREPLACE_H
#define SERVER_DIALOGFINDREPLACE_H

#include <QDialog>

namespace Ui {
class mbServerDialogFindReplace;
}

class mbServerWindowManager;

class mbServerDialogFindReplace : public QDialog
{
    Q_OBJECT

public:
    explicit mbServerDialogFindReplace(QWidget *parent = nullptr);
    ~mbServerDialogFindReplace();

public:
    void execFindReplace(bool replace = false);

public Q_SLOTS:
    void findNext();
    void findPrevious();
    void replace();

private:
    int getFindFlags();

private:
    Ui::mbServerDialogFindReplace *ui;
};

#endif // SERVER_DIALOGFINDREPLACE_H
