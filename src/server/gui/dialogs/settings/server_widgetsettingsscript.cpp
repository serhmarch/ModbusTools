#include "server_widgetsettingsscript.h"
#include "ui_server_widgetsettingsscript.h"

#include <server.h>
#include <gui/server_ui.h>

#include "../server_dialogs.h"
#include "server_modelsettingsscript.h"

mbServerWidgetSettingsScript::mbServerWidgetSettingsScript(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbServerWidgetSettingsScript)
{
    ui->setupUi(this);

    m_model = new mbServerModelSettingsScript(this);
    m_model->setAutoDetected(mbServer::global()->scriptAutoDetectedExecutables());
    ui->viewInterpreters->setModel(m_model);

    connect(ui->btnPyAdd        , &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPyAdd        );
    connect(ui->btnPySet        , &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPySet        );
    connect(ui->btnPyRemove     , &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPyRemove     );
    connect(ui->btnPyMakeDefault, &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPyMakeDefault);
    connect(ui->btnPyClear      , &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPyClear      );
    connect(ui->btnPyBrowse     , &QPushButton::clicked, this, &mbServerWidgetSettingsScript::slotPyBrowse     );

    QItemSelectionModel *sm = ui->viewInterpreters->selectionModel();
    connect(sm, &QItemSelectionModel::selectionChanged, this, &mbServerWidgetSettingsScript::selectionChanged);
}

mbServerWidgetSettingsScript::~mbServerWidgetSettingsScript()
{
    delete ui;
}

bool mbServerWidgetSettingsScript::scriptEnable() const
{
    return ui->chbScriptEnable->isChecked();
}

void mbServerWidgetSettingsScript::setScriptEnable(bool enable)
{
    ui->chbScriptEnable->setChecked(enable);
}

bool mbServerWidgetSettingsScript::scriptGenerateComment() const
{
    return ui->chbGenerateComment->isChecked();
}

void mbServerWidgetSettingsScript::setScriptGenerateComment(bool gen)
{
    ui->chbGenerateComment->setChecked(gen);
}

QStringList mbServerWidgetSettingsScript::scriptManualExecutables() const
{
    return m_model->manual();
}

void mbServerWidgetSettingsScript::scriptSetManualExecutables(const QStringList &exec)
{
    m_model->setManual(exec);
}

QString mbServerWidgetSettingsScript::scriptDefaultExecutable() const
{
    return m_model->scriptDefaultExecutable();
}

void mbServerWidgetSettingsScript::scriptSetDefaultExecutable(const QString exec)
{
    m_model->scriptSetDefaultExecutable(exec);
}

void mbServerWidgetSettingsScript::slotPyAdd()
{
    QString s = ui->lnExecPath->text();
    m_model->scriptAddExecutable(s);
}

void mbServerWidgetSettingsScript::slotPySet()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid() && m_model->isManual(index))
    {
        QString s = ui->lnExecPath->text();
        m_model->scriptSetExecutable(index, s);
    }
}

void mbServerWidgetSettingsScript::slotPyRemove()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid())
    {
        m_model->scriptRemoveExecutable(index);
    }
}

void mbServerWidgetSettingsScript::slotPyMakeDefault()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid())
    {
        QString s = m_model->data(index).toString();
        m_model->scriptSetDefaultExecutable(s);
    }
}

void mbServerWidgetSettingsScript::slotPyClear()
{
    m_model->clearManual();
}

void mbServerWidgetSettingsScript::slotPyBrowse()
{
    QString s = mbServer::global()->ui()->dialogs()->getOpenFileName(this, "Browse Python Interpreter");
    if (s.count())
        ui->lnExecPath->setText(s);
}

void mbServerWidgetSettingsScript::selectionChanged(const QItemSelection &selected, const QItemSelection &)
{
    QModelIndexList ls = selected.indexes();
    if (ls.count())
    {
        QModelIndex index = ls.first();
        if (m_model->isManual(index))
        {
            QString s = m_model->manual(index.row());
            ui->lnExecPath->setEnabled(true);
            ui->lnExecPath->setText(s);
        }
        else
        {
            if (m_model->isAuto(index))
            {
                QString s = m_model->autoDetected(index.row());
                ui->lnExecPath->setText(s);
            }
            else
                ui->lnExecPath->setText(QString());
            ui->lnExecPath->setEnabled(false);
        }
    }
}
