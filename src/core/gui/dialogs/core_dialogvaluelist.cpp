#include "core_dialogvaluelist.h"
#include "ui_core_dialogvaluelist.h"

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
