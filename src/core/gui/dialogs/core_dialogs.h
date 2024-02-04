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

#include <project/core_project.h>

class mbCore;
class mbCoreDialogName;
class mbCoreDialogSystemSettings;
class mbCoreDialogProject;
class mbCoreDialogPort;
class mbCoreDialogDevice;
class mbCoreDialogDataView;
class mbCoreDialogDataViewItem;
class mbCoreDialogSettings;

class MB_EXPORT mbCoreDialogs
{
public:
    struct MB_EXPORT Strings
    {
        const QString settings_lastDir;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreDialogs(QWidget* parent = nullptr);
    virtual ~mbCoreDialogs();

public:
    QString getText(QWidget *parent, const QString &title, const QString &label,
                    QLineEdit::EchoMode echo = QLineEdit::Normal,
                    const QString &text = QString(), bool *ok = nullptr,
                    Qt::WindowFlags flags = Qt::WindowFlags(),
                    Qt::InputMethodHints inputMethodHints = Qt::ImhNone);
    QString getOpenFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString(),
                            const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = QFileDialog::Options());
    QString getSaveFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString(),
                            const QString &filter = QString(), QString *selectedFilter = nullptr, QFileDialog::Options options = QFileDialog::Options());
    QString getImportFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString());
    QString getExportFileName(QWidget *parent = nullptr, const QString &caption = QString(), const QString &dir = QString());

    QString getName(const QString &oldName = QString(), const QString &title = QString());

    bool editSystemSettings(const QString& title = QString());

    MBSETTINGS getProject      (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getPort         (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDevice       (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDataView     (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());
    MBSETTINGS getDataViewItem (const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString());

    virtual MBSETTINGS settings() const;
    virtual void setSettings(const MBSETTINGS &settings);

protected:
    QString m_lastDir;
    QString m_importExportFilter;

protected:
    mbCoreDialogName           *m_name        ;
    mbCoreDialogSystemSettings *m_settings    ;
    mbCoreDialogProject        *m_project     ;
    mbCoreDialogPort           *m_port        ;
    mbCoreDialogDevice         *m_device      ;
    mbCoreDialogDataView       *m_dataView    ;
    mbCoreDialogDataViewItem   *m_dataViewItem;
};

#endif // CORE_DIALOGS_H
