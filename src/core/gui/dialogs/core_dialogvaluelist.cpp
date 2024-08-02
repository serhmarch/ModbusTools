#include "core_dialogvaluelist.h"
#include "ui_core_dialogvaluelist.h"

#include <QListWidgetItem>

mbCoreDialogValueList::Strings::Strings() :
    title(QStringLiteral("Edit values"))
{
}

const mbCoreDialogValueList::Strings &mbCoreDialogValueList::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogValueList::mbCoreDialogValueList(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbCoreDialogValueList)
{
    ui->setupUi(this);
}

mbCoreDialogValueList::~mbCoreDialogValueList()
{
    delete ui;
}

bool mbCoreDialogValueList::getValueList(const QVariantList &all, QVariantList &current, const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(all, current);
    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(current);
        return true;
    }
    return false;
}

void mbCoreDialogValueList::fillForm(const QVariantList &all, const QVariantList &current)
{
    ui->lsAvailable->clear();
    ui->lsCurrent->clear();
    QVariantList available = all;
    Q_FOREACH (const QVariant &v, current)
    {
        QString s = v.toString();
        ui->lsCurrent->addItem(s);
        int ai = available.indexOf(v);
        if (ai >= 0)
            available.removeAt(ai);
    }
    Q_FOREACH (const QVariant &v, available)
    {
        QString s = v.toString();
        ui->lsAvailable->addItem(s);
    }
}

void mbCoreDialogValueList::fillData(QVariantList &current)
{
    current.clear();
    for (int i = 0; i < ui->lsCurrent->count(); i++)
    {
        QListWidgetItem *item = ui->lsCurrent->item(i);
        current.append(item->data(Qt::DisplayRole));
    }
}

void mbCoreDialogValueList::slotAdd()
{
    
}

void mbCoreDialogValueList::slotRemove()
{
    
}

void mbCoreDialogValueList::slotMoveUp()
{
    
}

void mbCoreDialogValueList::slotMoveDown()
{
    
}
