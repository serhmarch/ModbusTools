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
#include "core_helpui.h"

#include <QApplication>
#include <QSplitter>

#include <QtHelp/QHelpEngine>
#include <QtHelp/QHelpLink>

#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>

#include <core.h>

#include "core_helpbrowser.h"

mbCoreHelpUi::mbCoreHelpUi(const QString &relativeCollectionFile, QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Help"));
    this->resize(800, 600);

    QString collectionFile = QApplication::applicationDirPath() + relativeCollectionFile;
    m_helpEngine = new QHelpEngine(collectionFile, this);
    m_helpEngine->setUsesFilterEngine(true);
    if (!m_helpEngine->setupData())
        mbCore::LogError("HelpSystem", "HelpEngine is not setup");

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->addTab(m_helpEngine->contentWidget(), "Contents");
    tabWidget->addTab(m_helpEngine->indexWidget  (), "Index"   );

    mbCoreHelpBrowser *textViewer = new mbCoreHelpBrowser(m_helpEngine, this);
    //textViewer->setSource(QUrl("qthelp://ModbusClient/help/ModbusClient.html"));
    connect(m_helpEngine->contentWidget(), SIGNAL(linkActivated(QUrl))        , textViewer, SLOT(setSource(QUrl)));
    connect(m_helpEngine->indexWidget  (), SIGNAL(linkActivated(QUrl,QString)), textViewer, SLOT(setSource(QUrl)));

    QSplitter *horizSplitter = new QSplitter(Qt::Horizontal, this);
    horizSplitter->insertWidget(0, tabWidget);
    horizSplitter->insertWidget(1, textViewer);
    horizSplitter->setStretchFactor(0,0);
    horizSplitter->setStretchFactor(1,1);
    horizSplitter->setSizes({180, 600});

    this->setCentralWidget(horizSplitter);

    textViewer->showHelpForKeyword(QStringLiteral("AboutModbusTools"));
}

mbCoreHelpUi::~mbCoreHelpUi()
{
}
