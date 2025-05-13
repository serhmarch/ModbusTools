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
#include "server_outputview.h"

#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QToolBar>
#include <QCoreApplication>

#include <core.h>
#include <gui/core_ui.h>
#include <gui/dialogs/core_dialogs.h>

mbServerOutputView::Strings::Strings() :
    prefix(QStringLiteral("Ui.ScriptOutput.")),
    font(prefix+QStringLiteral("font"))
{
}

const mbServerOutputView::Strings &mbServerOutputView::Strings::instance()
{
    static const Strings s;
    return s;
}

mbServerOutputView::Defaults::Defaults() :
    font(QFont("Courier New", 8).toString())
{
}

const mbServerOutputView::Defaults &mbServerOutputView::Defaults::instance()
{
    static const Defaults s;
    return s;
}

mbServerOutputView::mbServerOutputView(QWidget *parent)
    : QWidget{parent}
{
    m_toolBar = new QToolBar(this);
    m_toolBar->setIconSize(QSize(16,16));
    m_toolBar->setContentsMargins(0,0,0,0);

    m_view = new QPlainTextEdit(this);
    m_view->setReadOnly(true);
    setFontString(Defaults::instance().font);

    QAction *actionClear = new QAction(m_toolBar);
    actionClear->setIcon(QIcon(":/core/icons/clear.png"));
    actionClear->setText(QCoreApplication::translate("mbServerOutputView", "Clear", nullptr));
    connect(actionClear, &QAction::triggered, this, &mbServerOutputView::clear);
    m_toolBar->addAction(actionClear);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_toolBar);
    layout->addWidget(m_view);

}

QString mbServerOutputView::fontString() const
{
    return m_view->font().toString();
}

void mbServerOutputView::setFontString(const QString &font)
{
    QFont f = m_view->font();
    if (f.fromString(font))
        m_view->setFont(f);
}

MBSETTINGS mbServerOutputView::cachedSettings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    r[s.font] = this->fontString();
    return r;
}

void mbServerOutputView::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    it = settings.find(s.font);
    if (it != end)
    {
        this->setFontString(it.value().toString());
    }

}

void mbServerOutputView::clear()
{
    m_view->clear();
}

void mbServerOutputView::showOutput(const QString &message)
{
    m_view->moveCursor(QTextCursor::End);
    m_view->insertPlainText(message);
    m_view->moveCursor(QTextCursor::End);
}
