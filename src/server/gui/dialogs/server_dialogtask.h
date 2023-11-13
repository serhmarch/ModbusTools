#ifndef SERVER_DIALOGTASK_H
#define SERVER_DIALOGTASK_H

#include <QDialog>

namespace Ui {
class mbServerDialogTask;
}

class mbServer;

class mbServerDialogTask : public QDialog
{
    Q_OBJECT

public:
    struct Strings
    {
        const QString title;
        Strings();
        static const Strings &instance();
    };

    struct Data
    {
        QString name;
        QString type;
    };

public:
    explicit mbServerDialogTask(mbServer *core, QWidget *parent = nullptr);
    ~mbServerDialogTask();

public:
    bool getData(Data &data, const QString &title = QString());

private:
    Ui::mbServerDialogTask *ui;
    mbServer* m_core;
};

#endif // SERVER_DIALOGTASK_H
