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

#include "settings/core_dialogsettings.h"
#include "core_dialogprojectinfo.h"
#include "core_dialogproject.h"
#include "core_dialogport.h"
#include "core_dialogdevice.h"
#include "core_dialogdataview.h"
#include "core_dialogdataviewitem.h"
#include "core_dialogvaluelist.h"

mbCoreDialogs::Strings::Strings() :
    settings_lastDir(QStringLiteral   ("Ui.Dialogs.lastDir")),
    settings_lastFilter(QStringLiteral("Ui.Dialogs.lastFilter"))

{
}

const mbCoreDialogs::Strings &mbCoreDialogs::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogs::mbCoreDialogs(QWidget *parent)
{
    m_settings     = nullptr;
    m_projectInfo  = new mbCoreDialogProjectInfo(parent);
    m_project      = new mbCoreDialogProject(parent);
    m_dataView     = new mbCoreDialogDataView(parent);
    m_port         = nullptr;
    m_device       = nullptr;
    m_dataViewItem = nullptr;
    m_valueList    = new mbCoreDialogValueList(parent);
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
    QString tLastDir = dir.isEmpty() ? m_lastDir : dir;
    QString tSelectedFilter = m_lastFilter;
    QString f = QFileDialog::getOpenFileName(parent, caption, tLastDir, filter, &tSelectedFilter, options);
    if (!f.isEmpty())
    {
        m_lastDir = QFileInfo(f).absolutePath();
        if (!tSelectedFilter.isEmpty())
            m_lastFilter = tSelectedFilter;
        if (selectedFilter)
            *selectedFilter = tSelectedFilter;
    }
    return f;
}

QString mbCoreDialogs::getSaveFileName(QWidget *parent, const QString &caption, const QString &dir, const QString &filter, QString *selectedFilter, QFileDialog::Options options)
{
    QString tLastDir = dir.isEmpty() ? m_lastDir : dir;
    QString tSelectedFilter = m_lastFilter;
    QString f = QFileDialog::getSaveFileName(parent, caption, tLastDir, filter, &tSelectedFilter, options);
    if (!f.isEmpty())
    {
        m_lastDir = QFileInfo(f).absolutePath();
        if (!tSelectedFilter.isEmpty())
            m_lastFilter = tSelectedFilter;
        if (selectedFilter)
            *selectedFilter = tSelectedFilter;
    }
    return f;
}

QColor mbCoreDialogs::getColor(const QColor &initial, QWidget *parent, const QString &title, QColorDialog::ColorDialogOptions options)
{
    return QColorDialog::getColor(initial, parent, title, options);
}

bool mbCoreDialogs::getFont(QFont &font, QWidget *parent, const QString &title, QFontDialog::FontDialogOptions options)
{
    bool ok;
    font = QFontDialog::getFont(&ok, font, parent, title, options);
    return ok;
}

QString mbCoreDialogs::getExistingDirectory(QWidget *parent, const QString &caption, const QString &dir, QFileDialog::Options options)
{
    QString tLastDir = dir.isEmpty() ? m_lastDir : dir;
    QString f = QFileDialog::getExistingDirectory(parent, caption, tLastDir, options);
    if (!f.isEmpty())
    {
        m_lastDir = QFileInfo(f).absolutePath();
    }
    return f;
}

bool mbCoreDialogs::editSystemSettings(const QString &title)
{
    return m_settings->editSettings(title);
}

void mbCoreDialogs::showProjectInfo(mbCoreProject *project)
{
    m_projectInfo->showProjectInfo(project);
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

bool mbCoreDialogs::getValueList(const QVariantList &all, QVariantList &current, const QString &title)
{
    return m_valueList->getValueList(all, current, title);
}

bool mbCoreDialogs::getValueList(const QStringList &all, QStringList &current, const QString &title)
{
    return m_valueList->getValueList(all, current, title);
}

MBSETTINGS mbCoreDialogs::cachedSettings() const
{
    const Strings &s = Strings::instance();

    MBSETTINGS r = m_port->cachedSettings();
    mb::unite(r, m_device      ->cachedSettings());
    mb::unite(r, m_dataView    ->cachedSettings());
    mb::unite(r, m_dataViewItem->cachedSettings());
    mb::unite(r, m_projectInfo ->cachedSettings());
    mb::unite(r, m_project     ->cachedSettings());
    mb::unite(r, m_valueList   ->cachedSettings());
    mb::unite(r, m_settings    ->cachedSettings());

    r[s.settings_lastDir   ] = m_lastDir   ;
    r[s.settings_lastFilter] = m_lastFilter;
    return r;
}

void mbCoreDialogs::setCachedSettings(const MBSETTINGS &settings)
{
    const Strings &s = Strings::instance();
    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = settings.end();
    //bool ok;

    m_port        ->setCachedSettings(settings);
    m_device      ->setCachedSettings(settings);
    m_dataView    ->setCachedSettings(settings);
    m_dataViewItem->setCachedSettings(settings);
    m_projectInfo ->setCachedSettings(settings);
    m_project     ->setCachedSettings(settings);
    m_valueList   ->setCachedSettings(settings);
    m_settings    ->setCachedSettings(settings);

    it = settings.find(s.settings_lastDir);
    if (it != end)
    {
        QString v = it.value().toString();
        m_lastDir = v;
    }


    it = settings.find(s.settings_lastFilter);
    if (it != end)
    {
        QString v = it.value().toString();
        m_lastFilter = v;
    }
}

QString mbCoreDialogs::getFilterString(int filter) const
{
    QStringList filters;
    if (filter & Filter_ProjectFiles)
        filters.append(m_projectFilter);
    if (filter & Filter_XmlFiles)
        filters.append(QStringLiteral("XML files (*.xml)"));
    if (filter & Filter_CsvFiles)
        filters.append(QStringLiteral("CSV files (*.csv)"));
    if (filter & Filter_AllFiles)
        filters.append(QStringLiteral("All files (*.*)"));
    return filters.join(QStringLiteral(";;"));
}
