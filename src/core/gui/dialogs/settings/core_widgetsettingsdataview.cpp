#include "core_widgetsettingsdataview.h"
#include "ui_core_widgetsettingsdataview.h"

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>

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

    setColumns(mbCore::globalCore()->availableDataViewColumns());
    connect(ui->btnEditDataViewColumns, &QPushButton::clicked, this, &mbCoreWidgetSettingsDataView::slotEditColumns);
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

QStringList mbCoreWidgetSettingsDataView::getColumns() const
{
    QStringList res;
    for (int i = 0; i < ui->lsDataViewColumns->count(); i++)
    {
        QListWidgetItem *item = ui->lsDataViewColumns->item(i);
        res.append(item->data(Qt::DisplayRole).toString());
    }
    return res;
}

void mbCoreWidgetSettingsDataView::setColumns(const QStringList &columns)
{
    ui->lsDataViewColumns->clear();
    Q_FOREACH (const QString &column, columns)
        ui->lsDataViewColumns->addItem(column);
}

void mbCoreWidgetSettingsDataView::slotEditColumns()
{
    QStringList ls = getColumns();
    bool res = mbCore::globalCore()->coreUi()->dialogsCore()->getValueList(mbCore::globalCore()->availableDataViewColumns(), ls, QStringLiteral("Edit Columns"));
    if (res)
        setColumns(ls);
}
