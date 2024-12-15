#ifndef CORE_WIDGETSETTINGSLOG_H
#define CORE_WIDGETSETTINGSLOG_H

#include <QWidget>

namespace Ui {
class mbCoreWidgetSettingsLog;
}

class mbCoreWidgetSettingsLog : public QWidget
{
    Q_OBJECT

public:
    explicit mbCoreWidgetSettingsLog(QWidget *parent = nullptr);
    ~mbCoreWidgetSettingsLog();

private:
    Ui::mbCoreWidgetSettingsLog *ui;
};

#endif // CORE_WIDGETSETTINGSLOG_H
