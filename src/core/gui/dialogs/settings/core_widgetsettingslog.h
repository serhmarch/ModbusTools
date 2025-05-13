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

    bool useTimestamp() const;
    void setUseTimestamp(bool use);

    QString formatDateTime() const;
    void setFormatDateTime(const QString &format);

    QString logViewFont() const;
    void setLogViewFont(const QString &font);

protected:
    QFont getLogViewFont() const;
    void setLogViewFont(const QFont &f);

private Q_SLOTS:
    void slotFont();

private:
    Ui::mbCoreWidgetSettingsLog *ui;
};

#endif // CORE_WIDGETSETTINGSLOG_H
