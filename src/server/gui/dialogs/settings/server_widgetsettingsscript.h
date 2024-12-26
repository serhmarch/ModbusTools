#ifndef SERVER_WIDGETSETTINGSSCRIPT_H
#define SERVER_WIDGETSETTINGSSCRIPT_H

#include <QWidget>

class QItemSelection;

namespace Ui {
class mbServerWidgetSettingsScript;
}

class mbServerModelSettingsScript;

class mbServerWidgetSettingsScript : public QWidget
{
    Q_OBJECT

public:
    explicit mbServerWidgetSettingsScript(QWidget *parent = nullptr);
    ~mbServerWidgetSettingsScript();

public:
    bool scriptEnable() const;
    void setScriptEnable(bool enable);

    bool scriptGenerateComment() const;
    void setScriptGenerateComment(bool gen);

    QStringList scriptManualExecutables() const;
    void scriptSetManualExecutables(const QStringList &exec);

    QString scriptDefaultExecutable() const;
    void scriptSetDefaultExecutable(const QString exec);

private Q_SLOTS:
    void slotPyAdd        ();
    void slotPySet        ();
    void slotPyRemove     ();
    void slotPyMakeDefault();
    void slotPyClear      ();
    void slotPyBrowse     ();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::mbServerWidgetSettingsScript *ui;
    mbServerModelSettingsScript *m_model;
};

#endif // SERVER_WIDGETSETTINGSSCRIPT_H
