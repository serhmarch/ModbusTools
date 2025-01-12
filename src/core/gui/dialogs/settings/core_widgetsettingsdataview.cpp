#include "core_widgetsettingsdataview.h"
#include "ui_core_widgetsettingsdataview.h"

mbCoreWidgetSettingsDataView::mbCoreWidgetSettingsDataView(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsDataView)
{
    ui->setupUi(this);

    QComboBox *cmb;
    cmb = ui->cmbAddressNotation;
    cmb->addItem(mb::toFineString(mb::Address_Modbus));
    cmb->addItem(mb::toFineString(mb::Address_IEC61131));
    cmb->setCurrentIndex(0);
}

mbCoreWidgetSettingsDataView::~mbCoreWidgetSettingsDataView()
{
    delete ui;
}

mb::AddressNotation mbCoreWidgetSettingsDataView::addressNotation() const
{
    return static_cast<mb::AddressNotation>(ui->cmbAddressNotation->currentIndex()+1);
}

void mbCoreWidgetSettingsDataView::setAddressNotation(mb::AddressNotation notation)
{
    ui->cmbAddressNotation->setCurrentIndex(notation-1);
}
