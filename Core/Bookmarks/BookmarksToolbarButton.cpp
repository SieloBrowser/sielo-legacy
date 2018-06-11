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

#include "BookmarksToolbarButton.hpp"

#include <QPainter>
#include <QStyleOptionToolButton>

#include "Bookmarks/Bookmarks.hpp"
#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarkItem.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

constexpr const int MAX_WIDTH = 150;
constexpr const int SEPARATOR_WIDTH = 8;
constexpr const int PADDING = 5;

namespace Sn
{
BookmarksToolbarButton::BookmarksToolbarButton(BookmarkItem* bookmark, QWidget* parent) :
	QToolButton(parent),
	m_bookmark(bookmark)
{
	Q_ASSERT(m_bookmark);

	setFocusPolicy(Qt::NoFocus);
	setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
	setToolTip(createTooltip());

	if (m_bookmark->isFolder()) {
		QMenu* menu{new QMenu(this)};
		setMenu(menu);
		createMenu();
	}
}

BookmarksToolbarButton::~BookmarksToolbarButton()
{
	// Empty
}

void BookmarksToolbarButton::setMainWindow(BrowserWindow* window)
{
	m_window = window;
}

void BookmarksToolbarButton::setShowOnlyIcon(bool show)
{
	m_showOnlyIcon = show;

	updateGeometry();
	update();
}

void BookmarksToolbarButton::setShowOnlyText(bool show)
{
	m_showOnlyText = show;

	updateGeometry();
	update();
}

QSize BookmarksToolbarButton::sizeHint() const
{
	int width{PADDING * 2};

	if (!m_showOnlyText)
		width += 16;

	if (m_bookmark->isSeparator())
		width = SEPARATOR_WIDTH;
	else if (!m_showOnlyIcon) {
		width += PADDING * 2 + fontMetrics().width(m_bookmark->title());

		if (menu())
			width += PADDING + 8;
	}

	return QSize(qMin(width, MAX_WIDTH), preferredHeight());
}

QSize BookmarksToolbarButton::minimumSizeHint() const
{
	int width{PADDING * 2};

	if (!m_showOnlyText)
		width += 16;

	if (m_bookmark->isSeparator())
		width = SEPARATOR_WIDTH;
	else if (!m_showOnlyIcon && menu())
		width += PADDING + 8;

	return QSize(width, preferredHeight());
}

int BookmarksToolbarButton::preferredHeight() const
{
	return fontMetrics().height() + PADDING * 2;
}

void BookmarksToolbarButton::createMenu()
{
	if (!menu()->isEmpty())
		return;

	QMenu* m = qobject_cast<QMenu*>(menu());
	Q_ASSERT(m);

	addFolderContentsToMenu(m, m_bookmark);
}

void BookmarksToolbarButton::menuAboutToShow()
{
	Q_ASSERT(qobject_cast<QMenu*>(sender()));
	QMenu* menu = static_cast<QMenu*>(sender());

	foreach(QAction *action, menu->actions()) {
		BookmarkItem* item{static_cast<BookmarkItem*>(action->data().value<void*>())};

		if (item && item->type() == BookmarkItem::Url && action->icon().isNull())
			action->setIcon(item->icon());
	}
}

void BookmarksToolbarButton::bookmarkActivated(BookmarkItem* item)
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		item = static_cast<BookmarkItem*>(action->data().value<void*>());

	Q_ASSERT(item);

	openBookmark(item);
}

void BookmarksToolbarButton::bookmarkCtrlActivated(BookmarkItem* item)
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		item = static_cast<BookmarkItem*>(action->data().value<void*>());

	Q_ASSERT(item);

	openBookmarkInNewTab(item);
}

void BookmarksToolbarButton::bookmarkShiftActivated(BookmarkItem* item)
{
	if (QAction* action = qobject_cast<QAction*>(sender()))
		item = static_cast<BookmarkItem*>(action->data().value<void*>());

	Q_ASSERT(item);

	openBookmarkInNewWindow(item);
}

