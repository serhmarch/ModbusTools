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
#ifndef CORE_DIALOGS_H
#define CORE_DIALOGS_H

#include <QInputDialog>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>

#include "core_dialogreplace.h"

#include <project/core_project.h>

class mbCore;
class mbCoreDialogName;
class mbCoreDialogSettings;
class mbCoreDialogProjectInfo;
class mbCoreDialogProject;
class mbCoreDialogPort;
class mbCoreDialogDevice;
class mbCoreDialogDataView;
class mbCoreDialogDataViewItem;
class mbCoreDialogValueList;
class mbCoreDialogEdit;

class MB_EXPORT mbCoreDialogs
{
public:
    struct MB_EXPORT Strings
    {
        const QString settings_lastDir;
        const QString settings_lastFilter;
        Strings();
        static const Strings &instance();
    };

public:
    enum Filter
    {
        Filter_ProjectFiles      = 0x00000001,
        Filter_XmlFiles          = 0x00000002,
        Filter_CsvFiles          = 0x00000004,
        Filter_AllFiles          = 0x80000000,
        Filter_ProjectAll        = Filter_ProjectFiles | Filter_XmlFiles | Filter_AllFiles,
        Filter_PortAll           = Filter_XmlFiles | Filter_AllFiles,
        Filter_DeviceAll         = Filter_XmlFiles | Filter_AllFiles,
        Filter_DataViewAll       = Filter_CsvFiles | Filter_XmlFiles | Filter_AllFiles,
        Filter_DataViewItemsAll  = Filter_CsvFiles | Filter_XmlFiles | Filter_AllFiles,
    };

public:
    explicit mbCoreDialogs(QWidget* parent = nullptr);
    virtual ~mbCoreDialogs();

public:
    int replace(const QString &title, const QString &label, bool useAllButtons = false);
    QString getText(QWidget *parent, const QString &title, const QString &label,
                    QLineEdit::EchoMode echo = QLineEdit::Normal,
                    const QString &text = QString(), bool *ok = nullptr,
                    Qt::WindowFlags flags = Qt::WindowFlags(),
                    Qt::InputMethodHints inputMethodHints = Qt::ImhNone);
    QString getOpenFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString(),
                            const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = QFileDialog::Options());
    QString getSaveFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString(),
                            const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = QFileDialog::Options());
    QColor getColor(const QColor &initial = Qt::white, QWidget *parent = nullptr, const QString &title = QString(),
                    QColorDialog::ColorDialogOptions options = QColorDialog::ColorDialogOptions());
    bool getFont(QFont &font, QWidget *parent = nullptr, const QString &title = QString(),
                    QFontDialog::FontDialogOptions options = QFontDialog::FontDialogOptions());
    QString getExistingDirectory(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString(),
                                 QFileDialog::Options options = QFileDialog::ShowDirsOnly);
    bool editSystemSettings(const QString& title = QString());
    void showProjectInfo(mbCoreProject *project);

    MBSETTINGS getProject      (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getPort         (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDevice       (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDataView     (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDataViewItem (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    bool getValueList(const QVariantList &all, QVariantList &current, const QString &title = QString());
    bool getValueList(const QStringList &all, QStringList &current, const QString &title = QString());
    virtual MBSETTINGS cachedSettings() const;
    virtual void setCachedSettings(const MBSETTINGS &settings);

public:
    QString getFilterString(int filter) const;

protected:
    QString m_projectFilter; // Note: must be set in derived classes
    QString m_lastDir;
    QString m_lastFilter;

protected:
    mbCoreDialogReplace        *m_replace     ;
    mbCoreDialogSettings       *m_settings    ;
    mbCoreDialogProjectInfo    *m_projectInfo ;
    mbCoreDialogProject        *m_project     ;
    mbCoreDialogPort           *m_port        ;
    mbCoreDialogDevice         *m_device      ;
    mbCoreDialogDataView       *m_dataView    ;
    mbCoreDialogDataViewItem   *m_dataViewItem;
    mbCoreDialogValueList      *m_valueList   ;
};

#endif // CORE_DIALOGS_H
