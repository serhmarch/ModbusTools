#ifndef CORE_DIALOGVALUELIST_H
#define CORE_DIALOGVALUELIST_H

#include <QDialog>

namespace Ui {
class mbCoreDialogValueList;
}

class mbCoreDialogValueList : public QDialog
{
    Q_OBJECT

public:
    explicit mbCoreDialogValueList(QWidget *parent = nullptr);
    ~mbCoreDialogValueList();

private:
    Ui::mbCoreDialogValueList *ui;
};

#endif // CORE_DIALOGVALUELIST_H
