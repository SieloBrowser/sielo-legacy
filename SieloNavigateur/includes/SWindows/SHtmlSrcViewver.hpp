#pragma once

#include <QtWidgets>
#include <QtGui>

class SMainWindow;

// This is the class used for coloring text in QTextEdit
class SHtmlHighlighter : public QSyntaxHighlighter
{
public:
	enum Construct {
		Entity,
		Tag,
		Comment,
		LastConstruct = Comment
	};

	// Constructor and destructor
	SHtmlHighlighter(QTextDocument *document);
	~SHtmlHighlighter();

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

// This is the class for showing the source code of the web page
class SHtmlSrcViewver : public QDialog
{
public:
	SHtmlSrcViewver(SMainWindow* parent = nullptr);
	~SHtmlSrcViewver();

private:
	SMainWindow *m_parent{ nullptr };

	// Layout of the window
	QVBoxLayout *m_layout{ new QVBoxLayout(this) };

	// Widgets of the window
	SHtmlHighlighter *m_htmlHighlighter{ nullptr };
	QTextEdit *m_src{ new QTextEdit(this) };
	QDialogButtonBox* m_boxBtn{ new QDialogButtonBox(QDialogButtonBox::Ok, Qt::Horizontal, this) };
};
