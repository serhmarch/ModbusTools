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

    QStringList columns = mbCore::globalCore()->availableDataViewColumns();
    setColumns(columns);
    connect(ui->btnEditDataViewColumns, &QPushButton::clicked, this, &mbCoreWidgetSettingsDataView::slotEditColumns);
}

mbCoreWidgetSettingsDataView::~mbCoreWidgetSettingsDataView()
{
    delete ui;
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
