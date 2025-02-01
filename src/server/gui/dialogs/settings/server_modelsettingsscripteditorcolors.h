#ifndef MBSERVERMODELSETTINGSSCRIPTEDITORCOLORS_H
#define MBSERVERMODELSETTINGSSCRIPTEDITORCOLORS_H

#include <QAbstractTableModel>

#include <gui/script/editor/server_scripthighlighter.h>

class mbServerModelSettingsScriptEditorColors : public QAbstractTableModel
{
    Q_OBJECT

public:
    struct ColorFormat
    {
        ColorFormat(mbServerScriptHighlighter::ColorFormatType ftype) :
            type(ftype),
            name(mbServerScriptHighlighter::toString(ftype))
        {}

        const mbServerScriptHighlighter::ColorFormatType type;
        const QString name;
        QColor color;
    };

    typedef QList<ColorFormat> ColorFormats;

public:
    explicit mbServerModelSettingsScriptEditorColors(QObject *parent = nullptr);

public: // QAbstractItemModel interface
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

public:
    mbServerScriptHighlighter::ColorFormats colorFormats() const;
    void setColorFormats(const mbServerScriptHighlighter::ColorFormats &f);

public Q_SLOTS:
    void setDefaultEditorColors();

private:
    ColorFormats m_colorFormats;
};

#endif // MBSERVERMODELSETTINGSSCRIPTEDITORCOLORS_H
