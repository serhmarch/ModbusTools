#include "core_dialogreplace.h"

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>

mbCoreDialogReplace::mbCoreDialogReplace(QWidget *parent)
{
    setWindowTitle("Replace Dialog");

    auto *mainLayout = new QVBoxLayout(this);
    m_label = new QLabel("What would you like to do?", this);
    mainLayout->addWidget(m_label);

    QGridLayout *grid = new QGridLayout;
    mainLayout->addLayout(grid);

    m_buttonReplace = new QPushButton(tr("Replace"), this);
    connect(m_buttonReplace, &QPushButton::clicked, this, &mbCoreDialogReplace::slotReplaceClicked);
    grid->addWidget(m_buttonReplace, 0, 0);

    m_buttonRename = new QPushButton(tr("Rename"), this);
    connect(m_buttonRename, &QPushButton::clicked, this, &mbCoreDialogReplace::slotRenameClicked);
    grid->addWidget(m_buttonRename, 0, 1);

    m_buttonSkip = new QPushButton(tr("Skip"), this);
    connect(m_buttonSkip, &QPushButton::clicked, this, &mbCoreDialogReplace::slotSkipClicked);
    grid->addWidget(m_buttonSkip, 0, 2);

    m_buttonReplaceAll = new QPushButton(tr("Replace All"), this);
    connect(m_buttonReplaceAll, &QPushButton::clicked, this, &mbCoreDialogReplace::slotReplaceAllClicked);
    grid->addWidget(m_buttonReplaceAll, 1, 0);

    m_buttonRenameAll = new QPushButton(tr("Rename All"), this);
    connect(m_buttonRenameAll, &QPushButton::clicked, this, &mbCoreDialogReplace::slotRenameAllClicked);
    grid->addWidget(m_buttonRenameAll, 1, 1);

    m_buttonSkipAll = new QPushButton(tr("Skip All"), this);
    connect(m_buttonSkipAll, &QPushButton::clicked, this, &mbCoreDialogReplace::slotSkipAllClicked);
    grid->addWidget(m_buttonSkipAll, 1, 2);

    m_buttonCancel = new QPushButton(tr("Cancel"), this);
    connect(m_buttonCancel, &QPushButton::clicked, this, &mbCoreDialogReplace::slotCancelClicked);
    grid->addWidget(m_buttonCancel, 2, 2);
}

QString mbCoreDialogReplace::text() const
{
    return m_label->text();
}

void mbCoreDialogReplace::setText(const QString &text)
{
    m_label->setText(text);
}

void mbCoreDialogReplace::setUseAllButtons(bool use)
{
    m_buttonReplaceAll->setVisible(use);
    m_buttonRenameAll->setVisible(use);
    m_buttonSkipAll->setVisible(use);
}

void mbCoreDialogReplace::slotReplaceClicked()
{
    QDialog::done(Replace);
}

void mbCoreDialogReplace::slotRenameClicked()
{
    QDialog::done(Rename);
}

void mbCoreDialogReplace::slotSkipClicked()
{
    QDialog::done(Skip);
}

void mbCoreDialogReplace::slotReplaceAllClicked()
{
    QDialog::done(ReplaceAll);
}

void mbCoreDialogReplace::slotRenameAllClicked()
{
    QDialog::done(RenameAll);
}

void mbCoreDialogReplace::slotSkipAllClicked()
{
    QDialog::done(SkipAll);
}

void mbCoreDialogReplace::slotCancelClicked()
{
    QDialog::reject();
}
