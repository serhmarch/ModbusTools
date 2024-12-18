#include "server_widgetsettingsscript.h"
#include "ui_server_widgetsettingsscript.h"

mbServerWidgetSettingsScript::mbServerWidgetSettingsScript(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbServerWidgetSettingsScript)
{
    ui->setupUi(this);
}

mbServerWidgetSettingsScript::~mbServerWidgetSettingsScript()
{
    delete ui;
}
