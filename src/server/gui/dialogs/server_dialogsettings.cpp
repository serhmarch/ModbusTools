#include "server_dialogsettings.h"
#include "ui_server_dialogsettings.h"

#include <server.h>

mbServerDialogSettings::Strings::Strings() :
    title(QStringLiteral("Settings"))
{

}

const mbServerDialogSettings::Strings &mbServerDialogSettings::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogSettings::mbServerDialogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mbServerDialogSettings)
{
    ui->setupUi(this);

    connect(ui->buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(ui->buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

mbServerDialogSettings::~mbServerDialogSettings()
{
    delete ui;
}

bool mbServerDialogSettings::editSystemSettings(const QString &title)
{
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    mbServer *core = mbServer::global();
    ui->chbUseTimestamp->setChecked(core->useTimestamp());
    ui->lnFormat->setText(core->formatDateTime());
    int r = QDialog::exec();
    switch (r)
    {
    case QDialog::Accepted:
        m_core->setUseTimestamp(ui->chbUseTimestamp->isChecked());
        m_core->setFormatDateTime(ui->lnFormat->text());
        return true;
    }
    return false;
}
