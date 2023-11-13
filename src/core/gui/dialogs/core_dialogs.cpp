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
#include "core_dialogs.h"

#include <core.h>

#include <gui/core_ui.h>

#include "core_dialogname.h"
#include "core_dialogsystemsettings.h"
#include "core_dialogproject.h"
#include "core_dialogport.h"
#include "core_dialogdevice.h"
#include "core_dialogdataview.h"
#include "core_dialogdataviewitem.h"

mbCoreDialogs::Strings::Strings() :
    settings_lastDir(QStringLiteral("Core.Ui.Dialogs.LastDir"))
{
}

const mbCoreDialogs::Strings &mbCoreDialogs::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogs::mbCoreDialogs(QWidget *parent)
{
    m_importExportFilter = "XML files (*.xml)";

    m_name         = new mbCoreDialogName(parent);
    m_settings     = new mbCoreDialogSystemSettings(parent);
    m_project      = new mbCoreDialogProject(parent);
    m_dataView     = new mbCoreDialogDataView(parent);
    m_port         = nullptr;
    m_device       = nullptr;
    m_dataViewItem = nullptr;
}

mbCoreDialogs::~mbCoreDialogs()
{
}

QString mbCoreDialogs::getText(QWidget *parent, const QString &title, const QString &label, QLineEdit::EchoMode echo, const QString &text, bool *ok, Qt::WindowFlags flags, Qt::InputMethodHints inputMethodHints)
{
    return QInputDialog::getText(parent, title, label, echo, text, ok, flags, inputMethodHints);
}

QString mbCoreDialogs::getOpenFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    QString d = dir.isEmpty() ? m_lastDir : dir;
    QString f = QFileDialog::getOpenFileName(parent, caption, d, filter, selectedFilter, options);
    if (!f.isEmpty())
        m_lastDir = QFileInfo(f).absolutePath();
    return f;
}

QString mbCoreDialogs::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    QString d = dir.isEmpty() ? m_lastDir : dir;
    QString f = QFileDialog::getSaveFileName(parent, caption, d, filter, selectedFilter, options);
    if (!f.isEmpty())
        m_lastDir = QFileInfo(f).absolutePath();
    return f;
}

QString mbCoreDialogs::getImportFileName(QWidget *parent, const QString &caption, const QString &dir)
{
    return getOpenFileName(parent, caption, dir, m_importExportFilter);
}

QString mbCoreDialogs::getExportFileName(QWidget *parent, const QString &caption, const QString &dir)
{
    return getSaveFileName(parent, caption, dir, m_importExportFilter);
}

QString mbCoreDialogs::getName(const QString &oldName, const QString &title)
{
    return m_name->getName(oldName, title);
}

bool mbCoreDialogs::editSystemSettings(const QString &title)
{
    return m_settings->editSystemSettings(title);
}

MBSETTINGS mbCoreDialogs::getProject(const MBSETTINGS &settings, const QString &title)
{
    return m_project->getSettings(settings, title);
}

MBSETTINGS mbCoreDialogs::getDataView(const MBSETTINGS &settings, const QString &title)
{
    return m_dataView->getSettings(settings, title);
}

MBSETTINGS mbCoreDialogs::getPort(const MBSETTINGS &settings, const QString &title)
{
    return m_port->getSettings(settings, title);
}

MBSETTINGS mbCoreDialogs::getDevice(const MBSETTINGS &settings, const QString &title)
{
    return m_device->getSettings(settings, title);
}

MBSETTINGS mbCoreDialogs::getDataViewItem(const MBSETTINGS &settings, const QString &title)
{
    return m_dataViewItem->getSettings(settings, title);
}

MBSETTINGS mbCoreDialogs::settings() const
{
    const Strings &s = Strings::instance();
    MBSETTINGS r;
    r[s.settings_lastDir] = m_lastDir;
    return r;
}

void mbCoreDialogs::setSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    it = settings.find(s.settings_lastDir);
    if (it != end)
    {
        QString v = it.value().toString();
        m_lastDir = v;
    }

}
