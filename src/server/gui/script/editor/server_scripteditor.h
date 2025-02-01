#ifndef SERVER_SCRIPTEDITOR_H
#define SERVER_SCRIPTEDITOR_H

#include <QPlainTextEdit>

#include "server_scripthighlighter.h"

class mbServerScriptHighlighter;

class mbServerScriptEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    mbServerScriptEditor(const mbServerScriptHighlighter::ColorFormats formats, QWidget *parent = nullptr);

public:
    mbServerScriptHighlighter::ColorFormats colorFormats() const;
    void setColorFormats(const mbServerScriptHighlighter::ColorFormats &f);

public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

protected Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

protected:
    void insertFromMimeData(const QMimeData *source) override;

private:
    QWidget *lineNumberArea;
    mbServerScriptHighlighter *m_highlighter;

    class LineNumberArea : public QWidget
    {
    public:
        LineNumberArea(mbServerScriptEditor *editor);
        QSize sizeHint() const;

    protected:
        void paintEvent(QPaintEvent *event);

    private:
        mbServerScriptEditor *codeEditor;
    };

};

#endif // SERVER_SCRIPTEDITOR_H
