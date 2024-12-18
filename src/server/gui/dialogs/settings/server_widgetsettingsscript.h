#ifndef SERVER_WIDGETSETTINGSSCRIPT_H
#define SERVER_WIDGETSETTINGSSCRIPT_H

#include <QWidget>

namespace Ui {
class mbServerWidgetSettingsScript;
}

class mbServerWidgetSettingsScript : public QWidget
{
    Q_OBJECT

public:
    explicit mbServerWidgetSettingsScript(QWidget *parent = nullptr);
    ~mbServerWidgetSettingsScript();

private:
    Ui::mbServerWidgetSettingsScript *ui;
};

#endif // SERVER_WIDGETSETTINGSSCRIPT_H
