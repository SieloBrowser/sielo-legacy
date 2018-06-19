/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2018 Victor DENIS (victordenis01@gmail.com)                      **
**                                                                                **
** Permission is hereby granted, free of charge, to any person obtaining a copy   **
** of this software and associated documentation files (the "Software"), to deal  **
** in the Software without restriction, including without limitation the rights   **
** to use, copy, modify, merge, publish, distribute, sublicense, and/or sell      **
** copies of the Software, and to permit persons to whom the Software is          **
** furnished to do so, subject to the following conditions:                       **
**                                                                                **
** The above copyright notice and this permission notice shall be included in all **
** copies or substantial portions of the Software.                                **
**                                                                                **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR     **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,       **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE    **
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER         **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  **
** OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  **
** SOFTWARE.                                                                      **
***********************************************************************************/

#include "AddressBarCompleterDelegate.hpp"

#include <QPainter>

#include <QTextLayout>

#include "Widgets/AddressBar/AddressBarCompleterModel.hpp"

#include "Application.hpp"

namespace Sn
{
AddressBarCompleterDelegate::AddressBarCompleterDelegate(QObject* parent) :
	QStyledItemDelegate(parent)
{
	// Empty
}

void AddressBarCompleterDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option,
                                        const QModelIndex& index) const
{
	QStyleOptionViewItem opt{option};
	initStyleOption(&opt, index);

	QColor backgroundColor{ option.palette.window().color() };
	const double darkness{ 1 - (0.299 * backgroundColor.red() + 0.587 * backgroundColor.green() +
		0.115 * backgroundColor.blue()) / 255 };
	bool isDark{ darkness >= 0.5 };

	const QWidget* w{opt.widget};
	const QStyle* style{w ? w->style() : QApplication::style()};

	const int height{opt.rect.height()};
	const int center{height / 2 + opt.rect.top()};

	// Prepare title font
	QFont titleFont = opt.font;
	titleFont.setPointSize(titleFont.pointSize() + 1);

	const QFontMetrics titleMetrics{titleFont};

	int leftPosition{m_padding * 2};
	int rightPosition{opt.rect.right() - m_padding};

	opt.state |= QStyle::State_Active;

	const QIcon::Mode iconMode{opt.state & QStyle::State_Selected ? QIcon::Selected : QIcon::Normal};
	QColor textColor{};
	QColor highlightedTextColor{};
	QColor linkColor{};

	if (isDark) {
		textColor = QColor(250, 250, 250);
		highlightedTextColor = QColor(200, 200, 200);
		linkColor = QColor(0, 157, 255);
	}
	else {
		textColor = QColor(10, 10, 10);
		highlightedTextColor = QColor(55, 55, 55);
		linkColor = QColor(0, 57, 255);
	}
/*
	const QPalette::ColorRole colorRole{opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text};
	const QPalette::ColorRole colorLinkRole{
		opt.state & QStyle::State_Selected
			? QPalette::HighlightedText
			: QPalette::Link
	};
*/

	QPalette textPalette{opt.palette};
	//textPalette.setCurrentColorGroup(opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled);

/*
	textPalette.setColor(QPalette::All, QPalette::HighlightedText, QColor(255, 255, 255));
	textPalette.setColor(QPalette::All, QPalette::Text, QColor(255, 255, 255));
	textPalette.setColor(QPalette::All, QPalette::Link, QColor(255, 255, 255));
*/	

	// Draw background
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, w);

	const bool isVisitSearchItem{index.data(AddressBarCompleterModel::VisitSearchItemRole).toBool()};
	const bool isSearchSuggestion{index.data(AddressBarCompleterModel::SearchSuggestionRole).toBool()};
	const bool isWebSearch{!isUrlOrDomain(m_originalText.trimmed())};

	// Draw icon
	const int iconSize{16};
	const int iconYPos{center - (iconSize / 2)};
	QRect iconRect{leftPosition, iconYPos, iconSize, iconSize};
	QPixmap pixmap{index.data(Qt::DecorationRole).value<QIcon>().pixmap(iconSize, iconMode)};

	if (isSearchSuggestion || (isVisitSearchItem && isWebSearch))
		pixmap = Application::getAppIcon("search").pixmap(iconSize, iconMode);

	painter->drawPixmap(iconRect, pixmap);
	leftPosition = iconRect.right() + m_padding * 2;

	// Draw star to bookmark items
	int starPixmapWidth{0};
	if (index.data(AddressBarCompleterModel::BookmarkRole).toBool()) {
		const QIcon icon{Application::getAppIcon("bookmarks")};
		const QSize starSize{16, 16};
		starPixmapWidth = starSize.width();
		QPoint pos{rightPosition - starPixmapWidth, center - starSize.height() / 2};
		QRect starRect{pos, starSize};
		painter->drawPixmap(starRect, icon.pixmap(starSize, iconMode));
	}

	QString searchText{index.data(AddressBarCompleterModel::SearchStringRole).toString()};

	// Draw title
	leftPosition += 2;
	QRect titleRect{
		leftPosition, center - titleMetrics.height() / 2, static_cast<int>(opt.rect.width() * 0.6), titleMetrics.height()
	};
	QString title{index.data(AddressBarCompleterModel::TitleRole).toString()};
	painter->setFont(titleFont);

	if (isVisitSearchItem) {
		title = m_originalText.trimmed();
		if (searchText == title)
			searchText.clear();
	}

	leftPosition += viewItemDrawText(painter, &opt, titleRect, title, opt.state & QStyle::State_Selected ? highlightedTextColor : textColor, searchText);
	leftPosition += m_padding * 2;

	const int maxChars{(opt.rect.width() - leftPosition) / opt.fontMetrics.width(QLatin1Char('i'))};
	QString link{};
	const QByteArray linkArray{index.data(Qt::DisplayRole).toByteArray()};

	if (!linkArray.startsWith("data") && !linkArray.startsWith("javascript"))
		link = QString::fromUtf8(QByteArray::fromPercentEncoding(linkArray)).left(maxChars);
	else
		link = QString::fromLatin1(linkArray.left(maxChars));

	if (isVisitSearchItem || isSearchSuggestion) {
		if (!isSearchSuggestion && !isWebSearch)
			link = tr("Visit");
		else if (opt.state.testFlag(QStyle::State_Selected) || opt.state.testFlag(QStyle::State_MouseOver))
			link = tr("Search with your search engine");
		else
			link.clear();
	}

	// Draw separator
	if (!link.isEmpty()) {
		QChar separator{QLatin1Char('-')};
		QRect separatorRect{
			leftPosition, center - titleMetrics.height() / 2, titleMetrics.width(separator),
			titleMetrics.height()
		};
		style->drawItemText(painter, separatorRect, Qt::AlignCenter, textPalette, true, separator, opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text);
		leftPosition += separatorRect.width() + m_padding * 2;
	}

	// Draw link
	const int leftLinkEdge{leftPosition};
	const int rightLinkEdge{rightPosition - m_padding - starPixmapWidth};
	QRect linkRect{
		leftLinkEdge, center - opt.fontMetrics.height() / 2, rightLinkEdge - leftLinkEdge,
		opt.fontMetrics.height()
	};

	painter->setFont(opt.font);

	// Draw url (or switch to tab)
	int tabPos{index.data(AddressBarCompleterModel::TabPositionTabRole).toInt()};

	if (drawSwitchToTab() && tabPos != -1) {
		// TODO: have an icon for real tab
		const QIcon tabIcon = Application::getAppIcon("new-tab");
		QRect iconRect{linkRect};
		iconRect.setX(iconRect.x());
		iconRect.setWidth(16);
		painter->drawPixmap(iconRect, tabIcon.pixmap(iconRect.size(), iconMode));

		QRect textRect{linkRect};
		textRect.setX(textRect.x() + m_padding + 16 + m_padding);
		viewItemDrawText(painter, &opt, textRect, tr("Switch to tab"), opt.state & QStyle::State_Selected ? highlightedTextColor : linkColor);
	}
	else if (isVisitSearchItem || isSearchSuggestion)
		viewItemDrawText(painter, &opt, linkRect, link, opt.state & QStyle::State_Selected ? highlightedTextColor : linkColor);

	else
		viewItemDrawText(painter, &opt, linkRect, link, opt.state & QStyle::State_Selected ? highlightedTextColor : linkColor, searchText);

	// Draw line at the very bottom of item if the item is not highlighted
	if (!(opt.state & QStyle::State_Selected)) {
		QRect lineRect{opt.rect.left(), opt.rect.bottom(), opt.rect.width(), 1};
		painter->fillRect(lineRect, opt.palette.color(QPalette::AlternateBase));
	}
}

