#pragma once

#include <QtWidgets>
#include <QtGui>

class SMainWindow;

class SHtmlHighlighter : public QSyntaxHighlighter
{
public:
    enum Construct {
        Entity,
        Tag,
        Comment,
        LastConstruct = Comment
    };

    SHtmlHighlighter(QTextDocument *document);

    void setFormatFor(Construct construct, const QTextCharFormat &format);
    QTextCharFormat formatFor(Construct construct) const { return m_formats[construct]; }

protected:
    enum State {
        NormalState = -1,
        InComment,
        InTag
    };

    void highlightBlock(const QString &text);

private:
    QTextCharFormat m_formats[LastConstruct + 1]{};
};

class SHtmlSrcViewver : public QDialog
{
public:
    SHtmlSrcViewver(SMainWindow* parent = nullptr);
    ~SHtmlSrcViewver();

private:
    SMainWindow *m_parent{ nullptr };
    SHtmlHighlighter *m_htmlHighlighter{ nullptr };
    QVBoxLayout *m_layout{ new QVBoxLayout(this) };
    QTextEdit *m_src{ new QTextEdit(this) };
    QDialogButtonBox* m_boxBtn{ new QDialogButtonBox(Qt::Horizontal, this) };
};
