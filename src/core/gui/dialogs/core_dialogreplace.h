#ifndef CORE_DIALOGREPLACE_H
#define CORE_DIALOGREPLACE_H

#include <QDialog>

class QLabel;
class QPushButton;

class mbCoreDialogReplace : public QDialog
{
    Q_OBJECT

public:
    enum Button
    {
        None        = 0x00,
        Replace     = 0x01,
        Rename      = 0x02,
        Skip        = 0x04,
        ReplaceAll  = 0x08,
        RenameAll   = 0x10,
        SkipAll     = 0x20,
        Cancel      = 0x40
    };

public:
    mbCoreDialogReplace(QWidget *parent = nullptr);

public:
    QString text() const;
    void setText(const QString &text);
    void setUseAllButtons(bool use);

private Q_SLOTS:
    void slotReplaceClicked();
    void slotRenameClicked();
    void slotSkipClicked();
    void slotReplaceAllClicked();
    void slotRenameAllClicked();
    void slotSkipAllClicked();
    void slotCancelClicked();

private:
    QLabel* m_label;
    QPushButton* m_buttonReplace;
    QPushButton* m_buttonRename;
    QPushButton* m_buttonSkip;
    QPushButton* m_buttonReplaceAll;
    QPushButton* m_buttonRenameAll;
    QPushButton* m_buttonSkipAll;
    QPushButton* m_buttonCancel;

};

#endif // CORE_DIALOGREPLACE_H
