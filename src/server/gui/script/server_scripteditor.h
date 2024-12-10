#ifndef SERVER_SCRIPTEDITOR_H
#define SERVER_SCRIPTEDITOR_H

#include <QPlainTextEdit>

class mbServerScriptEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    mbServerScriptEditor(QWidget *parent = nullptr);

public:
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

protected:
    void resizeEvent(QResizeEvent *event) override;

protected Q_SLOTS:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &rect, int dy);

protected:
    void insertFromMimeData(const QMimeData *source) override;

private:
    QWidget *lineNumberArea;

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
