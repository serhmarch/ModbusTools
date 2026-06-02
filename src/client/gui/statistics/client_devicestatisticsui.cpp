#include "client_devicestatisticsui.h"
#include "ui_client_devicestatisticsui.h"

#include <project/client_device.h>

mbClientDeviceStatisticsUi::mbClientDeviceStatisticsUi(mbClientDevice *device, QWidget *parent) :
    mbCoreDeviceStatisticsUi(device, parent),
    ui(new Ui::mbClientDeviceStatisticsUi)
{
    ui->setupUi(this);

    m_ui.lnSinceTimestamp       = ui->lnSinceTimestamp      ;
    m_ui.lnLastStatus           = ui->lnLastStatus          ;
    m_ui.lnLastTimestamp        = ui->lnLastTimestamp       ;
    m_ui.lnLastSuccessTimestamp = ui->lnLastSuccessTimestamp;
    m_ui.lnLastErrorStatus      = ui->lnLastErrorStatus     ;
    m_ui.lnLastErrorTimestamp   = ui->lnLastErrorTimestamp  ;
    m_ui.txtLastErrorText       = ui->txtLastErrorText      ;
    m_ui.lnCountTx              = ui->lnCountTx             ;
    m_ui.lnCountRx              = ui->lnCountRx             ;
    m_ui.lnCountGood            = ui->lnCountGood           ;
    m_ui.lnCountBad             = ui->lnCountBad            ;
    m_ui.lnCountBadStandard     = ui->lnCountBadStandard    ;

    connect(ui->btnReset, &QPushButton::clicked, this, &mbClientDeviceStatisticsUi::resetStatistics);
}

mbClientDeviceStatisticsUi::~mbClientDeviceStatisticsUi()
{
    delete ui;
}

void mbClientDeviceStatisticsUi::syncStatistics()
{
    mbCoreDeviceStatisticsUi::syncStatistics();
    auto s = device()->statistics();

    ui->lnTimeResponseLast ->setText(QString::number(s.timeResponseLast));
    ui->lnTimeResponseMin  ->setText(QString::number(s.timeResponseMin ));
    ui->lnTimeResponseMax  ->setText(QString::number(s.timeResponseMax ));
    ui->lnTimeResponseAvg  ->setText(QString::number(s.timeResponseAvg ));

    ui->lnCountBadConnection->setText(QString::number(s.countBadConnection));
    ui->lnCountBadTimeout   ->setText(QString::number(s.countBadTimeout   ));
    ui->lnCountBadCRC       ->setText(QString::number(s.countBadCRC       ));
}
