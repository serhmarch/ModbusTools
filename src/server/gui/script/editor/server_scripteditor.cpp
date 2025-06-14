#include "server_scripteditor.h"

#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QMimeData>

#include <server_global.h>
#include "server_scripthighlighter.h"

// https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html

mbServerScriptEditor::Defaults::Defaults() :
    settings(Settings(false,
                      true ,
                      4    ,
                      QFont("Courier New", 10).toString(),
                      mbServerScriptHighlighter::Defaults::instance().colorFormats))
{
}

const mbServerScriptEditor::Defaults &mbServerScriptEditor::Defaults::instance()
{
    static const mbServerScriptEditor::Defaults d;
    return d;
}

QTextDocument::FindFlags mbServerScriptEditor::toQTextDocumentFindFlags(int findFlags)
{
    QTextDocument::FindFlags tFindFlags;
    if (findFlags & mb::FindBackward       ) tFindFlags |= QTextDocument::FindBackward       ;
    if (findFlags & mb::FindCaseSensitively) tFindFlags |= QTextDocument::FindCaseSensitively;
    if (findFlags & mb::FindWholeWords     ) tFindFlags |= QTextDocument::FindWholeWords     ;
    return tFindFlags;
}

mbServerScriptEditor::mbServerScriptEditor(const mbServerScriptEditor::Settings settings, QWidget *parent) : QPlainTextEdit(parent)
{
    lineNumberArea = nullptr;
    m_highlighter = new mbServerScriptHighlighter(settings.colorFormats, this->document());
    //highlightCurrentLine();
    setSettings(settings);
}

bool mbServerScriptEditor::wordWrap() const
{
    return LineWrapMode() != NoWrap;
}

void mbServerScriptEditor::setWordWrap(bool wrap)
{
    if (wrap)
        setLineWrapMode(WidgetWidth);
    else
        setLineWrapMode(NoWrap);
}

bool mbServerScriptEditor::useLineNumbers() const
{
    return lineNumberArea != nullptr;
}

void mbServerScriptEditor::setUseLineNumbers(bool use)
{
    if (use != useLineNumbers())
    {
        if (lineNumberArea)
        {
            LineNumberArea *old = static_cast<LineNumberArea*>(lineNumberArea);
            old->disconnect(this);
            lineNumberArea = nullptr;
            delete old;
        }
        else
        {
            lineNumberArea = new LineNumberArea(this);

            connect(this, &mbServerScriptEditor::blockCountChanged    , this, &mbServerScriptEditor::updateLineNumberAreaWidth);
            connect(this, &mbServerScriptEditor::updateRequest        , this, &mbServerScriptEditor::updateLineNumberArea);
            //connect(this, &mbServerScriptEditor::cursorPositionChanged, this, &mbServerScriptEditor::highlightCurrentLine);
            connect(this, SIGNAL(cursorPositionChanged()), lineNumberArea, SLOT(repaint()));
            lineNumberArea->setVisible(true);
        }
        updateLineNumberAreaWidth(0);
    }
}

int mbServerScriptEditor::tabSpaces() const
{
    return m_settings.tabSpaces;
}

void mbServerScriptEditor::setTabSpaces(int spaces)
{
    m_settings.tabSpaces = spaces;
    setTabStopWidth(fontMetrics().horizontalAdvance(QLatin1Char('9')) * m_settings.tabSpaces);
}

QString mbServerScriptEditor::fontString() const
{
    return this->font().toString();
}

void mbServerScriptEditor::setFontString(const QString &font)
{
    QFont f = this->font();
    if (f.fromString(font))
        this->setFont(f);
}

mbServerScriptEditor::Settings mbServerScriptEditor::settings() const
{
    mbServerScriptEditor::Settings res;
    res.wordWrap = wordWrap();
    res.useLineNumbers = useLineNumbers();
    res.tabSpaces = tabSpaces();
    res.colorFormats = m_highlighter->colorFormats();
    res.font = this->fontString();
    return res;
}

void mbServerScriptEditor::setSettings(const mbServerScriptEditor::Settings &s)
{
    setWordWrap(s.wordWrap);
    setTabSpaces(s.tabSpaces);
    setFontString(s.font);
    m_highlighter->setColorFormats(s.colorFormats);
    m_highlighter->rehighlight();
    setUseLineNumbers(s.useLineNumbers);
}

int mbServerScriptEditor::lineNumberAreaWidth()
{
    if (!lineNumberArea)
        return 0;
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10)
    {
        max /= 10;
        ++digits;
    }

    int space = 13 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;

    return space;
}

