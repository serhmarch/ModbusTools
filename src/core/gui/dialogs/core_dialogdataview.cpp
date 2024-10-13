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

#include <project/core_dataview.h>

mbCoreDialogDataView::Strings::Strings() : mbCoreDialogSettings::Strings(),
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
    mbCoreDialogSettings(Strings::instance().cachePrefix, parent),
    ui(new Ui::mbCoreDialogDataView)
{
    ui->setupUi(this);

    mbCoreDataView::Defaults d = mbCoreDataView::Defaults::instance();

    //QMetaEnum e;
    QLineEdit *ln;
    QSpinBox *sp;
    //QComboBox *cmb;

    // Name
    ln = ui->lnName;
    ln->setText(d.name);

    // Period
    sp = ui->spPeriod;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.period);
    
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

    MBSETTINGS m = mbCoreDialogSettings::cachedSettings();
    m[prefix+vs.name     ] = ui->lnName  ->text() ;
    m[prefix+vs.period   ] = ui->spPeriod->value();
    return m;
}

void mbCoreDialogDataView::setCachedSettings(const MBSETTINGS &m)
{
    mbCoreDialogSettings::setCachedSettings(m);

    const mbCoreDataView::Strings &vs = mbCoreDataView::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+vs.name  ); if (it != end) ui->lnName  ->setText (it.value().toString());
    it = m.find(prefix+vs.period); if (it != end) ui->spPeriod->setValue(it.value().toInt   ());
}

MBSETTINGS mbCoreDialogDataView::getSettings(const MBSETTINGS &settings, const QString &title)
{
    MBSETTINGS r;
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
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
}

void mbCoreDialogDataView::fillData(MBSETTINGS &settings)
{
    const mbCoreDataView::Strings &s = mbCoreDataView::Strings::instance();
    
    settings[s.name  ] = ui->lnName  ->text();
    settings[s.period] = ui->spPeriod->value();
}


