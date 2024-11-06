#ifndef CORE_DIALOGPROJECTINFO_H
#define CORE_DIALOGPROJECTINFO_H

#include "core_dialogbase.h"

namespace Ui {
class mbCoreDialogProjectInfo;
}

class mbCoreProject;

class MB_EXPORT mbCoreDialogProjectInfo : public mbCoreDialogBase
{
    Q_OBJECT

public:
    struct MB_EXPORT Strings : public mbCoreDialogBase::Strings
    {
        const QString cachePrefix;
        Strings();
        static const Strings &instance();
    };

public:
    explicit mbCoreDialogProjectInfo(QWidget *parent = nullptr);
    ~mbCoreDialogProjectInfo();

public:
    void setProjectType(const QString &type);

public:
    void showProjectInfo(mbCoreProject *project);

protected:
    void fillProjectInfo(mbCoreProject *project);

private:
    Ui::mbCoreDialogProjectInfo *ui;
};

#endif // CORE_DIALOGPROJECTINFO_H
