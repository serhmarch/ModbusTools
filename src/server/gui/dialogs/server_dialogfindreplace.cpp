#include "server_dialogfindreplace.h"
#include "ui_server_dialogfindreplace.h"

#include <gui/server_ui.h>
#include <gui/script/server_scriptmanager.h>
#include <gui/script/server_basescripteditor.h>

#define MAX_FIND_COMBO_SZ 20

mbServerDialogFindReplace::Strings::Strings() :
    cachePrefix    (QStringLiteral("Ui.Dialogs.FindReplace.")),
    findComboBox   (QStringLiteral("findComboBox")),
    replaceComboBox(QStringLiteral("replaceComboBox"))
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
    connect(ui->btnReplace     , &QPushButton::clicked, this, &mbServerDialogFindReplace::replace     );
    connect(ui->btnReplaceAll  , &QPushButton::clicked, this, &mbServerDialogFindReplace::replaceAll  );
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

    QComboBox *cmb;
    QStringList lsFindComboBox;
    cmb = ui->cmbFind;
    for (int i = 0; i < cmb->count(); ++i)
        lsFindComboBox.append(cmb->itemText(i));

    QStringList lsReplaceComboBox;
    cmb = ui->cmbReplace;
    for (int i = 0; i < cmb->count(); ++i)
        lsReplaceComboBox.append(cmb->itemText(i));

    MBSETTINGS m = mbCoreDialogBase::cachedSettings();
    m[prefix+ds.findComboBox   ] = lsFindComboBox   ;
    m[prefix+ds.replaceComboBox] = lsReplaceComboBox;
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

    it = settings.find(prefix+ds.replaceComboBox);
    if (it != end)
    {
        QStringList ls = it.value().toStringList();
        int i = 0;
        Q_FOREACH(const QString &s, ls)
        {
            ui->cmbReplace->insertItem(i, s);
            ++i;
        }
    }
}

void mbServerDialogFindReplace::execFindReplace(bool replace)
{
    if (replace)
    {
        this->setWindowTitle("Replace");
    }
    else
    {
        this->setWindowTitle("Find");
    }
    ui->lblReplace   ->setVisible(replace);
    ui->cmbReplace   ->setVisible(replace);
    ui->btnReplace   ->setVisible(replace);
    ui->btnReplaceAll->setVisible(replace);
    this->show();
    mbServerBaseScriptEditor *se = mbServer::global()->ui()->scriptManager()->activeScriptEditor();
    if (se)
    {
        QString text = se->textCursor().selectedText();
        if (text.size())
            ui->cmbFind->setCurrentText(text);
    }
}

void mbServerDialogFindReplace::findNext()
{
    processCombo(ui->cmbFind);
    mbServerBaseScriptEditor *se = mbServer::global()->ui()->scriptManager()->activeScriptEditor();
    if (se)
    {
        int flags = getFindFlags();
        QString sfind = ui->cmbFind->currentText();
        if (flags & mb::FindEscapeSequence)
            sfind = mb::resolveEscapeSequnces(sfind);
        se->findText(sfind, flags);
    }
}

void mbServerDialogFindReplace::findPrevious()
{
    processCombo(ui->cmbFind);
    mbServerBaseScriptEditor *se = mbServer::global()->ui()->scriptManager()->activeScriptEditor();
    if (se)
    {
        int flags = getFindFlags();
        QString sfind = ui->cmbFind->currentText();
        if (flags & mb::FindEscapeSequence)
            sfind = mb::resolveEscapeSequnces(sfind);
        flags |= mb::FindBackward;
        se->findText(sfind, flags);
    }
}

void mbServerDialogFindReplace::replace()
{
    processCombo(ui->cmbFind);
    processCombo(ui->cmbReplace);
    mbServerBaseScriptEditor *se = mbServer::global()->ui()->scriptManager()->activeScriptEditor();
    if (se)
    {
        int flags = getFindFlags();
        QString sfind = ui->cmbFind->currentText();
        QString sreplace = ui->cmbReplace->currentText();
        if (flags & mb::FindEscapeSequence)
        {
            sfind = mb::resolveEscapeSequnces(sfind);
            sreplace = mb::resolveEscapeSequnces(sreplace);
        }
        se->replaceText(sfind,
                        sreplace,
                        flags);
    }
}

void mbServerDialogFindReplace::replaceAll()
{
    processCombo(ui->cmbFind);
    processCombo(ui->cmbReplace);
    mbServerBaseScriptEditor *se = mbServer::global()->ui()->scriptManager()->activeScriptEditor();
    if (se)
    {
        int flags = getFindFlags();
        QString sfind = ui->cmbFind->currentText();
        QString sreplace = ui->cmbReplace->currentText();
        if (flags & mb::FindEscapeSequence)
        {
            sfind = mb::resolveEscapeSequnces(sfind);
            sreplace = mb::resolveEscapeSequnces(sreplace);
        }
        se->replaceTextAll(sfind,
                           sreplace,
                           flags);
    }
}

int mbServerDialogFindReplace::getFindFlags()
{
    int flags = 0;
    if (ui->chbMatchCase     ->isChecked()) flags |= mb::FindCaseSensitively;
    if (ui->chbMatchWord     ->isChecked()) flags |= mb::FindWholeWords     ;
    if (ui->chbEscapeSequence->isChecked()) flags |= mb::FindEscapeSequence ;
    return flags;
}

void mbServerDialogFindReplace::processCombo(QComboBox *cmb)
{
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
