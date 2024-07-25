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
#include "core_helpbrowser.h"

#include <QApplication>
#include <QtHelp/QHelpEngine>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#include <QtHelp/QHelpLink>
#endif

mbCoreHelpBrowser::mbCoreHelpBrowser(QHelpEngine *helpEngine, QWidget *parent) :
    QTextBrowser(parent),
    m_helpEngine(helpEngine)
{
    QFont font;
    font.setPointSize(11);
    this->setFont(font);
}

QVariant mbCoreHelpBrowser::loadResource(int /*type*/, const QUrl &name)
{
    QUrl url(name);
    if (name.isRelative())
        url = source().resolved(url);
    QByteArray ba = m_helpEngine->fileData(url);
    return ba;
}

void mbCoreHelpBrowser::showHelpForKeyword(const QString &id)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    QList<QHelpLink> links = m_helpEngine->documentsForIdentifier(id);
    if (links.count())
        setSource(links.first().url);
#else
    QMap<QString, QUrl> links = m_helpEngine->linksForIdentifier(id);
    if (links.count())
        setSource(links.first());
#endif
}