QSize AddressBarCompleterDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)
{
	Q_UNUSED(index)

	if (!m_rowHeight) {
		QStyleOptionViewItem opt{option};
		initStyleOption(&opt, index);

		const QWidget* widget{opt.widget};
		const QStyle* style{widget ? widget->style() : Application::style()};
		const int padding{style->pixelMetric(QStyle::PM_FocusFrameHMargin, 0) + 1};

		QFont titleFont{opt.font};
		titleFont.setPointSize(titleFont.pointSize() + 1);

		m_padding = padding > 3 ? padding : 3;

		const QFontMetrics titleMetrics{titleFont};

		m_rowHeight = 4 * m_padding + qMax(16, titleMetrics.height());
	}

	return QSize(200, m_rowHeight);
}

void AddressBarCompleterDelegate::setShowSwitchToTab(bool enable)
{
	m_drawSwitchToTab = enable;
}

void AddressBarCompleterDelegate::setOriginalText(const QString& originalText)
{
	m_originalText = originalText;
}

bool AddressBarCompleterDelegate::isUrlOrDomain(const QString& text) const
{
	QUrl url{text};
	if (!url.scheme().isEmpty() && (!url.host().isEmpty() || !url.path().isEmpty()))
		return true;
	if (text.contains(QLatin1Char('.')) && !text.contains(QLatin1Char(' ')))
		return true;
	if (text == QLatin1String("localhost"))
		return true;

	return false;
}

