#include "core_dialogsettings.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <QListWidget>
#include <QStackedWidget>
#include <QDialogButtonBox>

#include <core.h>
#include <gui/core_ui.h>
#include <project/core_dataview.h>

#include "core_widgetsettingsview.h"
#include "core_widgetsettingsdataview.h"
#include "core_widgetsettingslog.h"

mbCoreDialogSettings::Strings::Strings() :
    title(QStringLiteral("Settings")),
    cachePrefix(QStringLiteral("Ui.Dialogs.SystemSettings."))
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
    m_splitter = new QSplitter(this);
    m_splitter->setObjectName(QString::fromUtf8("splitter"));
    m_splitter->setOrientation(Qt::Horizontal);

    m_listWidget = new QListWidget(m_splitter);
    m_listWidget->setObjectName(QString::fromUtf8("listWidget"));
    m_splitter->addWidget(m_listWidget);
    m_splitter->setStretchFactor(0, 0);

    m_stackedWidget = new QStackedWidget(m_splitter);
    m_stackedWidget->setObjectName(QString::fromUtf8("stackedWidget"));
    m_splitter->addWidget(m_stackedWidget);
    m_splitter->setStretchFactor(1, 1);

    m_splitter->setSizes({50, 1});
    m_splitter->setChildrenCollapsible(false);

    connect(m_listWidget, &QListWidget::currentRowChanged, m_stackedWidget, &QStackedWidget::setCurrentIndex);

    m_listWidget->addItem(QStringLiteral("View"));
    m_view = new mbCoreWidgetSettingsView(m_stackedWidget);
    m_stackedWidget->addWidget(m_view);

    m_listWidget->addItem(QStringLiteral("DataView"));
    m_dataView = new mbCoreWidgetSettingsDataView(m_stackedWidget);
    m_stackedWidget->addWidget(m_dataView);

    m_listWidget->addItem(QStringLiteral("Log" ));
    m_log = new mbCoreWidgetSettingsLog (m_stackedWidget);
    m_stackedWidget->addWidget(m_log );

    QDialogButtonBox *box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(box, SIGNAL(accepted()), this, SLOT(accept()));
    connect(box, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(m_splitter, 1);
    layout->addWidget(box       , 0);
    this->setLayout(layout);

}

bool mbCoreDialogSettings::editSettings(const QString &title)
{
    MBSETTINGS m = mbCore::globalCore()->cachedSettings();
    if (title.isEmpty())
        setWindowTitle(Strings::instance().title);
    else
        setWindowTitle(title);
    fillForm(m);
    int r = QDialog::exec();
    switch (r)
    {
    case QDialog::Accepted:
        fillData(m);
        mbCore::globalCore()->setCachedSettings(m);
        return true;
    }
    return false;
}

void mbCoreDialogSettings::fillForm(const MBSETTINGS &m)
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    m_view->setUseNameWithSettings(m.value(sUi.settings_useNameWithSettings).toBool());


    m_log->setLogFlags(static_cast<mb::LogFlag>(m.value(sCore.settings_logFlags).toInt()));
    m_log->setFormatDateTime(m.value(sCore.settings_formatDateTime).toString());
    m_dataView->setAddressNotation(mb::toAddressNotation(m.value(sCore.settings_addressNotation)));
}

void mbCoreDialogSettings::fillData(MBSETTINGS &m)
{
    const mbCore::Strings &sCore = mbCore::Strings::instance();
    const mbCoreUi::Strings &sUi = mbCoreUi::Strings::instance();

    m[sUi.settings_useNameWithSettings] = m_view->useNameWithSettings();

    m[sCore.settings_logFlags       ] = static_cast<int>(m_log->logFlags());
    m[sCore.settings_formatDateTime ] = m_log->formatDateTime();
    m[sCore.settings_addressNotation] = m_dataView->addressNotation();

}
