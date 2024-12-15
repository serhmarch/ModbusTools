#ifndef CORE_WIDGETSETTINGSVIEW_H
#define CORE_WIDGETSETTINGSVIEW_H

#include <QWidget>

namespace Ui {
class mbCoreWidgetSettingsView;
}

class mbCoreWidgetSettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit mbCoreWidgetSettingsView(QWidget *parent = nullptr);
    ~mbCoreWidgetSettingsView();

private:
    Ui::mbCoreWidgetSettingsView *ui;
};

#endif // CORE_WIDGETSETTINGSVIEW_H
