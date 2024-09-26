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
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#include <QtHelp/QHelpLink>
#endif

#include <QtHelp/QHelpContentWidget>
#include <QtHelp/QHelpIndexWidget>

#include <core.h>

#include "core_helpbrowser.h"

mbCoreHelpUi::Strings::Strings() :
    prefix(QStringLiteral("Ui.Help.")),
    wGeometry(prefix+QStringLiteral("geometry")),
    wState   (prefix+QStringLiteral("windowState"))
{
}

const mbCoreHelpUi::Strings &mbCoreHelpUi::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreHelpUi::mbCoreHelpUi(const QString &relativeCollectionFile, QWidget *parent) :
    QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Help"));
    this->resize(800, 600);

    QString collectionFile = QApplication::applicationDirPath() + relativeCollectionFile;
    m_helpEngine = new QHelpEngine(collectionFile, this);
//#if (QT_VERSION >= QT_VERSION_CHECK(5, 13, 0))
//    m_helpEngine->setUsesFilterEngine(true);
//#endif

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
    horizSplitter->setSizes(QList<int>() << 180 << 600);

    this->setCentralWidget(horizSplitter);

    textViewer->showHelpForKeyword(mbCore::globalCore()->applicationName());
}

mbCoreHelpUi::~mbCoreHelpUi()
{
}

MBSETTINGS mbCoreHelpUi::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    r[s.wGeometry] = this->saveGeometry();
    r[s.wState   ] = this->saveState();
    return r;
}

void mbCoreHelpUi::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    it = settings.find(s.wGeometry);
    if (it != end)
    {
        this->restoreGeometry(it.value().toByteArray());
    }

    it = settings.find(s.wState);
    if (it != end)
    {
        this->restoreState(it.value().toByteArray());
    }

}
