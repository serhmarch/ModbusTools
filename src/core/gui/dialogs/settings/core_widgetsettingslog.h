#ifndef CORE_WIDGETSETTINGSLOG_H
#define CORE_WIDGETSETTINGSLOG_H

#include <QWidget>

#include <mbcore.h>

namespace Ui {
class mbCoreWidgetSettingsLog;
}

class mbCoreWidgetSettingsLog : public QWidget
{
    Q_OBJECT

public:
    explicit mbCoreWidgetSettingsLog(QWidget *parent = nullptr);
    ~mbCoreWidgetSettingsLog();

public: // properties
    mb::LogFlags logFlags() const;
    void setLogFlags(mb::LogFlags flags);

    QString formatDateTime() const;
    void setFormatDateTime(const QString &format);

private:
    Ui::mbCoreWidgetSettingsLog *ui;
};

#endif // CORE_WIDGETSETTINGSLOG_H
