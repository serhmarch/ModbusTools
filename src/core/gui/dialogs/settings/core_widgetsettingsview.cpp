#include "core_widgetsettingsview.h"
#include "ui_core_widgetsettingsview.h"

mbCoreWidgetSettingsView::mbCoreWidgetSettingsView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsView)
{
    ui->setupUi(this);

    QComboBox *cmb;
    cmb = ui->cmbAddressNotation;
    cmb->addItem(mb::toFineString(mb::Address_Modbus));
    cmb->addItem(mb::toFineString(mb::Address_IEC61131));
    cmb->setCurrentIndex(0);
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

mb::AddressNotation mbCoreWidgetSettingsView::addressNotation() const
{
    return static_cast<mb::AddressNotation>(ui->cmbAddressNotation->currentIndex()+1);
}

void mbCoreWidgetSettingsView::setAddressNotation(mb::AddressNotation notation)
{
    ui->cmbAddressNotation->setCurrentIndex(notation-1);
}

