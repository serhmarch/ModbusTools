#include "client_portstatisticsui.h"
#include "ui_client_portstatisticsui.h"

#include <project/client_port.h>

mbClientPortStatisticsUi::mbClientPortStatisticsUi(mbClientPort *port, QWidget *parent) :
    mbCorePortStatisticsUi(port, parent),
    ui(new Ui::mbClientPortStatisticsUi)
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
    m_ui.lnCountBadTimeout      = ui->lnCountBadTimeout     ;
    m_ui.lnCountBadCRC          = ui->lnCountBadCRC         ;
    m_ui.lnCycleCount           = ui->lnCycleCount          ;
    m_ui.lnCycleSumDuration     = ui->lnCycleSumDuration    ;
    m_ui.lnCycleLastDuration    = ui->lnCycleLastDuration   ;
    m_ui.lnCycleMinDuration     = ui->lnCycleMinDuration    ;
    m_ui.lnCycleMaxDuration     = ui->lnCycleMaxDuration    ;
    m_ui.lnCycleAvgDuration     = ui->lnCycleAvgDuration    ;

    connect(ui->btnReset, &QPushButton::clicked, this, &mbClientPortStatisticsUi::resetStatistics);
}

mbClientPortStatisticsUi::~mbClientPortStatisticsUi()
{
    delete ui;
}

void mbClientPortStatisticsUi::syncStatistics()
{
    mbCorePortStatisticsUi::syncStatistics();
    auto s = port()->statistics();

    ui->lnTimeResponseLast ->setText(QString::number(s.timeResponseLast));
    ui->lnTimeResponseMin  ->setText(QString::number(s.timeResponseMin ));
    ui->lnTimeResponseMax  ->setText(QString::number(s.timeResponseMax ));
    ui->lnTimeResponseAvg  ->setText(QString::number(s.timeResponseAvg ));
    
    ui->lnCountBadConnection->setText(QString::number(s.countBadConnection));
}
