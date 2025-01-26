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

#include "core_dialogedit.h"

class QComboBox;
class QLineEdit;
class QSpinBox;
class QToolButton;
class QTabWidget;
class QDialogButtonBox;

class mbCoreDevice;

class MB_EXPORT mbCoreDialogDataViewItem : public mbCoreDialogEdit
{
    Q_OBJECT
public:
    struct MB_EXPORT Strings : public mbCoreDialogEdit::Strings
    {
        const QString title;
        const QString count;
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    mbCoreDialogDataViewItem(QWidget *parent = nullptr);

protected:
    void initializeBaseUi();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

protected:
    void fillForm(const MBSETTINGS &settings);
    void fillFormFormat(const QVariant &v);
    void fillFormByteOrder(const QVariant &v, mbCoreDevice *dev = nullptr);
    void fillFormRegisterOrder(const QVariant &v, mbCoreDevice *dev = nullptr);
    void fillFormByteArrayFormat(const QVariant &v, mbCoreDevice *dev = nullptr);
    void fillFormByteArraySeparator(const QVariant &v, mbCoreDevice *dev = nullptr);
    void fillFormStringLengthType(const QVariant &v, mbCoreDevice *dev = nullptr);
    void fillFormStringEncoding(const QVariant &v, mbCoreDevice *dev = nullptr);

    void fillData(MBSETTINGS &settings) const;
    void fillDataFormat(MBSETTINGS &settings, const QString &key) const;
    void fillDataByteOrder(MBSETTINGS &settings, const QString &key) const;
    void fillDataRegisterOrder(MBSETTINGS &settings, const QString &key) const;
    void fillDataByteArrayFormat(MBSETTINGS &settings, const QString &key) const;
    void fillDataByteArraySeparator(MBSETTINGS &settings, const QString &key) const;
    void fillDataStringLengthType(MBSETTINGS &settings, const QString &key) const;
    void fillDataStringEncoding(MBSETTINGS &settings, const QString &key) const;

protected:
    virtual void fillFormEditInner(const MBSETTINGS &settings);
    virtual void fillFormNewInner(const MBSETTINGS &settings);
    virtual void fillDataInner(MBSETTINGS &settings) const;

protected:
    mb::Address modbusAddress() const;
    void setModbusAddress(const QVariant &v);

protected Q_SLOTS:
    void setModbusAddresNotation(mb::AddressNotation notation);
    void deviceChanged(int i);
    void adrTypeChanged(int i);
    void formatChanged(int i);
    void lengthChanged(int i);
    void togleDefaultByteArraySeparator();

protected:
    void setVariableLength(int len);
    bool isDefaultByteArraySeparator() const;
    inline QString nonDefaultByteArraySeparator() const { return m_nonDefaultByteArraySeparator; }
    void setNonDefaultByteArraySeparator(const QString &s) const;

protected:
    mb::Format m_formatLast;
    int m_variableLength;
    mutable QString m_nonDefaultByteArraySeparator;

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
