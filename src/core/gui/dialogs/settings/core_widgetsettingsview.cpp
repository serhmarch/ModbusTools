#include "core_widgetsettingsview.h"
#include "ui_core_widgetsettingsview.h"

mbCoreWidgetSettingsView::mbCoreWidgetSettingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsView)
{
    ui->setupUi(this);
}

mbCoreWidgetSettingsView::~mbCoreWidgetSettingsView()
{
    delete ui;
}

bool mbCoreWidgetSettingsView::useNameWithSettings() const
{
    return ui->chbUseNameWithSettings->isChecked();
}

void mbCoreWidgetSettingsView::setUseNameWithSettings(bool use)
{
    ui->chbUseNameWithSettings->setChecked(use);
}
