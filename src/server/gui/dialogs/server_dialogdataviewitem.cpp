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
#include "server_dialogdataviewitem.h"
#include "ui_server_dialogdataviewitem.h"

#include <QMetaEnum>

#include <server.h>
#include <project/server_project.h>
#include <project/server_device.h>
#include <project/server_dataview.h>

mbServerDialogDataViewItem::mbServerDialogDataViewItem(QWidget *parent) :
    mbCoreDialogDataViewItem(parent),
    ui(new Ui::mbServerDialogDataViewItem)
{
    ui->setupUi(this);

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

mbServerDialogDataViewItem::~mbServerDialogDataViewItem()
{
    delete ui;
}
