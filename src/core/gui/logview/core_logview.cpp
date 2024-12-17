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
#include "core_logview.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QTableView>
#include <QToolBar>

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>

#include "core_logviewmodel.h"

mbCoreLogView::mbCoreLogView(QWidget *parent)
    : QWidget{parent}
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16,16));
    m_toolBar->setContentsMargins(0,0,0,0);

    m_view = new QTableView(this);
    m_model = new mbCoreLogViewModel(m_view);
    m_view->setModel(m_model);
    QHeaderView *header;
    header = m_view->horizontalHeader();
    header->setStretchLastSection(true);
    header->hide();
    header = m_view->verticalHeader();
    header->setSectionResizeMode(QHeaderView::ResizeToContents);
    header->hide();

    QAction *actionClear = new QAction(m_toolBar);
    actionClear->setIcon(QIcon(":/core/icons/clear.png"));
    connect(actionClear, &QAction::triggered, this, &mbCoreLogView::clear);
    m_toolBar->addAction(actionClear);

    QAction *actionExportLog = new QAction(m_toolBar);
    actionExportLog->setIcon(QIcon(":/core/icons/logexport.png"));
    connect(actionExportLog, &QAction::triggered, this, &mbCoreLogView::exportLog);
    m_toolBar->addAction(actionExportLog);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_view);

}

void mbCoreLogView::clear()
{
    m_model->clear();
}

void mbCoreLogView::exportLog()
{
    //mbCoreUi *ui = mbCore::globalCore()->coreUi();
    //QString fileName =ui->dialogsCore()->getSaveFileName(ui, QStringLiteral("Export Log"), QString(), QStringLiteral("Text files (*.txt);;All files (*)"));
    //if (fileName.isEmpty())
    //    return;
    //QFile file(fileName);
    //if (!file.open(QFile::WriteOnly))
    //    return;
    //file.write(m_text->toPlainText().toUtf8());
    //file.close();
}

void mbCoreLogView::logMessage(mb::LogFlag flag, const QString &source, const QString &text)
{
    m_model->logMessage(flag, source, text);
}
