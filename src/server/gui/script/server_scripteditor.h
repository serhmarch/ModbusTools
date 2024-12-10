#ifndef SERVER_SCRIPTEDITOR_H
#define SERVER_SCRIPTEDITOR_H

#include <QTextEdit>

class LineNumberArea : public QWidget
{
    Q_OBJECT

public:
    LineNumberArea(QTextEdit *editor);

    QSize sizeHint() const;

protected:
    void paintEvent(QPaintEvent *event);

private:
    QTextEdit *codeEditor;
};

class mbServerScriptEditor : public QTextEdit
{
    Q_OBJECT
public:
    mbServerScriptEditor(QWidget *parent = nullptr);
    int getFirstVisibleBlockId();
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

public Q_SLOTS:

    void resizeEvent(QResizeEvent *e);

protected Q_SLOTS:

    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(QRectF /*rect_f*/);
    void updateLineNumberArea(int /*slider_pos*/);
    void updateLineNumberArea();

protected:
    void insertFromMimeData(const QMimeData *source) override;

private:
    QWidget *lineNumberArea;
};

#endif // SERVER_SCRIPTEDITOR_H
