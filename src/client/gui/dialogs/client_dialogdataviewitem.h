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
#ifndef CLIENT_DIALOGDATAVIEWITEM_H
#define CLIENT_DIALOGDATAVIEWITEM_H

#include <gui/dialogs/core_dialogdataviewitem.h>

class mbClientDevice;

namespace Ui {
class mbClientDialogDataViewItem;
}

class mbClientDialogDataViewItem : public mbCoreDialogDataViewItem
{
    Q_OBJECT
public:
    explicit mbClientDialogDataViewItem(QWidget *parent = nullptr);
    ~mbClientDialogDataViewItem();

public:
    MBSETTINGS getSettings(const MBSETTINGS &settings = MBSETTINGS(), const QString &title = QString()) override;

private:
    void fillForm(const MBSETTINGS &settings);
    void fillFormByteOrder(mb::DataOrder e);
    void fillFormRegisterOrder(mb::DataOrder e, mbClientDevice *dev = nullptr);
    void fillFormByteArrayFormat(mb::DigitalFormat e, mbClientDevice *dev = nullptr);
    void fillFormByteArraySeparator(const QString &s, mbClientDevice *dev = nullptr);
    void fillFormStringLengthType(mb::StringLengthType e, mbClientDevice *dev = nullptr);
    void fillFormStringEncoding(mb::StringEncoding e, mbClientDevice *dev = nullptr);

    void fillData(MBSETTINGS &settings);
    void fillDataByteOrder(MBSETTINGS &settings);
    void fillDataRegisterOrder(MBSETTINGS &settings);
    void fillDataByteArrayFormat(MBSETTINGS &settings);
    void fillDataByteArraySeparator(MBSETTINGS &settings);
    void fillDataStringLengthType(MBSETTINGS &settings);
    void fillDataStringEncoding(MBSETTINGS &settings);

private Q_SLOTS:
    void deviceChanged(int i);
    void adrTypeChanged(int i);
    void formatChanged(int i);
    void lengthChanged(int i);
    void togleDefaultByteArraySeparator();

private:
    void setVariableLength(int len);
    bool isDefaultByteArraySeparator();
    inline QString nonDefaultByteArraySeparator() const { return m_nonDefaultByteArraySeparator; }
    void setNonDefaultByteArraySeparator(const QString &s);

private:
    Ui::mbClientDialogDataViewItem *ui;

    mb::Format m_formatLast;
    int m_variableLength;
    QString m_nonDefaultByteArraySeparator;
};

#endif // CLIENT_DIALOGDATAVIEWITEM_H
