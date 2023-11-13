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
#ifndef SERVER_DEVICEUI_H
#define SERVER_DEVICEUI_H

#include <QWidget>

#include <mbcore.h>

namespace Ui {
class mbServerDeviceUi;
}

class mbServerDevice;
class mbServerDeviceUiModel_0x;
class mbServerDeviceUiModel_1x;
class mbServerDeviceUiModel_3x;
class mbServerDeviceUiModel_4x;

class mbServerDeviceUi : public QWidget
{
    Q_OBJECT
public:
    explicit mbServerDeviceUi(mbServerDevice *device, QWidget *parent = nullptr);
    ~mbServerDeviceUi();

public:
    QString name() const;
    inline mbServerDevice *device() const { return m_device; }

public:
    Modbus::MemoryType currentMemoryType() const;
    QByteArray getData(Modbus::MemoryType memoryType) const;
    void setData(Modbus::MemoryType memoryType, const QByteArray &data);

Q_SIGNALS:
    void nameChanged(const QString &name);

public Q_SLOTS:
    void slotMemoryZerro   ();
    void slotMemoryZerroAll();

private Q_SLOTS:
    void deviceChanged();
    void tabChanged(int);

protected:
    bool event(QEvent *event) override;

private:
    inline bool isScanning() const { return m_timerId > 0; }
    void startScanning(int period);
    void stopScanning();
    void refreshData();

private:
    Ui::mbServerDeviceUi *ui;

    mbServerDevice *m_device;
    mbServerDeviceUiModel_0x *m_model_0x;
    mbServerDeviceUiModel_1x *m_model_1x;
    mbServerDeviceUiModel_3x *m_model_3x;
    mbServerDeviceUiModel_4x *m_model_4x;
    int m_timerId;
};

#endif // SERVER_DEVICEUI_H
