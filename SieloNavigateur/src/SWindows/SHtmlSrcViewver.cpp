#include "includes/SWindows/SHtmlSrcViewver.hpp"
#include "includes/SMainWindow.hpp"

template<typename Arg, typename R, typename C>
struct InvokeWrapper {
	R *receiver;
	void (C::*memberFun)(Arg);
	void operator()(Arg result) {
		(receiver->*memberFun)(result);
	}
};

template<typename Arg, typename R, typename C>
InvokeWrapper<Arg, R, C> invoke(R *receiver, void (C::*memberFun)(Arg))
{
	InvokeWrapper<Arg, R, C> wrapper = {receiver, memberFun};
	return wrapper;
}

SHtmlHighlighter::SHtmlHighlighter(QTextDocument *document) :
	QSyntaxHighlighter(document)
{
	QTextCharFormat entityFormat{};
	entityFormat.setForeground(QColor(0, 128, 0));
	entityFormat.setFontWeight(QFont::Bold);
	setFormatFor(Entity, entityFormat);

	QTextCharFormat tagFormat{};
	tagFormat.setForeground(QColor(192, 16, 112));
	tagFormat.setFontWeight(QFont::Bold);
	setFormatFor(Tag, tagFormat);

	QTextCharFormat commentFormat{};
	commentFormat.setForeground(QColor(128, 128, 128));
	commentFormat.setFontItalic(true);
	setFormatFor(Comment, commentFormat);
}

SHtmlHighlighter::~SHtmlHighlighter()
{
	// Empty
}

void SHtmlHighlighter::setFormatFor(Construct construct, const QTextCharFormat &format)
{
	m_formats[construct] = format;
	rehighlight();
}

void SHtmlHighlighter::highlightBlock(const QString &text)
{
	int state{ previousBlockState() };
	int len{ text.length() };
	int start{ 0 };
	int pos{ 0 };

	while (pos < len) {
		switch (state) {
			case NormalState:
				default:
					while (pos < len) {
						QChar ch = text.at(pos);
						if (ch == '<') {
							if (text.mid(pos, 4) == "<!--") {
							state = InComment;
							}
							else {
							state = InTag;
							}
							break;
						}
						else if (ch == '&') {
							start = pos;
							while (pos < len && text.at(pos++) != ';');
							setFormat(start, pos - start, m_formats[Entity]);
						} else {
						++pos;
						}
					}
				break;
			case InComment:
				start = pos;
				while (pos < len) {
					if (text.mid(pos, 3) == "-->") {
						pos += 3;
						state = NormalState;
						break;
					}
					else {
						++pos;
					}
				}
				setFormat(start, pos - start,
				m_formats[Comment]);
				break;
			case InTag:
				QChar quote = QChar::Null;
				start = pos;
				while (pos < len) {
					QChar ch = text.at(pos);
					if (quote.isNull()) {
					if (ch == '\\' || ch == '"') {
					quote = ch;
					} else if (ch == '>') {
					++pos;
					state = NormalState;
					break;
					}
					} else if (ch == quote) {
					quote = QChar::Null;
					}
					++pos;
				}
			setFormat(start, pos - start, m_formats[Tag]);
		}
	}

	setCurrentBlockState(state);
}

SHtmlSrcViewver::SHtmlSrcViewver(SMainWindow *parent) :
	QDialog(parent),
	m_parent(parent)
{
	setWindowTitle(tr("Source de la page ").arg(m_parent->currentPage()->title()));
	setMinimumWidth(640);
	setMinimumHeight(560);

	m_layout->addWidget(m_src);
	m_layout->addWidget(m_boxBtn);

	m_htmlHighlighter = new SHtmlHighlighter(m_src->document());
	m_parent->currentPage()->page()->toHtml(invoke(m_src, &QTextEdit::setPlainText));

	connect(m_boxBtn, &QDialogButtonBox::accepted, this, &SHistoryWindow::accept);
	connect(m_boxBtn, &QDialogButtonBox::rejected, this, &SHistoryWindow::reject);
}

SHtmlSrcViewver::~SHtmlSrcViewver()
{

}