bool mbServerScriptEditor::findText(const QString &text, int findFlags)
{
    QTextDocument::FindFlags tFindFlags = toQTextDocumentFindFlags(findFlags);
    return this->find(text, tFindFlags);
}

bool mbServerScriptEditor::replaceText(const QString &replacement)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return false;

    // Store selected text before replacement
    QString selectedText = cursor.selectedText();

    // Replace current selection
    cursor.insertText(replacement);

    // Move to the next occurrence
    QTextDocument *doc = document();
    QTextCursor next = doc->find(selectedText, cursor);

    if (next.isNull())
        return true;

    // Select the next found text
    setTextCursor(next);
    return true;
}

bool mbServerScriptEditor::replaceTextAll(const QString &replacement)
{
    QTextCursor cursor = textCursor();
    if (!cursor.hasSelection())
        return false;

    QString selectedText = cursor.selectedText();
    if (selectedText.isEmpty())
        return false;

    moveCursor(QTextCursor::Start);
    bool replaced = false;

    /*
    QTextDocument *doc = document();
    QTextCursor searchCursor(doc);

    searchCursor.beginEditBlock();
    while (true)
    {
        searchCursor = doc->find(selectedText, searchCursor);
        if (searchCursor.isNull())
            break;
        searchCursor.insertText(replacement);
        replaced = true;
    }
    searchCursor.endEditBlock();
    */

    this->textCursor().beginEditBlock();
    while (this->find(selectedText))
    {
        this->textCursor().insertText(replacement);
        replaced = true;
    }
    this->textCursor().endEditBlock();

    return replaced;
}

void mbServerScriptEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void mbServerScriptEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    if (lineNumberArea)
    {
        QRect cr = contentsRect();
        lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
    }
}

void mbServerScriptEditor::keyPressEvent(QKeyEvent *event)
{
switch (event->key())
{
    case Qt::Key_Tab:
    {
        QTextCursor cursor = this->textCursor();
        if (cursor.hasSelection())
        {
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();
            cursor.setPosition(start);
            int startBlock = cursor.blockNumber();
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            int endBlock = cursor.blockNumber();

            // Begin edit block for undo/redo
            cursor.beginEditBlock();
            for (int i = startBlock; i <= endBlock; ++i)
            {
                QTextBlock block = document()->findBlockByNumber(i);
                QTextCursor blockCursor(block);
                blockCursor.movePosition(QTextCursor::StartOfBlock);
                blockCursor.insertText(QString(" ").repeated(m_settings.tabSpaces));
            }
            cursor.endEditBlock();
        }
        else
        {
            int column = cursor.columnNumber();  // Column (0-based)
            int cSymbols = m_settings.tabSpaces - column % m_settings.tabSpaces;
            QString tabSpaces = QString(" ").repeated(cSymbols);
            insertPlainText(tabSpaces); // Insert spaces instead of tab
        }
    }
        break;
    case Qt::Key_Backtab:
    {
        QTextCursor cursor = this->textCursor();
        if (cursor.hasSelection())
        {
            int start = cursor.selectionStart();
            int end = cursor.selectionEnd();
            cursor.setPosition(start);
            int startBlock = cursor.blockNumber();
            cursor.setPosition(end, QTextCursor::KeepAnchor);
            int endBlock = cursor.blockNumber();

            // Begin edit block for undo/redo
            cursor.beginEditBlock();
            for (int i = startBlock; i <= endBlock; ++i)
            {
                QTextBlock block = document()->findBlockByNumber(i);
                QTextCursor blockCursor(block);
                blockCursor.movePosition(QTextCursor::StartOfBlock);
                for (int j = 0; j < m_settings.tabSpaces; ++j)
                {
                    if (blockCursor.document()->characterAt(blockCursor.position()) == QChar(' '))
                        blockCursor.deleteChar();
                    else
                        break;
                }
            }
            cursor.endEditBlock();
        }
        else
        {
            int column = cursor.columnNumber();  // Column (0-based)
            int cSymbols = column % m_settings.tabSpaces;
            int cRemove = cSymbols + (cSymbols == 0) * m_settings.tabSpaces;
            int pos = cursor.position();
            if (cRemove >= column)
                pos -= column;
            else
                pos -= cRemove;
            cursor.setPosition(pos, QTextCursor::KeepAnchor);
            cursor.removeSelectedText();
        }
    }
        break;
    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        QTextCursor cursor = this->textCursor();
        cursor.movePosition(QTextCursor::StartOfBlock);
        QString currentLine = cursor.block().text();

        // Extract leading spaces manually
        QString leadingSpaces;
        Q_FOREACH (QChar ch, currentLine)
        {
            if (ch.isSpace())
                leadingSpaces += ch;
            else
                break;
        }
        for (int i = currentLine.length()-1; i >= 0; --i)
        {
            if (!currentLine[i].isSpace())
            {
                // TODO maybe ':' can be inside comment or multiline text
                if (currentLine[i] == ':')
                    leadingSpaces += QString(" ").repeated(m_settings.tabSpaces);
                break;
            }
        }
        cursor.movePosition(QTextCursor::EndOfBlock);

        // Insert new line with indentation
        QPlainTextEdit::keyPressEvent(event);
        cursor.insertText(leadingSpaces);
    }
        break;
    default:
        QPlainTextEdit::keyPressEvent(event);
        break;
    }
}

void mbServerScriptEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly())
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void mbServerScriptEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (!lineNumberArea)
        return;
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void mbServerScriptEditor::insertFromMimeData(const QMimeData *source)
{
    if (source->hasText())
    {
        // Preserve tabs by directly inserting plain text
        QString text = source->text();
        insertPlainText(text);
    }
}

void mbServerScriptEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(blockBoundingGeometry(block).translated(contentOffset()).top());
    int bottom = top + qRound(blockBoundingRect(block).height());
    const int currentBlockNumber = textCursor().blockNumber();
    while (block.isValid() && top <= event->rect().bottom())
    {
        if (block.isVisible() && bottom >= event->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            if (blockNumber == currentBlockNumber)
                painter.setPen(Qt::yellow);
            else
                painter.setPen(Qt::black);
            painter.drawText(-5, top, lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + qRound(blockBoundingRect(block).height());
        ++blockNumber;
    }
}

mbServerScriptEditor::LineNumberArea::LineNumberArea(mbServerScriptEditor *editor) : QWidget(editor)
{
    codeEditor = editor;
    m_selectStartLine = -1;
}

QSize mbServerScriptEditor::LineNumberArea::sizeHint() const
{
    return QSize(codeEditor->lineNumberAreaWidth(), 0);
}

void mbServerScriptEditor::LineNumberArea::paintEvent(QPaintEvent *event)
{
    codeEditor->lineNumberAreaPaintEvent(event);
}

void mbServerScriptEditor::LineNumberArea::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        // Calculate the block (line) number from the y position
        QTextBlock block = codeEditor->firstVisibleBlock();
        int blockNumber = block.blockNumber();
        int top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
        int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
        while (block.isValid() && top <= event->pos().y())
        {
            if (block.isVisible() && bottom >= event->pos().y())
            {
                m_selectStartLine = blockNumber;
                QTextCursor cursor(codeEditor->document()->findBlockByNumber(blockNumber));
                cursor.movePosition(QTextCursor::StartOfBlock);
                cursor.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
                codeEditor->setTextCursor(cursor);
                break;
            }
            block = block.next();
            top = bottom;
            bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
            ++blockNumber;
        }
    }
}

void mbServerScriptEditor::LineNumberArea::mouseMoveEvent(QMouseEvent *event)
{
    setCursor(Qt::PointingHandCursor);
    if (!(event->buttons() & Qt::LeftButton) || m_selectStartLine == -1)
        return;
    // Calculate the block (line) number from the y position
    QTextBlock block = codeEditor->firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = qRound(codeEditor->blockBoundingGeometry(block).translated(codeEditor->contentOffset()).top());
    int bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
    int currentLine = -1;
    while (block.isValid() && top <= event->pos().y())
    {
        if (block.isVisible() && bottom >= event->pos().y())
        {
            currentLine = blockNumber;
            break;
        }
        block = block.next();
        top = bottom;
        bottom = top + qRound(codeEditor->blockBoundingRect(block).height());
        ++blockNumber;
    }
    if (currentLine != -1)
    {
        int start = qMin(m_selectStartLine, currentLine);
        int end = qMax(m_selectStartLine, currentLine);
        QTextCursor cursor(codeEditor->document()->findBlockByNumber(start));
        cursor.movePosition(QTextCursor::StartOfBlock);
        cursor.setPosition(codeEditor->document()->findBlockByNumber(end).position() + codeEditor->document()->findBlockByNumber(end).length() - 1, QTextCursor::KeepAnchor);
        codeEditor->setTextCursor(cursor);
    }
}

void mbServerScriptEditor::LineNumberArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) 
        m_selectStartLine = -1;
}

void mbServerScriptEditor::LineNumberArea::enterEvent(QEvent *event)
{
    Q_UNUSED(event);
    setCursor(Qt::PointingHandCursor); // Change to row selection cursor
}

void mbServerScriptEditor::LineNumberArea::leaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    unsetCursor(); // Restore default cursor
}
