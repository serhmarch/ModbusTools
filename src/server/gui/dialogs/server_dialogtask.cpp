#include "server_dialogtask.h"
#include "ui_server_taskdialog.h"

#include <server.h>
#include <task/core_taskfactoryinfo.h>

mbServerDialogTask::Strings::Strings() :
    title(QStringLiteral("Data"))
{
}

const mbServerDialogTask::Strings &mbServerDialogTask::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogTask::mbServerDialogTask(mbServer* core, QWidget *parent) : QDialog(parent),
    ui(new Ui::mbServerDialogTask)
{
    ui->setupUi(this);

    m_core = core;
}

mbServerDialogTask::~mbServerDialogTask()
{
    delete ui;
}

bool mbServerDialogTask::getData(mbServerDialogTask::Data &data, const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    // fill 'Type'-combobox
    QComboBox* cmb = ui->cmbType;
    cmb->clear();
    Q_FOREACH (const mbCoreTaskFactoryInfo* f, m_core->taskFactories())
        cmb->addItem(f->name());
    if (data.type.length()) // type is not empty string
    {
        ui->lnName->setText(data.name);
        cmb->setCurrentText(data.type);
    }
    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        data.name = ui->lnName->text();
        data.type = cmb->currentText();
        return true;
    default:
        return false;
    }
}
