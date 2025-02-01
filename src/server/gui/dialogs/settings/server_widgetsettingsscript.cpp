#include "server_widgetsettingsscript.h"
#include "ui_server_widgetsettingsscript.h"

#include <server.h>
#include <gui/server_ui.h>

#include "../server_dialogs.h"

#include "server_modelsettingsscripteditorcolors.h"
#include "server_delegatesettingsscripteditorcolors.h"

#include "server_modelsettingsscriptinterpreters.h"

#include <gui/script/editor/server_scripthighlighter.h>

mbServerWidgetSettingsScript::mbServerWidgetSettingsScript(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbServerWidgetSettingsScript)
{
    ui->setupUi(this);

    mbServer *server = mbServer::global();
    setScriptEnable(server->scriptEnable());
    setScriptEnable(server->scriptUseOptimization());

    m_modelEditorColors = new mbServerModelSettingsScriptEditorColors(this);
    connect(ui->btnDefaultEditorColors, &QToolButton::clicked,
            m_modelEditorColors, &mbServerModelSettingsScriptEditorColors::setDefaultEditorColors);
    ui->viewEditorColorFormats->setModel(m_modelEditorColors);
    ui->viewEditorColorFormats->setItemDelegate(new mbServerDelegateSettingsScriptEditorColors(this));

    m_modelInterpreters = new mbServerModelSettingsScriptInterpreters(this);
    m_modelInterpreters->setAutoDetected(mbServer::global()->scriptAutoDetectedExecutables());
    ui->viewInterpreters->setModel(m_modelInterpreters);

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

bool mbServerWidgetSettingsScript::scriptUseOptimization() const
{
    return ui->chbScriptUseOptimization->isChecked();
}

void mbServerWidgetSettingsScript::setScriptUseOptimization(bool use)
{
    ui->chbScriptUseOptimization->setChecked(use);
}

bool mbServerWidgetSettingsScript::scriptGenerateComment() const
{
    return ui->chbGenerateComment->isChecked();
}

void mbServerWidgetSettingsScript::setScriptGenerateComment(bool gen)
{
    ui->chbGenerateComment->setChecked(gen);
}

QString mbServerWidgetSettingsScript::scriptEditorColorFormars() const
{
    return mbServerScriptHighlighter::toString(m_modelEditorColors->colorFormats());
}

void mbServerWidgetSettingsScript::scriptSetEditorColorFormars(const QString &formats)
{
    mbServerScriptHighlighter::ColorFormats cf = mbServerScriptHighlighter::toColorFormats(formats);
    m_modelEditorColors->setColorFormats(cf);
}

QStringList mbServerWidgetSettingsScript::scriptManualExecutables() const
{
    return m_modelInterpreters->manual();
}

void mbServerWidgetSettingsScript::scriptSetManualExecutables(const QStringList &exec)
{
    m_modelInterpreters->setManual(exec);
}

QString mbServerWidgetSettingsScript::scriptDefaultExecutable() const
{
    return m_modelInterpreters->scriptDefaultExecutable();
}

void mbServerWidgetSettingsScript::scriptSetDefaultExecutable(const QString &exec)
{
    m_modelInterpreters->scriptSetDefaultExecutable(exec);
}

void mbServerWidgetSettingsScript::slotPyAdd()
{
    QString s = ui->lnExecPath->text();
    if (s.isEmpty())
        s = QStringLiteral("Unnamed");
    m_modelInterpreters->scriptAddExecutable(s);
}

void mbServerWidgetSettingsScript::slotPySet()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid() && m_modelInterpreters->isManual(index))
    {
        QString s = ui->lnExecPath->text();
        m_modelInterpreters->scriptSetExecutable(index, s);
    }
}

void mbServerWidgetSettingsScript::slotPyRemove()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid())
    {
        m_modelInterpreters->scriptRemoveExecutable(index);
    }
}

void mbServerWidgetSettingsScript::slotPyMakeDefault()
{
    QItemSelectionModel *sel = ui->viewInterpreters->selectionModel();
    QModelIndex index = sel->currentIndex();
    if (index.isValid())
    {
        QString s = m_modelInterpreters->data(index).toString();
        m_modelInterpreters->scriptSetDefaultExecutable(s);
    }
}

void mbServerWidgetSettingsScript::slotPyClear()
{
    m_modelInterpreters->clearManual();
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
        if (m_modelInterpreters->isManual(index))
        {
            QString s = m_modelInterpreters->manual(index.row());
            ui->lnExecPath->setEnabled(true);
            ui->lnExecPath->setText(s);
        }
        else
        {
            if (m_modelInterpreters->isAuto(index))
            {
                QString s = m_modelInterpreters->autoDetected(index.row());
                ui->lnExecPath->setText(s);
            }
            else
                ui->lnExecPath->setText(QString());
            ui->lnExecPath->setEnabled(false);
        }
    }
}
