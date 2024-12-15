#include "core_widgetsettingslog.h"
#include "ui_core_widgetsettingslog.h"

mbCoreWidgetSettingsLog::mbCoreWidgetSettingsLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsLog)
{
    ui->setupUi(this);
}

mbCoreWidgetSettingsLog::~mbCoreWidgetSettingsLog()
{
    delete ui;
}
