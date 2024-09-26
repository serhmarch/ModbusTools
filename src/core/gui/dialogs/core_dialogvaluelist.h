#ifndef CORE_DIALOGVALUELIST_H
#define CORE_DIALOGVALUELIST_H

#include "core_dialogbase.h"

namespace Ui {
class mbCoreDialogValueList;
}

class mbCoreDialogValueList : public mbCoreDialogBase
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
