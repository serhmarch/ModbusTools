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
#ifndef CORE_DIALOGDEVICE_H
#define CORE_DIALOGDEVICE_H

#include "core_dialogsettings.h"

class QLineEdit;
class QSpinBox;
class QComboBox;
class QDialogButtonBox;

class MB_EXPORT mbCoreDialogDevice : public mbCoreDialogSettings
{
public:
    struct MB_EXPORT Strings
    {
        const QString title;
        const QString settings_prefix;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogDevice(QWidget *parent = nullptr);

protected:
    void initializeBaseUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &m) override;

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

protected:
    virtual void fillForm(const MBSETTINGS& params);
    virtual void fillData(MBSETTINGS& params) const;

protected:
    struct
    {
        QLineEdit        *lnName                     ;
        QSpinBox         *spMaxReadCoils             ;
        QSpinBox         *spMaxReadDiscreteInputs    ;
        QSpinBox         *spMaxReadHoldingRegisters  ;
        QSpinBox         *spMaxReadInputRegisters    ;
        QSpinBox         *spMaxWriteMultipleCoils    ;
        QSpinBox         *spMaxWriteMultipleRegisters;
        QComboBox        *cmbRegisterOrder           ;
        QComboBox        *cmbByteArrayFormat         ;
        QLineEdit        *lnByteArraySeparator       ;
        QComboBox        *cmbStringLengthType        ;
        QComboBox        *cmbStringEncoding          ;
        QDialogButtonBox *buttonBox                  ;
    } m_ui;
};

#endif // CORE_DIALOGDEVICE_H
