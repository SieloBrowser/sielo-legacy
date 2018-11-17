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

#include "PluginListDelegate.hpp"

#include <QApplication>

namespace Sn
{
PluginListDelegate::PluginListDelegate(QListWidget* parent) :
	QStyledItemDelegate(parent)
{
	// Empty
}

void PluginListDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItem opt = option;
	initStyleOption(&opt, index);

	const QWidget* w{opt.widget};
	const QStyle* style{w ? w->style() : QApplication::style()};
	const int height{opt.rect.height()};
	const int center{height / 2 + opt.rect.top()};

	painter->setLayoutDirection(Qt::LeftToRight);

	// Prepare title font
	QFont titleFont{opt.font};
	titleFont.setBold(true);
	titleFont.setPointSize(titleFont.pointSize() + 1);

	const QFontMetrics titleMetrics{titleFont};
	const QPalette::ColorRole colorRole{opt.state & QStyle::State_Selected ? QPalette::HighlightedText : QPalette::Text};

	QPalette::ColorGroup cg{opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled};

	if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active)) 
		cg = QPalette::Inactive;

#ifdef Q_OS_WIN
	opt.palette.setColor(QPalette::All, QPalette::HighlightedText, opt.palette.color(QPalette::Active, QPalette::Text));
	opt.palette.setColor(QPalette::All, QPalette::Highlight, opt.palette.base().color().darker(108));
#endif

	QPalette textPalette{opt.palette};
	textPalette.setCurrentColorGroup(cg);

	int leftPosition{m_padding};
	int rightPosition{opt.rect.right() - m_padding};

	// Draw background
	style->drawPrimitive(QStyle::PE_PanelItemViewItem, &opt, painter, w);

	// Draw checkbox
	const int checkboxSize{18};
	const int checkboxYPos{center - (checkboxSize / 2)};
	
	QStyleOptionViewItem opt2 = opt;
	opt2.checkState == Qt::Checked ? opt2.state |= QStyle::State_On : opt2.state |= QStyle::State_Off;
	QRect styleCheckBoxRect = style->subElementRect(QStyle::SE_ViewItemCheckIndicator, &opt2, w);
	opt2.rect = QRect(leftPosition, checkboxYPos, styleCheckBoxRect.width(), styleCheckBoxRect.height());
	
	style->drawPrimitive(QStyle::PE_IndicatorViewItemCheck, &opt2, painter, w);
	leftPosition = opt2.rect.right() + m_padding;

	// Draw icon
	const int iconSize{32};
	const int iconYPos{center - (iconSize / 2)};
	QRect iconRect{leftPosition, iconYPos, iconSize, iconSize};
	QPixmap pixmap{index.data(Qt::DecorationRole).value<QIcon>().pixmap(iconSize)};

	painter->drawPixmap(iconRect, pixmap);
	leftPosition = iconRect.right() + m_padding;

	// Draw plugin name
	const QString name{index.data(Qt::DisplayRole).toString()};
	const int leftTitleEdge{leftPosition + 2};
	const int rightTitleEdge{rightPosition - m_padding};
	const int leftPosForVersion{titleMetrics.width(name) + m_padding};
	QRect nameRect{leftTitleEdge, opt.rect.top() + m_padding, rightTitleEdge - leftTitleEdge, titleMetrics.height()};
	
	painter->setFont(titleFont);
	style->drawItemText(painter, nameRect, Qt::AlignLeft, textPalette, true, name, colorRole);

	// Draw version
	const QString version{index.data(Qt::UserRole).toString()};
	QRect versionRect{nameRect.x() + leftPosForVersion, nameRect.y(), rightTitleEdge - leftPosForVersion, titleMetrics.height()};
	QFont versionFont{titleFont};
	
	versionFont.setBold(false);
	painter->setFont(versionFont);
	style->drawItemText(painter, versionRect, Qt::AlignLeft, textPalette, true, version, colorRole);

	// Draw info
	const int infoYPos{nameRect.bottom() + opt.fontMetrics.leading()};
	QRect infoRect{nameRect.x(), infoYPos, nameRect.width(), opt.fontMetrics.height()};
	const QString info{opt.fontMetrics.elidedText(index.data(Qt::UserRole + 1).toString(), Qt::ElideRight, infoRect.width())};
	
	painter->setFont(opt.font);
	style->drawItemText(painter, infoRect, Qt::TextSingleLine | Qt::AlignLeft, textPalette, true, info, colorRole);

	// Draw description
	const int descriptionYPos{infoRect.bottom() + opt.fontMetrics.leading()};
	QRect descriptionRect{infoRect.x(), descriptionYPos, infoRect.width(), opt.fontMetrics.height()};
	const QString description{opt.fontMetrics.elidedText(index.data(Qt::UserRole + 2).toString(), Qt::ElideRight, descriptionRect.width())};
	
	style->drawItemText(painter, descriptionRect, Qt::TextSingleLine | Qt::AlignLeft, textPalette, true, description, colorRole);
}

QSize PluginListDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(index)

	if (!m_rowHeight) {
		QStyleOptionViewItem opt{option};
		initStyleOption(&opt, index);

		const QWidget* widget{opt.widget};
		const QStyle* style{widget ? widget->style() : QApplication::style()};
		const int padding{style->pixelMetric(QStyle::PM_FocusFrameHMargin, nullptr) + 1};

		QFont titleFont{opt.font};
		titleFont.setBold(true);
		titleFont.setPointSize(titleFont.pointSize() + 1);

		m_padding = padding > 5 ? padding : 5;

		const QFontMetrics titleMetrics(titleFont);

		m_rowHeight = 2 * m_padding + 2 * opt.fontMetrics.leading() + 2 * opt.fontMetrics.height() + titleMetrics.height();
	}

	return QSize(200, m_rowHeight);
}
}
