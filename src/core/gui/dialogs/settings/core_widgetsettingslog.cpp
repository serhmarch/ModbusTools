#include "core_widgetsettingslog.h"
#include "ui_core_widgetsettingslog.h"

mbCoreWidgetSettingsLog::mbCoreWidgetSettingsLog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::mbCoreWidgetSettingsLog)
{
    ui->setupUi(this);
}

mbCoreWidgetSettingsLog::~mbCoreWidgetSettingsLog()
{
    delete ui;
}

mb::LogFlags mbCoreWidgetSettingsLog::logFlags() const
{
    mb::LogFlags flags = mb::LogFlags();
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogError  ->isChecked() * mb::Log_Error   ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogWarning->isChecked() * mb::Log_Warning ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogInfo   ->isChecked() * mb::Log_Info    ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogTx     ->isChecked() * mb::Log_Tx      ));
    flags = static_cast<mb::LogFlags>(flags | (ui->chbLogRx     ->isChecked() * mb::Log_Rx      ));
    return flags;
}

void mbCoreWidgetSettingsLog::setLogFlags(mb::LogFlags flags)
{
    ui->chbLogError  ->setChecked(flags & mb::Log_Error  );
    ui->chbLogWarning->setChecked(flags & mb::Log_Warning);
    ui->chbLogInfo   ->setChecked(flags & mb::Log_Info   );
    ui->chbLogTx     ->setChecked(flags & mb::Log_Tx     );
    ui->chbLogRx     ->setChecked(flags & mb::Log_Rx     );
}

QString mbCoreWidgetSettingsLog::formatDateTime() const
{
    return ui->lnFormat->text();
}

void mbCoreWidgetSettingsLog::setFormatDateTime(const QString &format)
{
    ui->lnFormat->setText(format);
}
