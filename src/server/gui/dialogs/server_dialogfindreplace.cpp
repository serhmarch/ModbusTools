#include "server_dialogfindreplace.h"
#include "ui_server_dialogfindreplace.h"

#include <gui/server_ui.h>
#include <gui/server_windowmanager.h>

#define MAX_FIND_COMBO_SZ 20

mbServerDialogFindReplace::Strings::Strings() :
    cachePrefix (QStringLiteral("Ui.Dialogs.FindReplace.")),
    findComboBox(QStringLiteral("findComboBox"))
{
}

const mbServerDialogFindReplace::Strings &mbServerDialogFindReplace::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerDialogFindReplace::mbServerDialogFindReplace(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent),
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

MBSETTINGS mbServerDialogFindReplace::cachedSettings() const
{
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    QStringList lsFindComboBox;
    for (int i = 0; i < ui->cmbFind->count(); ++i)
        lsFindComboBox.append(ui->cmbFind->itemText(i));
    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    m[prefix+ds.findComboBox] = lsFindComboBox;
    return m;
}

void mbServerDialogFindReplace::setCachedSettings(const MBSETTINGS &settings)
{
    mbCoreDialogBase::setCachedSettings(settings);
    const Strings &ds = Strings::instance();
    const QString &prefix = ds.cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    it = settings.find(prefix+ds.findComboBox);
    if (it != end)
    {
        QStringList ls = it.value().toStringList();
        int i = 0;
        Q_FOREACH(const QString &s, ls)
        {
            ui->cmbFind->insertItem(i, s);
            ++i;
        }
    }
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
    processFindCombo();
    int flags = getFindFlags();
    mbServer::global()->ui()->windowManager()->find(ui->cmbFind->currentText(), flags);
}

void mbServerDialogFindReplace::findPrevious()
{
    processFindCombo();
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

void mbServerDialogFindReplace::processFindCombo()
{
    QComboBox *cmb = ui->cmbFind;
    QString text = cmb->currentText();
    if (text.size())
    {
        if (text != cmb->itemText(0))
        {
            int i = cmb->findText(text);
            if (i > 0)
            {
                cmb->removeItem(i);
                cmb->insertItem(0, text);
                cmb->setCurrentText(text);
            }
            else if (i < 0)
                cmb->insertItem(0, text);

            while (cmb->count() > MAX_FIND_COMBO_SZ)
                cmb->removeItem(MAX_FIND_COMBO_SZ);
        }
    }
}
