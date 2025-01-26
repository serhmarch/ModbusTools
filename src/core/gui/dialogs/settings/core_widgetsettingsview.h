#ifndef CORE_WIDGETSETTINGSVIEW_H
#define CORE_WIDGETSETTINGSVIEW_H

#include <QWidget>

#include <mbcore.h>

namespace Ui {
class mbCoreWidgetSettingsView;
}

class mbCoreWidgetSettingsView : public QWidget
{
    Q_OBJECT

public:
    explicit mbCoreWidgetSettingsView(QWidget *parent = nullptr);
    ~mbCoreWidgetSettingsView();

public: // properties
    bool useNameWithSettings() const;
    void setUseNameWithSettings(bool use);
    mb::AddressNotation addressNotation() const;
    void setAddressNotation(mb::AddressNotation notation);

private:
    Ui::mbCoreWidgetSettingsView *ui;
};

#endif // CORE_WIDGETSETTINGSVIEW_H
