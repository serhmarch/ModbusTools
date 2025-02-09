#include "server_modelsettingsscripteditorcolors.h"

mbServerModelSettingsScriptEditorColors::mbServerModelSettingsScriptEditorColors(QObject *parent)
    : QAbstractTableModel{parent},
    m_colorFormats({
      ColorFormat(mbServerScriptHighlighter::TextFormat   ),
      ColorFormat(mbServerScriptHighlighter::KeywordFormat),
      ColorFormat(mbServerScriptHighlighter::NumberFormat ),
      ColorFormat(mbServerScriptHighlighter::CommentFormat),
      ColorFormat(mbServerScriptHighlighter::StringFormat ),
      ColorFormat(mbServerScriptHighlighter::SymbolFormat )
    })
{
}

QVariant mbServerModelSettingsScriptEditorColors::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        switch (orientation)
        {
        case Qt::Horizontal:
            switch (section)
            {
            case 0: return QStringLiteral("Color");
            }
            break;
        case Qt::Vertical:
            if (section < m_colorFormats.count())
                return m_colorFormats.at(section).name;
            break;
        }
        break;
    }
    return QVariant();
}

int mbServerModelSettingsScriptEditorColors::rowCount(const QModelIndex &parent) const
{
    return m_colorFormats.count();
}

int mbServerModelSettingsScriptEditorColors::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant mbServerModelSettingsScriptEditorColors::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::BackgroundRole:
    {
        int r = index.row();
        if (r < m_colorFormats.count())
            return m_colorFormats.at(r).color;
    }
        break;
    }
    return QVariant();
}

bool mbServerModelSettingsScriptEditorColors::setData(const QModelIndex &index, const QVariant &value, int role)
{
    switch (role)
    {
    case Qt::BackgroundRole:
    {
        int r = index.row();
        if (r < m_colorFormats.count())
        {
            QColor color = value.value<QColor>();
            m_colorFormats[r].color = color;
            return true;
        }
    }
        break;
    }
    return false;
}

mbServerScriptHighlighter::ColorFormats mbServerModelSettingsScriptEditorColors::colorFormats() const
{
    mbServerScriptHighlighter::ColorFormats res;
    Q_FOREACH (const ColorFormat &f, m_colorFormats)
        res[f.type] = f.color;
    return res;
}

void mbServerModelSettingsScriptEditorColors::setColorFormats(const mbServerScriptHighlighter::ColorFormats &fmt)
{
    mbServerScriptHighlighter::ColorFormats::const_iterator it;
    mbServerScriptHighlighter::ColorFormats::const_iterator end = fmt.end();

    for (ColorFormat &localfmt : m_colorFormats)
    {
        it = fmt.find(localfmt.type);
        if (it != end)
            localfmt.color = it.value();
    }
    Q_EMIT dataChanged(createIndex(0, 0), createIndex(m_colorFormats.count()-1, 0));
}

void mbServerModelSettingsScriptEditorColors::setDefaultEditorColors()
{
    mbServerScriptHighlighter::ColorFormats fmts = mbServerScriptHighlighter::Defaults::instance().colorFormats;
    setColorFormats(fmts);
}
