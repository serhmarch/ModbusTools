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