void BookmarksToolbarButton::openFolder(BookmarkItem* item)
{
	Q_ASSERT(item->isFolder());

	if (m_window)
		BookmarksUtils::openFolderInTabs(m_window, item);
}

void BookmarksToolbarButton::openBookmark(BookmarkItem* item)
{
	Q_ASSERT(item->isUrl());

	if (m_window)
		BookmarksUtils::openBookmark(m_window, item);
}

void BookmarksToolbarButton::openBookmarkInNewTab(BookmarkItem* item)
{
	Q_ASSERT(item->isUrl());

	if (m_window)
		BookmarksUtils::openBookmarkInNewTab(m_window, item);
}

void BookmarksToolbarButton::openBookmarkInNewWindow(BookmarkItem* item)
{
	Q_ASSERT(item->isUrl());

	BookmarksUtils::openBookmarkInNewWindow(item);
}

QString BookmarksToolbarButton::createTooltip() const
{
	if (!m_bookmark->description().isEmpty()) {
		if (!m_bookmark->urlString().isEmpty())
			return QString("%1\n%2").arg(m_bookmark->description(), m_bookmark->urlString());

		return m_bookmark->description();
	}

	if (!m_bookmark->url().isEmpty())
		return QString("%1\n%2").arg(m_bookmark->title(), m_bookmark->urlString());

	return m_bookmark->title();
}

void BookmarksToolbarButton::enterEvent(QEvent* event)
{
	QToolButton::enterEvent(event);

	update();
}

void BookmarksToolbarButton::leaveEvent(QEvent* event)
{
	QToolButton::leaveEvent(event);

	update();
}

void BookmarksToolbarButton::mousePressEvent(QMouseEvent* event)
{
	if (m_bookmark && m_bookmark->isFolder()) {
		if (event->buttons() == Qt::LeftButton && event->modifiers() == Qt::ControlModifier) {
			openFolder(m_bookmark);
			return;
		}
	}

	QToolButton::mousePressEvent(event);
}

void BookmarksToolbarButton::mouseReleaseEvent(QMouseEvent* event)
{
	if (m_bookmark && rect().contains(event->pos())) {
		Qt::MouseButton button = event->button();
		Qt::KeyboardModifiers modifiers = event->modifiers();

		if (m_bookmark->isUrl()) {
			if (button == Qt::LeftButton && modifiers == Qt::NoModifier)
				bookmarkActivated(m_bookmark);
			else if (button == Qt::LeftButton && modifiers == Qt::ShiftModifier)
				bookmarkShiftActivated(m_bookmark);
			else if (button == Qt::MiddleButton || modifiers == Qt::ControlModifier)
				bookmarkCtrlActivated(m_bookmark);
		}
		else if (m_bookmark->isFolder() && button == Qt::MiddleButton)
			openFolder(m_bookmark);
	}

	QToolButton::mouseReleaseEvent(event);
}

