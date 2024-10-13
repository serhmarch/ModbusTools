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
#include "client_dialogdataviewitem.h"
#include "ui_client_dialogdataviewitem.h"

#include <QMetaEnum>

#include <client.h>
#include <project/client_project.h>
#include <project/client_device.h>
#include <project/client_dataview.h>

mbClientDialogDataViewItem::Strings::Strings()
{
}

const mbClientDialogDataViewItem::Strings &mbClientDialogDataViewItem::Strings::instance()
{
    static const Strings s;
    return s;
}

mbClientDialogDataViewItem::mbClientDialogDataViewItem(QWidget *parent) :
    mbCoreDialogDataViewItem(parent),
    ui(new Ui::mbClientDialogDataViewItem)
{
    ui->setupUi(this);

    const mbClientDataViewItem::Defaults &d = mbClientDataViewItem::Defaults::instance();

    QSpinBox *sp;

    // Period
    sp = ui->spPeriod;
    sp->setMinimum(0);
    sp->setMaximum(INT_MAX);
    sp->setValue(d.period);

    m_ui.cmbDevice                         = ui->cmbDevice                        ;
    m_ui.cmbAdrType                        = ui->cmbAdrType                       ;
    m_ui.spOffset                          = ui->spOffset                         ;
    m_ui.spCount                           = ui->spCount                          ;
    m_ui.cmbFormat                         = ui->cmbFormat                        ;
    m_ui.spLength                          = ui->spLength                         ;
    m_ui.cmbByteOrder                      = ui->cmbByteOrder                     ;
    m_ui.cmbRegisterOrder                  = ui->cmbRegisterOrder                 ;
    m_ui.cmbByteArrayFormat                = ui->cmbByteArrayFormat               ;
    m_ui.lnByteArraySeparator              = ui->lnByteArraySeparator             ;
    m_ui.btnTogleDefaultByteArraySeparator = ui->btnTogleDefaultByteArraySeparator;
    m_ui.cmbStringLengthType               = ui->cmbStringLengthType              ;
    m_ui.cmbStringEncoding                 = ui->cmbStringEncoding                ;
    m_ui.tabWidget                         = ui->tabWidget                        ;
    m_ui.buttonBox                         = ui->buttonBox                        ;

    initializeBaseUi();
}

mbClientDialogDataViewItem::~mbClientDialogDataViewItem()
{
    delete ui;
}

MBSETTINGS mbClientDialogDataViewItem::cachedSettings() const
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    MBSETTINGS settings = mbCoreDialogDataViewItem::cachedSettings();
    const QString &prefix = Strings().cachePrefix;
    settings[prefix+sItem.period] = ui->spPeriod->value();
    return settings;
}

void mbClientDialogDataViewItem::setCachedSettings(const MBSETTINGS &settings)
{
    mbCoreDialogDataViewItem::setCachedSettings(settings);

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    const QString &prefix = Strings().cachePrefix;

    it = settings.find(prefix+sItem.period); if (it != end) ui->spPeriod->setValue(it.value().toInt());
}

void mbClientDialogDataViewItem::fillFormInner(const MBSETTINGS &settings)
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    int period = settings.value(sItem.period).toInt();
    ui->spPeriod->setValue(period);
}

void mbClientDialogDataViewItem::fillDataInner(MBSETTINGS &settings) const
{
    const mbClientDataViewItem::Strings &sItem = mbClientDataViewItem::Strings::instance();
    settings[sItem.period] = ui->spPeriod->value();
}
