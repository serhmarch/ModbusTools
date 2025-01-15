/*
    Modbus Tools

    Created: 2023
    Author: Serhii Marchuk, https://github.com/serhmarch

    Copyright (C) 2023  Serhii Marchuk

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "core_dialogdataview.h"
#include "ui_core_dialogdataview.h"

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>
#include <project/core_dataview.h>

mbCoreDialogDataView::Strings::Strings() : mbCoreDialogEdit::Strings(),
    title(QStringLiteral("DataView")),
    cachePrefix(QStringLiteral("Ui.Dialogs.DataView."))
{
}

const mbCoreDialogDataView::Strings &mbCoreDialogDataView::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogDataView::mbCoreDialogDataView(QWidget *parent) :
    mbCoreDialogEdit(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbCoreDialogDataView)
{
    ui->setupUi(this);

    mbCoreDataView::Defaults d = mbCoreDataView::Defaults::instance();

    //QMetaEnum e;
    QLineEdit *ln;
    QSpinBox *sp;
    QComboBox *cmb;
    QCheckBox *chb;

    // Name
    ln = ui->lnName;
    ln->setText(d.name);

    // Period
    sp = ui->spPeriod;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.period);
    
    // Address Notation
    cmb =  ui->cmbAddressNotation;
    cmb->addItem(mb::toString(mb::Address_Default));
    cmb->addItem(mb::toFineString(mb::Address_Modbus));
    cmb->addItem(mb::toFineString(mb::Address_IEC61131));

    chb = ui->chbUseDefaultColumns;
    connect(chb, &QCheckBox::stateChanged, this, &mbCoreDialogDataView::slotUseDefaultColumnsChange);
    chb->setChecked(d.useDefaultColumns);

    setColumns(mbCore::globalCore()->availableDataViewColumns());
    connect(ui->btnEditDataViewColumns, &QPushButton::clicked, this, &mbCoreDialogDataView::slotEditColumns);

    //===================================================
    connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

mbCoreDialogDataView::~mbCoreDialogDataView()
{
    delete ui;
}

MBSETTINGS mbCoreDialogDataView::cachedSettings() const
{
    const mbCoreDataView::Strings &vs = mbCoreDataView::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS m = mbCoreDialogEdit::cachedSettings();
    m[prefix+vs.name             ] = ui->lnName  ->text() ;
    m[prefix+vs.period           ] = ui->spPeriod->value();
    m[prefix+vs.addressNotation  ] = mb::toString(addressNotation());

    bool useDefaultColumns = ui->chbUseDefaultColumns->isChecked();
    m[prefix+vs.useDefaultColumns] = useDefaultColumns;
    if (useDefaultColumns)
        m[prefix+vs.columns] = getColumns();
    return m;
}

void mbCoreDialogDataView::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogEdit::setCachedSettings(m);

    const mbCoreDataView::Strings &vs = mbCoreDataView::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.name  );            if (it != end) ui->lnName  ->setText (it.value().toString());
    it = m.find(prefix+vs.period);            if (it != end) ui->spPeriod->setValue(it.value().toInt   ());
    it = m.find(prefix+vs.addressNotation);   if (it != end) setAddressNotation(mb::toAddressNotation(it.value()));
    it = m.find(prefix+vs.useDefaultColumns); if (it != end) ui->chbUseDefaultColumns->setChecked(it.value().toBool());
    it = m.find(prefix+vs.columns);           if (it != end) setColumns(it.value().toStringList());
}

MBSETTINGS mbCoreDialogDataView::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    QString defaultNotation = QString("Default (%1)").arg(mb::toString(mbCore::globalCore()->addressNotation()));
    ui->cmbAddressNotation->setItemText(0, defaultNotation);
    if (settings.count())
        fillForm(settings);
    // ----------------------
    switch (QDialog::exec())
    {
    case QDialog::Accepted:
        fillData(r);
    }
    return r;
}

void mbCoreDialogDataView::fillForm(const MBSETTINGS &m)
{
    const mbCoreDataView::Strings &vs = mbCoreDataView::Strings::instance();
    
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(vs.name  ); if (it != end) ui->lnName  ->setText (it.value().toString());
    it = m.find(vs.period); if (it != end) ui->spPeriod->setValue(it.value().toInt   ());

    it = m.find(vs.addressNotation);
    if (it != end)
        setAddressNotation(mb::toAddressNotation(it.value()));

    ui->chbUseDefaultColumns->setChecked(m.value(vs.useDefaultColumns).toBool());
    setColumns(m.value(vs.columns).toStringList());
}

void mbCoreDialogDataView::fillData(MBSETTINGS &settings)
{
    const mbCoreDataView::Strings &s = mbCoreDataView::Strings::instance();
    
    settings[s.name             ] = ui->lnName  ->text();
    settings[s.period           ] = ui->spPeriod->value();
    settings[s.addressNotation  ] = addressNotation();
    settings[s.useDefaultColumns] = ui->chbUseDefaultColumns->isChecked();
    settings[s.columns          ] = getColumns();
}

mb::AddressNotation mbCoreDialogDataView::addressNotation() const
{
    return static_cast<mb::AddressNotation>(ui->cmbAddressNotation->currentIndex());
}

void mbCoreDialogDataView::setAddressNotation(mb::AddressNotation notation)
{
    ui->cmbAddressNotation->setCurrentIndex(notation);
}

QStringList mbCoreDialogDataView::getColumns() const
{
    QStringList res;
    for (int i = 0; i < ui->lsDataViewColumns->count(); i++)
    {
        QListWidgetItem *item = ui->lsDataViewColumns->item(i);
        res.append(item->data(Qt::DisplayRole).toString());
    }
    return res;
}

void mbCoreDialogDataView::setColumns(const QStringList &columns)
{
    ui->lsDataViewColumns->clear();
    Q_FOREACH (const QString &column, columns)
        ui->lsDataViewColumns->addItem(column);
}

void mbCoreDialogDataView::slotUseDefaultColumnsChange(int state)
{
    ui->grDataViewColumns->setEnabled(state != Qt::Checked);
}

void mbCoreDialogDataView::slotEditColumns()
{
    QStringList ls = getColumns();
    bool res = mbCore::globalCore()->coreUi()->dialogsCore()->getValueList(mbCore::globalCore()->availableDataViewColumns(), ls, QStringLiteral("Edit Columns"));
    if (res)
        setColumns(ls);
}