void BookmarksToolbarButton::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event)

	QPainter p{this};

	// Just draw separator
	if (m_bookmark->isSeparator()) {
		QStyleOption opt{};
		opt.initFrom(this);
		opt.state |= QStyle::State_Horizontal;

		style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p);
		return;
	}

	QStyleOptionToolButton option{};
	initStyleOption(&option);

	option.features &= ~QStyleOptionButton::HasMenu;

	if (isDown() || underMouse()) {
		option.state |= QStyle::State_AutoRaise | QStyle::State_Raised;
		style()->drawPrimitive(QStyle::PE_PanelButtonTool, &option, &p, this);
	}

	const int shiftX{isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &option, this) : 0};
	const int shiftY{isDown() ? style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &option, this) : 0};

	const int height{option.rect.height()};
	const int center{height / 2 + option.rect.top() + shiftY};

	const int iconSize{16};
	const int iconYPos{center - iconSize / 2};

	int leftPosition{PADDING + shiftX};
	int rightPosition{option.rect.right() - PADDING};

	if (!m_showOnlyText) {
		QRect iconRect{leftPosition, iconYPos, iconSize, iconSize};

		p.drawPixmap(QStyle::visualRect(option.direction, option.rect, iconRect), m_bookmark->icon().pixmap(iconSize));
		leftPosition = iconRect.right() + PADDING;
	}

	if (!m_showOnlyIcon && menu()) {
		const int arrowSize{8};
		QStyleOption opt{};
		opt.initFrom(this);
		const QRect rect{rightPosition - 8, center - arrowSize / 2, arrowSize, arrowSize};
		opt.rect = QStyle::visualRect(option.direction, option.rect, rect);
		opt.state &= ~QStyle::State_MouseOver;

		style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, this);
		rightPosition = rect.left() - PADDING;
	}

	if (!m_showOnlyIcon) {
		const int textWidth{rightPosition - leftPosition};
		const int textYPos{center - fontMetrics().height() / 2};
		const QString txt{fontMetrics().elidedText(m_bookmark->title(), Qt::ElideRight, textWidth)};
		QRect textRect{leftPosition, textYPos, textWidth, fontMetrics().height()};

		style()->drawItemText(&p, QStyle::visualRect(option.direction, option.rect, textRect),
		                      Qt::TextSingleLine | Qt::AlignCenter, option.palette, true, txt);
	}
}

void BookmarksToolbarButton::addActionToMenu(QMenu* menu, BookmarkItem* item)
{
	Q_ASSERT(menu);
	Q_ASSERT(item);

	switch (item->type()) {
	case BookmarkItem::Url:
		addUrlToMenu(menu, item);
		break;
	case BookmarkItem::Folder:
		addFolderToMenu(menu, item);
		break;
	case BookmarkItem::Separator:
		addSeparatorToMenu(menu, item);
		break;
	default:
		break;
	}
}

void BookmarksToolbarButton::addUrlToMenu(QMenu* menu, BookmarkItem* bookmark)
{
	Q_ASSERT(menu);
	Q_ASSERT(bookmark);
	Q_ASSERT(bookmark->isUrl());

	QAction* action{new QAction(menu)};
	QString title{QFontMetrics(action->font()).elidedText(bookmark->title(), Qt::ElideRight, 250)};

	action->setText(title);
	action->setData(QVariant::fromValue<void*>(static_cast<void*>(bookmark)));
	action->setIconVisibleInMenu(true);

	connect(action, &QAction::triggered, this, &BookmarksToolbarButton::bookmarkActivated);

	menu->addAction(action);
}

void BookmarksToolbarButton::addFolderToMenu(QMenu* menu, BookmarkItem* folder)
{
	Q_ASSERT(menu);
	Q_ASSERT(folder);
	Q_ASSERT(folder->isFolder());

	QMenu* m{new QMenu(menu)};
	QString title{QFontMetrics(m->font()).elidedText(folder->title(), Qt::ElideRight, 250)};

	m->setTitle(title);
	m->setIcon(folder->icon());

	addFolderContentsToMenu(m, folder);

	QAction* action{menu->addMenu(m)};
	action->setData(QVariant::fromValue<void*>(static_cast<void*>(folder)));
	action->setIconVisibleInMenu(true);
}

void BookmarksToolbarButton::addSeparatorToMenu(QMenu* menu, BookmarkItem* separator)
{
	Q_UNUSED(separator)
	Q_ASSERT(menu);
	Q_ASSERT(separator->isSeparator());

	menu->addSeparator();
}

void BookmarksToolbarButton::addFolderContentsToMenu(QMenu* menu, BookmarkItem* folder)
{
	connect(menu, &QMenu::aboutToShow, this, &BookmarksToolbarButton::menuAboutToShow);

	foreach(BookmarkItem* child, folder->children())
		addActionToMenu(menu, child);

	if (menu->isEmpty())
		menu->addAction(tr("Empty"))->setEnabled(false);
}
}
