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
#ifndef CORE_DIALOGDATAVIEWITEM_H
#define CORE_DIALOGDATAVIEWITEM_H

#include "core_dialogsettings.h"

class QComboBox;
class QLineEdit;
class QSpinBox;
class QToolButton;
class QTabWidget;
class QDialogButtonBox;

class mbCoreDevice;

class MB_EXPORT mbCoreDialogDataViewItem : public mbCoreDialogSettings
{
    Q_OBJECT
public:
    struct MB_EXPORT Strings
    {
        const QString title;
        const QString count;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogDataViewItem(QWidget *parent = nullptr);

protected:
    void initializeBaseUi();

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

protected:
    virtual void fillFormInner(const MBSETTINGS &settings);
    virtual void fillDataInner(MBSETTINGS &settings);

protected:
    void fillForm(const MBSETTINGS &settings);
    void fillFormByteOrder(mb::DataOrder e);
    void fillFormRegisterOrder(mb::DataOrder e, mbCoreDevice *dev = nullptr);
    void fillFormByteArrayFormat(mb::DigitalFormat e, mbCoreDevice *dev = nullptr);
    void fillFormByteArraySeparator(const QString &s, mbCoreDevice *dev = nullptr);
    void fillFormStringLengthType(mb::StringLengthType e, mbCoreDevice *dev = nullptr);
    void fillFormStringEncoding(const QString &s, mbCoreDevice *dev = nullptr);

    void fillData(MBSETTINGS &settings);
    void fillDataByteOrder(MBSETTINGS &settings);
    void fillDataRegisterOrder(MBSETTINGS &settings);
    void fillDataByteArrayFormat(MBSETTINGS &settings);
    void fillDataByteArraySeparator(MBSETTINGS &settings);
    void fillDataStringLengthType(MBSETTINGS &settings);
    void fillDataStringEncoding(MBSETTINGS &settings);

protected Q_SLOTS:
    void deviceChanged(int i);
    void adrTypeChanged(int i);
    void formatChanged(int i);
    void lengthChanged(int i);
    void togleDefaultByteArraySeparator();

protected:
    void setVariableLength(int len);
    bool isDefaultByteArraySeparator();
    inline QString nonDefaultByteArraySeparator() const { return m_nonDefaultByteArraySeparator; }
    void setNonDefaultByteArraySeparator(const QString &s);

protected:
    mb::Format m_formatLast;
    int m_variableLength;
    QString m_nonDefaultByteArraySeparator;

    struct
    {
        QComboBox        *cmbDevice                        ;
        QComboBox        *cmbAdrType                       ;
        QSpinBox         *spOffset                         ;
        QSpinBox         *spCount                          ;
        QComboBox        *cmbFormat                        ;
        QSpinBox         *spLength                         ;
        QComboBox        *cmbByteOrder                     ;
        QComboBox        *cmbRegisterOrder                 ;
        QComboBox        *cmbByteArrayFormat               ;
        QLineEdit        *lnByteArraySeparator             ;
        QToolButton      *btnTogleDefaultByteArraySeparator;
        QComboBox        *cmbStringLengthType              ;
        QComboBox        *cmbStringEncoding                ;
        QTabWidget       *tabWidget                        ;
        QDialogButtonBox *buttonBox                        ;
    } m_ui;
};

#endif // CORE_DIALOGDATAVIEWITEM_H
