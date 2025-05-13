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
#ifndef SERVER_OUTPUTVIEW_H
#define SERVER_OUTPUTVIEW_H

#include <QWidget>
#include <mbcore.h>

class QPlainTextEdit;
class QToolBar;

class mbServerOutputView : public QWidget
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString prefix;
        const QString font;
        Strings();
        static const Strings &instance();
    };

    struct Defaults
    {
        const QString font;
        Defaults();
        static const Defaults &instance();
    };

public:
    explicit mbServerOutputView(QWidget *parent = nullptr);

public:
    QString fontString() const;
    void setFontString(const QString &font);

    MBSETTINGS cachedSettings() const;
    void setCachedSettings(const MBSETTINGS &settings);

public Q_SLOTS:
    void clear();
    void showOutput(const QString& message);

Q_SIGNALS:

protected:
    QToolBar *m_toolBar;
    QPlainTextEdit *m_view;
};

#endif // SERVER_OUTPUTVIEW_H
