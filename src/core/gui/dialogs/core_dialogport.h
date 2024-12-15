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
#ifndef CORE_DIALOGPORT_H
#define CORE_DIALOGPORT_H

#include "core_dialogedit.h"

class QLineEdit;
class QComboBox;
class QSpinBox;
class QToolButton;
class QTabWidget;
class QStackedWidget;
class QDialogButtonBox;

class MB_EXPORT mbCoreDialogPort : public mbCoreDialogEdit
{
    Q_OBJECT
public:
    struct MB_EXPORT Strings : public mbCoreDialogEdit::Strings
    {
        const QString title;
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogPort(QWidget *parent = nullptr);

protected:
    void initializeBaseUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

protected:
    void fillForm(const MBSETTINGS& params);
    void fillData(MBSETTINGS& params) const;

protected Q_SLOTS:
    void setType(int type);

protected:
    virtual void fillFormInner(const MBSETTINGS &settings);
    virtual void fillDataInner(MBSETTINGS &settings) const;

protected:
    struct
    {
        QLineEdit        *lnName            ;
        QComboBox        *cmbType           ;
        QComboBox        *cmbSerialPortName ;
        QComboBox        *cmbBaudRate       ;
        QComboBox        *cmbDataBits       ;
        QComboBox        *cmbParity         ;
        QComboBox        *cmbStopBits       ;
        QComboBox        *cmbFlowControl    ;
        QSpinBox         *spTimeoutFB       ;
        QSpinBox         *spTimeoutIB       ;
        QSpinBox         *spPort            ;
        QSpinBox         *spTimeout         ;
        QStackedWidget   *stackedWidget     ;
        QWidget          *pgTCP             ;
        QWidget          *pgSerial          ;
        QDialogButtonBox *buttonBox         ;
    } m_ui;
};

#endif // CORE_DIALOGPORT_H
