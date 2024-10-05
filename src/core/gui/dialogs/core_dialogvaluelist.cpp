#include "core_dialogvaluelist.h"
#include "ui_core_dialogvaluelist.h"

#include <QListWidgetItem>

mbCoreDialogValueList::Strings::Strings() :
    title(QStringLiteral("Edit values")),
    cachePrefix(QStringLiteral("Ui.Dialogs.ValueList."))
{
}

const mbCoreDialogValueList::Strings &mbCoreDialogValueList::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogValueList::mbCoreDialogValueList(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbCoreDialogValueList)
{
    ui->setupUi(this);

    connect(ui->btnAdd   , &QPushButton::clicked, this, &mbCoreDialogValueList::slotAdd     );
    connect(ui->btnRemove, &QPushButton::clicked, this, &mbCoreDialogValueList::slotRemove  );
    connect(ui->btnUp    , &QPushButton::clicked, this, &mbCoreDialogValueList::slotMoveUp  );
    connect(ui->btnDown  , &QPushButton::clicked, this, &mbCoreDialogValueList::slotMoveDown);

    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &mbCoreDialogValueList::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &mbCoreDialogValueList::reject);
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
        if (!all.contains(v))
            continue;
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
    QList<QListWidgetItem*> ls = ui->lsAvailable->selectedItems();
    Q_FOREACH(QListWidgetItem* w, ls)
        ui->lsAvailable->takeItem(ui->lsAvailable->row(w));

    Q_FOREACH(QListWidgetItem* w, ls)
        ui->lsCurrent->addItem(w);

}

void mbCoreDialogValueList::slotRemove()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    Q_FOREACH(QListWidgetItem* w, ls)
        ui->lsCurrent->takeItem(ui->lsCurrent->row(w));

    Q_FOREACH(QListWidgetItem* w, ls)
        ui->lsAvailable->addItem(w);
}

void mbCoreDialogValueList::slotMoveUp()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    if (ls.count())
    {
        QListWidgetItem *w = ls.first();
        int i = ui->lsCurrent->row(w);
        if (i > 0)
        {
            ui->lsCurrent->takeItem(i);
            i--;
            ui->lsCurrent->insertItem(i, w);
            ui->lsCurrent->setCurrentItem(w);
        }
    }
}

void mbCoreDialogValueList::slotMoveDown()
{
    QList<QListWidgetItem*> ls = ui->lsCurrent->selectedItems();
    if (ls.count())
    {
        QListWidgetItem *w = ls.first();
        int i = ui->lsCurrent->row(w);
        if (i < (ui->lsCurrent->count()-1))
        {
            ui->lsCurrent->takeItem(i);
            i++;
            ui->lsCurrent->insertItem(i, w);
            ui->lsCurrent->setCurrentItem(w);
        }
    }
}
