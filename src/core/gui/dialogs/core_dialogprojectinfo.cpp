#include "core_dialogprojectinfo.h"
#include "ui_core_dialogprojectinfo.h"

#include <project/core_project.h>

mbCoreDialogProjectInfo::Strings::Strings() : mbCoreDialogBase::Strings(),
    cachePrefix(QStringLiteral("Ui.Dialogs.ProjectInfo."))
{
}

const mbCoreDialogProjectInfo::Strings &mbCoreDialogProjectInfo::Strings::instance()
{
    static const mbCoreDialogProjectInfo::Strings s;
    return s;
}

mbCoreDialogProjectInfo::mbCoreDialogProjectInfo(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbCoreDialogProjectInfo)
{
    ui->setupUi(this);

    // ----------------------------------------------------------------------------------
    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbCoreDialogProjectInfo::~mbCoreDialogProjectInfo()
{
    delete ui;
}

void mbCoreDialogProjectInfo::setProjectType(const QString &type)
{
    ui->lbType->setText(type);
}

void mbCoreDialogProjectInfo::showProjectInfo(mbCoreProject *project)
{
    fillProjectInfo(project);
    QDialog::exec();
}

void mbCoreDialogProjectInfo::fillProjectInfo(mbCoreProject *project)
{
    ui->lbName      ->setText(project->name());
    ui->lbAuthor    ->setText(project->author());
    ui->lbPath      ->setText(project->absoluteFilePath());
    ui->lbSize      ->setText(QString::number(project->fileSize())+QStringLiteral(" Bytes") );
    ui->lbCreated   ->setText(project->fileCreated().toString(Qt::ISODate));
    ui->lbModified  ->setText(project->fileModified().toString(Qt::ISODate));
    ui->lbVersion   ->setText(project->versionStr());
    ui->lbEditNumber->setText(QString::number(project->editNumber()));

    ui->txtComment->setPlainText(project->comment());
}
