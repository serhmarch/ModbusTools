#include "server_scripteditor.h"

#include <QScrollBar>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include <QPainter>
#include <QMimeData>

LineNumberArea::LineNumberArea(QTextEdit *editor) : QWidget(editor)
{
    codeEditor = editor;
}

QSize LineNumberArea::sizeHint() const
{
    return QSize(((mbServerScriptEditor *)codeEditor)->lineNumberAreaWidth(), 0);
}

void LineNumberArea::paintEvent(QPaintEvent *event)
{
    ((mbServerScriptEditor *)codeEditor)->lineNumberAreaPaintEvent(event);
}

mbServerScriptEditor::mbServerScriptEditor(QWidget *parent) : QTextEdit(parent)
{
    setFont(QFont("Courier New", 9)); // Set a default font
    setTabStopWidth(fontMetrics().horizontalAdvance(' ') * 4);
    // Line numbers
    lineNumberArea = new LineNumberArea(this);
    ///
    connect(this->document(), SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(updateLineNumberArea/*_2*/(int)));
    connect(this, SIGNAL(textChanged()), this, SLOT(updateLineNumberArea()));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(updateLineNumberArea()));
    ///
    updateLineNumberAreaWidth(0);
}

int mbServerScriptEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, this->document()->blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 13 +  fontMetrics().width(QLatin1Char('9')) * (digits);

    return space;
}

void mbServerScriptEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}


void mbServerScriptEditor::updateLineNumberArea(QRectF /*rect_f*/)
{
    mbServerScriptEditor::updateLineNumberArea();
}
void mbServerScriptEditor::updateLineNumberArea(int /*slider_pos*/)
{
    mbServerScriptEditor::updateLineNumberArea();
}
void mbServerScriptEditor::updateLineNumberArea()
{
    /*
     * When the signal is emitted, the sliderPosition has been adjusted according to the action,
     * but the value has not yet been propagated (meaning the valueChanged() signal was not yet emitted),
     * and the visual display has not been updated. In slots connected to this signal you can thus safely
     * adjust any action by calling setSliderPosition() yourself, based on both the action and the
     * slider's value.
     */
    // Make sure the sliderPosition triggers one last time the valueChanged() signal with the actual value !!!!
    this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->sliderPosition());

    // Since "QTextEdit" does not have an "updateRequest(...)" signal, we chose
    // to grab the imformations from "sliderPosition()" and "contentsRect()".
    // See the necessary connections used (Class constructor implementation part).

    QRect rect =  this->contentsRect();
    lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());
    updateLineNumberAreaWidth(0);
    //----------
    int dy = this->verticalScrollBar()->sliderPosition();
    if (dy > -1) {
        lineNumberArea->scroll(0, dy);
    }

    // Addjust slider to alway see the number of the currently being edited line...
    int first_block_id = getFirstVisibleBlockId();
    if (first_block_id == 0 || this->textCursor().block().blockNumber() == first_block_id-1)
        this->verticalScrollBar()->setSliderPosition(dy-this->document()->documentMargin());

    //    // Snap to first line (TODO...)
    //    if (first_block_id > 0)
    //    {
    //        int slider_pos = this->verticalScrollBar()->sliderPosition();
    //        int prev_block_height = (int) this->document()->documentLayout()->blockBoundingRect(this->document()->findBlockByNumber(first_block_id-1)).height();
    //        if (dy <= this->document()->documentMargin() + prev_block_height)
    //            this->verticalScrollBar()->setSliderPosition(slider_pos - (this->document()->documentMargin() + prev_block_height));
    //    }

}

void mbServerScriptEditor::insertFromMimeData(const QMimeData *source)
{
    if (source->hasText())
        // If the source contains plain text, insert it without formatting
        insertPlainText(source->text());
}


void mbServerScriptEditor::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    QRect cr = this->contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}


int mbServerScriptEditor::getFirstVisibleBlockId()
{
    // Detect the first block for which bounding rect - once translated
    // in absolute coordinated - is contained by the editor's text area

    // Costly way of doing but since "blockBoundingGeometry(...)" doesn't
    // exists for "QTextEdit"...

    QTextCursor curs = QTextCursor(this->document());
    curs.movePosition(QTextCursor::Start);
    for(int i=0; i < this->document()->blockCount(); ++i)
    {
        QTextBlock block = curs.block();

        QRect r1 = this->viewport()->geometry();
        QRect r2 = this->document()->documentLayout()->blockBoundingRect(block).translated(
                                                                                   this->viewport()->geometry().x(), this->viewport()->geometry().y() - (
                                                                                       this->verticalScrollBar()->sliderPosition()
                                                                                       ) ).toRect();

        if (r1.contains(r2, true)) { return i; }

        curs.movePosition(QTextCursor::NextBlock);
    }

    return 0;
}

void mbServerScriptEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    this->verticalScrollBar()->setSliderPosition(this->verticalScrollBar()->sliderPosition());

    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);
    int blockNumber = this->getFirstVisibleBlockId();

    QTextBlock block = this->document()->findBlockByNumber(blockNumber);
    QTextBlock prev_block = (blockNumber > 0) ? this->document()->findBlockByNumber(blockNumber-1) : block;
    int translate_y = (blockNumber > 0) ? -this->verticalScrollBar()->sliderPosition() : 0;

    int top = this->viewport()->geometry().top();

    // Adjust text position according to the previous "non entirely visible" block
    // if applicable. Also takes in consideration the document's margin offset.
    int additional_margin;
    if (blockNumber == 0)
        // Simply adjust to document's margin
        additional_margin = (int) this->document()->documentMargin() -1 - this->verticalScrollBar()->sliderPosition();
    else
        // Getting the height of the visible part of the previous "non entirely visible" block
        additional_margin = (int) this->document()->documentLayout()->blockBoundingRect(prev_block)
                                .translated(0, translate_y).intersected(this->viewport()->geometry()).height();

    // Shift the starting point
    top += additional_margin;

    int bottom = top + (int) this->document()->documentLayout()->blockBoundingRect(block).height();

    QColor col_1(255, 255, 0);      // Current line (custom green)
    QColor col_0(120, 120, 120);    // Other lines  (custom darkgrey)

    // Draw the numbers (displaying the current line number in green)
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(QColor(120, 120, 120));
            painter.setPen((this->textCursor().blockNumber() == blockNumber) ? col_1 : col_0);
            painter.drawText(-5, top,
                             lineNumberArea->width(), fontMetrics().height(),
                             Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) this->document()->documentLayout()->blockBoundingRect(block).height();
        ++blockNumber;
    }

}
