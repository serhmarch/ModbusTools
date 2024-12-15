#include "core_dialogsettings.h"

#include <QHBoxLayout>
#include <QListWidget>
#include <QStackedWidget>

mbCoreDialogSettings::Strings::Strings() :
    title(QStringLiteral("Settings")),
    cachePrefix(QStringLiteral("Ui.Dialogs.Settings."))
{
}

const mbCoreDialogSettings::Strings &mbCoreDialogSettings::Strings::instance()
{
    static const Strings s;
    return s;
}

mbCoreDialogSettings::mbCoreDialogSettings(QWidget *parent) :
    mbCoreDialogBase(Strings::instance().cachePrefix, parent)
{
    m_listWidget    = new QListWidget   (this);
    m_stackedWidget = new QStackedWidget(this);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(m_listWidget   );
    layout->addWidget(m_stackedWidget);
    this->setLayout(layout);
}

bool mbCoreDialogSettings::editSettings(const QString &title)
{
    return false;
}
