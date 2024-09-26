#include "core_dialogbase.h"

mbCoreDialogBase::Strings::Strings() :
    wGeometry(QStringLiteral("geometry"))
{
}

const mbCoreDialogBase::Strings &mbCoreDialogBase::Strings::instance()
{
    static const Strings s;
    return s;
}


mbCoreDialogBase::mbCoreDialogBase(const QString &cachePrefix, QWidget *parent) : QDialog(parent),
    m_cachePrefix(cachePrefix)
{
}

MBSETTINGS mbCoreDialogBase::cachedSettings() const
{
    const Strings &ds = Strings::instance();
    const QString &prefix = m_cachePrefix;

    MBSETTINGS m;
    QRect r = this->geometry();
    if (!r.topLeft().isNull())
        m[prefix+ds.wGeometry] = this->saveGeometry();
    return m;
}

void mbCoreDialogBase::setCachedSettings(const MBSETTINGS &m)
{
    const Strings &ds = Strings::instance();
    const QString &prefix = m_cachePrefix;

    MBSETTINGS::const_iterator it;
    MBSETTINGS::const_iterator end = m.end();

    it = m.find(prefix+ds.wGeometry); if (it != end) this->restoreGeometry(it.value().toByteArray());
}
