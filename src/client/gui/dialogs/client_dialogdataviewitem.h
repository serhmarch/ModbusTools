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
    struct Strings : public mbCoreDialogDataViewItem::Strings
    {
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbClientDialogDataViewItem(QWidget *parent = nullptr);
    ~mbClientDialogDataViewItem();

public:
    MBSETTINGS cachedSettings() const override;
    void setCachedSettings(const MBSETTINGS &settings) override;

private:
    void fillFormEditInner(const MBSETTINGS &settings) override;
    void fillFormNewInner(const MBSETTINGS &settings) override;
    void fillDataInner(MBSETTINGS &settings) const override;

private:
    Ui::mbClientDialogDataViewItem *ui;
};

#endif // CLIENT_DIALOGDATAVIEWITEM_H
