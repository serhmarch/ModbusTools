#ifndef SERVER_WIDGETSETTINGSSCRIPT_H
#define SERVER_WIDGETSETTINGSSCRIPT_H

#include <QWidget>

class QItemSelection;
class QStringListModel;

namespace Ui {
class mbServerWidgetSettingsScript;
}

class mbServerModelSettingsScriptEditorColors;
class mbServerModelSettingsScriptInterpreters;

class mbServerWidgetSettingsScript : public QWidget
{
    Q_OBJECT

public:
    explicit mbServerWidgetSettingsScript(QWidget *parent = nullptr);
    ~mbServerWidgetSettingsScript();

public:
    bool scriptEnable() const;
    void setScriptEnable(bool enable);

    bool scriptUseOptimization() const;
    void setScriptUseOptimization(bool use);

    int scriptLoopPeriod() const;
    void setScriptLoopPeriod(int period);

    bool scriptGenerateComment() const;
    void setScriptGenerateComment(bool gen);

    bool scriptWordWrap() const;
    void setScriptWordWrap(bool wrap);

    bool scriptUseLineNumbers() const;
    void setScriptUseLineNumbers(bool use);

    int scriptTabSpaces() const;
    void setScriptTabSpaces(int spaces);

    QString scriptEditorFont() const;
    void setScriptEditorFont(const QString &font);

    QString scriptEditorColorFormars() const;
    void scriptSetEditorColorFormars(const QString &formats);

    QString scriptOutputFont() const;
    void setScriptOutputFont(const QString &font);

    QStringList scriptManualExecutables() const;
    void scriptSetManualExecutables(const QStringList &exec);

    QString scriptDefaultExecutable() const;
    void scriptSetDefaultExecutable(const QString &exec);

    QStringList scriptImportPath() const;
    void scriptSetImportPath(const QStringList &path);

private:
    QFont getScriptEditorFont() const;
    void setScriptEditorFont(const QFont &f);
    QFont getScriptOutputFont() const;
    void setScriptOutputFont(const QFont &f);

private Q_SLOTS:
    void slotFont         ();
    void slotOutputFont   ();
    void slotPyAdd        ();
    void slotPySet        ();
    void slotPyRemove     ();
    void slotPyMakeDefault();
    void slotPyClear      ();
    void slotPyBrowse     ();
    void slotImportAdd    ();
    void slotImportBrowse ();
    void slotImportRemove ();
    void slotImportClear  ();
    void slotImportUp     ();
    void slotImportDown   ();
    void selectionChanged (const QItemSelection &selected, const QItemSelection &deselected);

private:
    Ui::mbServerWidgetSettingsScript *ui;
    mbServerModelSettingsScriptEditorColors *m_modelEditorColors;
    mbServerModelSettingsScriptInterpreters *m_modelInterpreters;
    QStringListModel *m_modelImportPath;
};

#endif // SERVER_WIDGETSETTINGSSCRIPT_H
