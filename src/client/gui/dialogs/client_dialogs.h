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
#ifndef CLIENT_DIALOGS_H
#define CLIENT_DIALOGS_H

#include <gui/dialogs/core_dialogs.h>

class mbClientDialogSendMessage;

class mbClientDialogs : public mbCoreDialogs
{
public:
    mbClientDialogs(QWidget* parent = nullptr);

public:
    void sendMessage();

private:
    mbClientDialogSendMessage *m_sendMessage;
};

#endif // CLIENT_DIALOGS_H
