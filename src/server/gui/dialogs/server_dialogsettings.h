#ifndef SERVER_DIALOGSETTINGS_H
#define SERVER_DIALOGSETTINGS_H

#include <QDialog>

class mbServer;

namespace Ui {
class mbServerDialogSettings;
}

class mbServerDialogSettings : public QDialog
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString title;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbServerDialogSettings(QWidget *parent = nullptr);
    ~mbServerDialogSettings();

public:
    bool editSystemSettings(const QString& title = QString());

private:
    mbServer* m_core;
    Ui::mbServerDialogSettings *ui;
};

#endif // SERVER_DIALOGSETTINGS_H