QSizeF AddressBarCompleterDelegate::viewItemTextLayout(QTextLayout& textLayout, int lineWidth) const
{
	qreal height{0};
	qreal widthUsed{0};

	textLayout.beginLayout();
	QTextLine line{textLayout.createLine()};

	if (line.isValid()) {
		line.setLineWidth(lineWidth);
		line.setPosition(QPointF(0, height));
		height += line.height();
		widthUsed = qMax(widthUsed, line.naturalTextWidth());

		textLayout.endLayout();
	}

	return QSizeF(widthUsed, height);
}

int AddressBarCompleterDelegate::viewItemDrawText(QPainter* painter, const QStyleOptionViewItem* option,
                                                  const QRect& rect,
                                                  const QString& text, const QColor& color,
                                                  const QString& searchText) const
{
	if (text.isEmpty())
		return 0;

	const QFontMetrics fontMetrics{painter->font()};
	QString elidedText{fontMetrics.elidedText(text, option->textElideMode, rect.width())};

	QTextOption textOption{};
	textOption.setWrapMode(QTextOption::NoWrap);
	textOption.setAlignment(QStyle::visualAlignment(textOption.textDirection(), option->displayAlignment));

	QTextLayout textLayout{};
	textLayout.setFont(painter->font());
	textLayout.setText(elidedText);
	textLayout.setTextOption(textOption);

	if (!searchText.isEmpty()) {
		QList<int> delimiters{};
		QStringList searchStrings{searchText.split(QLatin1Char(' '), QString::SkipEmptyParts)};

		std::sort(searchStrings.begin(), searchStrings.end(), [](const QString& size1, const QString& size2) {
			return size1.size() > size2.size();
		});

		foreach(const QString &string, searchStrings) {
			int delimiter{text.indexOf(string, 0, Qt::CaseInsensitive)};

			while (delimiter != -1) {
				int start{delimiter};
				int end{delimiter + string.length()};
				bool alreadyContains{false};

				for (int i{0}; i < delimiters.count(); ++i) {
					int dStart{delimiters[i]};
					int dEnd{delimiters[++i]};

					if (dStart <= start && dEnd >= end) {
						alreadyContains = true;
						break;
					}
				}
				if (!alreadyContains) {
					delimiters.append(start);
					delimiters.append(end);
				}

				delimiter = text.indexOf(string, end, Qt::CaseInsensitive);
			}
		}

		std::sort(delimiters.begin(), delimiters.end());

		if (!delimiters.isEmpty() && !(delimiters.count() % 2)) {
			QList<QTextLayout::FormatRange> highlightParts{};

			QTextLayout::FormatRange lighterWholeLine{};
			lighterWholeLine.start = 0;
			lighterWholeLine.length = elidedText.size();
			QColor lighterColor{color.lighter(130)};

			if (lighterColor == color)
				lighterColor = QColor(Qt::gray).darker(180);

			lighterWholeLine.format.setForeground(lighterColor);
			highlightParts << lighterWholeLine;

			while (!delimiters.isEmpty()) {
				QTextLayout::FormatRange highlightedPart{};
				int start{delimiters.takeFirst()};
				int end{delimiters.takeFirst()};

				highlightedPart.start = start;
				highlightedPart.length = end - start;
				highlightedPart.format.setFontWeight(QFont::Bold);
				highlightedPart.format.setUnderlineStyle(QTextCharFormat::SingleUnderline);
				highlightedPart.format.setForeground(color);

				highlightParts << highlightedPart;
			}

			textLayout.setAdditionalFormats(highlightParts);
		}
	}

	viewItemTextLayout(textLayout, rect.width());

	if (textLayout.lineCount() <= 0)
		return 0;

	QTextLine textLine{textLayout.lineAt(0)};
	int diff = textLine.naturalTextWidth() - rect.width();

	if (diff > 0) {
		elidedText = fontMetrics.elidedText(elidedText, option->textElideMode, rect.width() - diff);

		textLayout.setText(elidedText);

		viewItemTextLayout(textLayout, rect.width());

		if (textLayout.lineCount() <= 0)
			return 0;

		textLine = textLayout.lineAt(0);
	}

	painter->setPen(color);
	qreal width{qMax<qreal>(rect.width(), textLayout.lineAt(0).width())};
	const QRect& layoutRect{
		QStyle::alignedRect(option->direction, option->displayAlignment,
		                    QSize(int(width), int(textLine.height())), rect)
	};
	const QPointF& position{layoutRect.topLeft()};

	textLine.draw(painter, position);

	return qMin<int>(rect.width(), textLayout.lineAt(0).naturalTextWidth());
}
}
