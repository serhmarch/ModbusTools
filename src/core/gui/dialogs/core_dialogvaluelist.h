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
    struct Strings
    {
        const QString title;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreDialogValueList(QWidget *parent = nullptr);
    ~mbCoreDialogValueList();

public:
    bool getValueList(const QVariantList &all, QVariantList &current, const QString &title = QString());

private:
    void fillForm(const QVariantList &all, const QVariantList &current);
    void fillData(QVariantList &current);

private Q_SLOTS:
    void slotAdd();
    void slotRemove();
    void slotMoveUp();
    void slotMoveDown();

private:
    Ui::mbCoreDialogValueList *ui;
};

#endif // CORE_DIALOGVALUELIST_H
