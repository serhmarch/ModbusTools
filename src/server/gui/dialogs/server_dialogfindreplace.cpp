#include "server_dialogfindreplace.h"
#include "ui_server_dialogfindreplace.h"

#include <gui/server_ui.h>
#include <gui/server_windowmanager.h>

mbServerDialogFindReplace::mbServerDialogFindReplace(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbServerDialogFindReplace)
{
    ui->setupUi(this);

    connect(ui->btnFindNext    , &QPushButton::clicked, this, &mbServerDialogFindReplace::findNext    );
    connect(ui->btnFindPrevious, &QPushButton::clicked, this, &mbServerDialogFindReplace::findPrevious);
    connect(ui->btnClose       , &QPushButton::clicked, this, &mbServerDialogFindReplace::close       );
}

mbServerDialogFindReplace::~mbServerDialogFindReplace()
{
    delete ui;
}

void mbServerDialogFindReplace::execFindReplace(bool replace)
{
    if (replace)
        this->setWindowTitle("Replace");
    else
        this->setWindowTitle("Find");
    this->show();
    QString text = mbServer::global()->ui()->windowManager()->selectedText();
    if (text.size())
        ui->cmbFind->setCurrentText(text);
}

void mbServerDialogFindReplace::findNext()
{
    int flags = getFindFlags();
    mbServer::global()->ui()->windowManager()->find(ui->cmbFind->currentText(), flags);
}

void mbServerDialogFindReplace::findPrevious()
{
    int flags = getFindFlags();
    flags |= mb::FindBackward;
    mbServer::global()->ui()->windowManager()->find(ui->cmbFind->currentText(), flags);
}

void mbServerDialogFindReplace::replace()
{

}

int mbServerDialogFindReplace::getFindFlags()
{
    int flags = 0;
    if (ui->chbMatchCase->isChecked()) flags |= mb::FindCaseSensitively;
    if (ui->chbMatchWord->isChecked()) flags |= mb::FindWholeWords     ;
    return flags;
}
