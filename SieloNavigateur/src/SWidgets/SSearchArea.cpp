#include "includes/SWidgets/SSearchArea.hpp"
#include "includes/SMainWindow.hpp"

SSearchArea::SSearchArea(const QIcon & icon, SMainWindow * parent) : 
	QLineEdit(parent),
	m_parent(parent),
	m_icon(icon)
{
	setTextMargins(18, 0, 0, 0);
	connect(this, &SSearchArea::returnPressed, this, &SSearchArea::loadSearch);
	setClearButtonEnabled(true);
}

SSearchArea::~SSearchArea()
{
	// Empty
}

void SSearchArea::paintEvent(QPaintEvent * event)
{
	// Paint the shearch icon before the line edit
	QLineEdit::paintEvent(event);
	QPainter painter(this);
	m_icon.paint(&painter, (height() - 16) / 2, (height() - 16) / 2, 16, 16);
}

void SSearchArea::loadSearch()
{
	// Load the search
	// TODO: fix the bug when we seach with "+"
	QString search{ text() };
	search.replace(" ", "+");
	m_parent->currentPage()->load(QUrl("http://www.google.com/search?q=" + search));
}
