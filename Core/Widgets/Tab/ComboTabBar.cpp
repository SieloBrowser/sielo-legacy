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

#include "Widgets/Tab/ComboTabBar.hpp"

#include <QApplication>

#include <QPainter>
#include <QStyleOption>

#include <QToolTip>

#include "Utils/ToolButton.hpp"

#include "Widgets/Tab/TabBar.hpp"
#include "Widgets/Tab/TabIcon.hpp"
#include "Widgets/Tab/TabScrollBar.hpp"
#include "Widgets/Tab/TabCloseButton.hpp"
#include "Widgets/Tab/TabBarScrollWidget.hpp"

namespace Sn
{

ComboTabBar::ComboTabBar(QWidget* parent) :
	QWidget(parent),
	m_mainBarOverFlowed(false),
	m_lastAppliedOverflow(false),
	m_usesScrollButton(false),
	m_blockCurrentChangedSignal(false)
{
	QObject::setObjectName(QLatin1String("tabbarwidget"));

	m_leftLayout = new QHBoxLayout{};
	m_rightLayout = new QHBoxLayout{};
	m_layout = new QHBoxLayout{this};

	m_leftLayout->setSpacing(0);
	m_leftLayout->setContentsMargins(0, 0, 0, 0);
	m_rightLayout->setSpacing(0);
	m_rightLayout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_leftContainer = new QWidget(this);
	m_leftContainer->setLayout(m_leftLayout);
	m_leftContainer->setObjectName(QLatin1String("tabbarwidget-left-container"));


	m_rightContainer = new QWidget(this);
	m_rightContainer->setLayout(m_rightLayout);
	m_leftContainer->setObjectName(QLatin1String("tabbarwidget-right-container"));


	m_mainTabBar = new TabBar(false, this);
	m_pinnedTabBar = new TabBar(true, this);
	m_mainTabBarWidget = new TabBarScrollWidget(m_mainTabBar, this);
	m_pinnedTabBarWidget = new TabBarScrollWidget(m_pinnedTabBar, this);

	m_mainTabBar->setObjectName("tabbar");
	m_pinnedTabBar->setObjectName("pinnedtabbar");

	m_mainTabBar->setScrollArea(m_mainTabBarWidget->scrollArea());
	m_mainTabBar->setActiveTabBar(true);
	m_pinnedTabBar->setScrollArea(m_pinnedTabBarWidget->scrollArea());
	m_pinnedTabBar->setTabsClosable(false);

	setAutoFillBackground(false);
	m_mainTabBar->setAutoFillBackground(false);
	m_pinnedTabBar->setAutoFillBackground(false);

	m_layout->addWidget(m_leftContainer);
	m_layout->addWidget(m_pinnedTabBarWidget);
	m_layout->addWidget(m_mainTabBarWidget);
	m_layout->addWidget(m_rightContainer);

	connect(m_mainTabBarWidget->scrollBar(), &TabScrollBar::rangeChanged, this, &ComboTabBar::setMinimumWidths);
	connect(m_mainTabBarWidget->scrollBar(), SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarValueChanged(int)));
	connect(m_mainTabBar, &TabBar::currentChanged, this, &ComboTabBar::sCurrentChanged);
	connect(m_mainTabBar, &TabBar::tabCloseRequested, this, &ComboTabBar::sTabCloseRequested);
	connect(m_mainTabBar, &TabBar::tabMoved, this, &ComboTabBar::sTabMoved);

	connect(m_pinnedTabBarWidget->scrollBar(), &TabScrollBar::rangeChanged, this, &ComboTabBar::setMinimumWidths);
	connect(m_pinnedTabBarWidget->scrollBar(), SIGNAL(valueChanged(int)), this, SIGNAL(scrollBarValueChanged(int)));
	connect(m_pinnedTabBar, &TabBar::currentChanged, this, &ComboTabBar::sCurrentChanged);
	connect(m_pinnedTabBar, &TabBar::tabCloseRequested, this, &ComboTabBar::sTabCloseRequested);
	connect(m_pinnedTabBar, &TabBar::tabMoved, this, &ComboTabBar::sTabMoved);

	connect(this, SIGNAL(overFlowChanged(bool)), m_mainTabBarWidget, SLOT(overflowChanged(bool)));

	m_mainTabBar->installEventFilter(this);
	m_pinnedTabBar->installEventFilter(this);
	m_mainTabBarWidget->installEventFilter(this);
	m_pinnedTabBarWidget->installEventFilter(this);
	m_leftContainer->installEventFilter(this);
	m_rightContainer->installEventFilter(this);
}

int ComboTabBar::addTab(const QString& text)
{
	return insertTab(-1, text);
}

int ComboTabBar::addTab(const QIcon& icon, const QString& text)
{
	return insertTab(-1, icon, text);
}

int ComboTabBar::insertTab(int index, const QString& text)
{
	return insertTab(index, QIcon(), text);
}

int ComboTabBar::insertTab(int index, const QIcon& icon, const QString& text, bool pinned)
{
	if (pinned)
		index = m_pinnedTabBar->insertTab(index, icon, text);
	else {
		index = m_mainTabBar->insertTab(index - pinnedTabsCount(), icon, text);

		if (tabsClosable()) {
			QWidget* closeButton{m_mainTabBar->tabButton(index, closeButtonPosition())};

			if ((closeButton && closeButton->objectName() != QLatin1String("tabs-closebutton"))
				|| !closeButton) {
				insertCloseButton(index + pinnedTabsCount());

				if (closeButton)
					closeButton->deleteLater();
			}
		}

		index += pinnedTabsCount();
	}

	updatePinnedTabBarVisibility();
	tabInserted(index);
	setMinimumWidths();

	return index;
}

void ComboTabBar::removeTab(int index)
{
	if (validIndex(index)) {
		setUpdatesEnabled(false);

		localTabBar(index)->removeTab(toLocalIndex(index));
		updatePinnedTabBarVisibility();
		tabRemoved(index);
		setMinimumWidths();

		setUpdatesEnabled(true);
		updateTabBars();
	}
}

void ComboTabBar::moveTab(int from, int to)
{
	if (from >= pinnedTabsCount() && to >= pinnedTabsCount())
		m_mainTabBar->moveTab(from - pinnedTabsCount(), to - pinnedTabsCount());
	else if (from < pinnedTabsCount() && to < pinnedTabsCount())
		m_pinnedTabBar->moveTab(from, to);
}

bool ComboTabBar::isTabEnabled(int index) const
{
	return localTabBar(index)->isTabEnabled(toLocalIndex(index));
}

void ComboTabBar::setTabEnabled(int index, bool enabled)
{
	localTabBar(index)->setTabEnabled(toLocalIndex(index), enabled);
}

QColor ComboTabBar::tabTextColor(int index) const
{
	return localTabBar(index)->tabTextColor(toLocalIndex(index));
}

void ComboTabBar::setTabTextColor(int index, const QColor& color)
{
	localTabBar(index)->setTabTextColor(toLocalIndex(index), color);
}

QRect ComboTabBar::tabRect(int index) const
{
	return mapFromLocalTabRet(localTabBar(index)->tabRect(toLocalIndex(index)), localTabBar(index));
}

QRect ComboTabBar::draggedTabRect() const
{
	const QRect rect{m_pinnedTabBar->draggedTabRect()};

	if (rect.isValid())
		return mapFromLocalTabRet(rect, m_pinnedTabBar);

	return mapFromLocalTabRet(m_mainTabBar->draggedTabRect(), m_mainTabBar);
}

QPixmap ComboTabBar::tabPixmap(int index) const
{
	return localTabBar(index)->tabPixmap(toLocalIndex(index));
}

int ComboTabBar::tabAt(const QPoint& position) const
{
	QWidget* widget = QApplication::widgetAt(mapToGlobal(position));

	if (!qobject_cast<TabBar*>(widget) && !qobject_cast<TabIcon*>(widget) && !qobject_cast<TabCloseButton*>(widget))
		return -1;

	if (m_pinnedTabBarWidget->geometry().contains(position))
		return m_pinnedTabBarWidget->tabAt(m_pinnedTabBarWidget->mapFromParent(position));
	else if (m_mainTabBarWidget->geometry().contains(position)) {
		int index{m_mainTabBarWidget->tabAt(m_mainTabBarWidget->mapFromParent(position))};

		if (index != -1)
			index += pinnedTabsCount();

		return index;
	}

	return -1;
}

bool ComboTabBar::emptyArea(const QPoint& position) const
{
	if (tabAt(position) != -1)
		return false;

	return qobject_cast<TabBar*>(QApplication::widgetAt(mapToGlobal(position)));
}

int ComboTabBar::mainTabBarCurentIndex() const
{
	return (m_mainTabBar->currentIndex() == -1 ? -1 : pinnedTabsCount() + m_mainTabBar->currentIndex());
}

int ComboTabBar::currentIndex() const
{
	if (m_pinnedTabBar->isActiveTabBar())
		return m_pinnedTabBar->currentIndex();
	else
		return (m_mainTabBar->currentIndex() == -1 ? -1 : pinnedTabsCount() + m_mainTabBar->currentIndex());
}

int ComboTabBar::count() const
{
	return pinnedTabsCount() + m_mainTabBar->count();
}

bool ComboTabBar::drawBase() const
{
	return m_mainTabBar->drawBase();
}

void ComboTabBar::setDrawBase(bool drawTheBase)
{
	m_mainTabBar->setDrawBase(drawTheBase);
	m_pinnedTabBar->setDrawBase(drawTheBase);
}

Qt::TextElideMode ComboTabBar::elideMode() const
{
	return m_mainTabBar->elideMode();
}

void ComboTabBar::setElideMode(Qt::TextElideMode elide)
{
	m_mainTabBar->setElideMode(elide);
	m_pinnedTabBar->setElideMode(elide);
}

QString ComboTabBar::tabText(int index) const
{
	return localTabBar(index)->tabText(toLocalIndex(index));
}

void ComboTabBar::setTabText(int index, const QString& text)
{
	localTabBar(index)->setTabText(toLocalIndex(index), text);
}

QString ComboTabBar::tabToolTip(int index) const
{
	return localTabBar(index)->tabToolTip(toLocalIndex(index));
}

void ComboTabBar::setTabToolTip(int index, const QString& tip)
{
	localTabBar(index)->setTabToolTip(toLocalIndex(index), tip);
}

bool ComboTabBar::tabsClosable() const
{
	return m_mainTabBar->tabsClosable();
}

void ComboTabBar::setTabsClosable(bool closable)
{
	if (closable == tabsClosable())
		return;

	if (closable) {
		for (int i{0}; i < m_mainTabBar->count(); ++i) {
			QWidget* closeButton{m_mainTabBar->tabButton(i, closeButtonPosition())};

			if (closeButton)
				if (closeButton->objectName() == QLatin1String("tabs-closebutton"))
					continue;

			insertCloseButton(i + pinnedTabsCount());

			if (closeButton)
				closeButton->deleteLater();
		}
	}

	m_mainTabBar->setTabsClosable(closable);
}

QWidget* ComboTabBar::tabButton(int index, QTabBar::ButtonPosition position) const
{
	return localTabBar(index)->tabButton(toLocalIndex(index), position);
}

void ComboTabBar::setTabButton(int index, QTabBar::ButtonPosition position, QWidget* widget)
{
	if (widget)
		widget->setMinimumSize(closeButtonSize());

	localTabBar(index)->setTabButton(toLocalIndex(index), position, widget);
}

QTabBar::SelectionBehavior ComboTabBar::selectionBehaviorOnRemove() const
{
	return m_mainTabBar->selectionBehaviorOnRemove();
}

void ComboTabBar::setSelectionBehaviorOnRemove(QTabBar::SelectionBehavior behavior)
{
	m_mainTabBar->setSelectionBehaviorOnRemove(behavior);
	m_pinnedTabBar->setSelectionBehaviorOnRemove(behavior);

}

bool ComboTabBar::expanding() const
{
	return m_mainTabBar->expanding();
}

void ComboTabBar::setExpanding(bool enable)
{
	m_mainTabBar->setExpanding(enable);
	m_pinnedTabBar->setExpanding(enable);
}

bool ComboTabBar::isMovable() const
{
	return m_mainTabBar->isMovable();
}

void ComboTabBar::setMovable(bool movable)
{
	m_mainTabBar->setMovable(movable);
	m_pinnedTabBar->setMovable(movable);
}

bool ComboTabBar::documentMode() const
{
	return m_mainTabBar->documentMode();
}

void ComboTabBar::setDocumentMode(bool set)
{
	m_mainTabBar->setDocumentMode(set);
	m_pinnedTabBar->setDocumentMode(set);
}

int ComboTabBar::pinnedTabsCount() const
{
	return m_pinnedTabBar->count();
}

int ComboTabBar::normalTabsCount() const
{
	return m_mainTabBar->count();
}

bool ComboTabBar::isPinned(int index) const
{
	return index >= 0 && index < pinnedTabsCount();
}

void ComboTabBar::setFocusPolicy(Qt::FocusPolicy policy)
{
	QWidget::setFocusPolicy(policy);

	m_mainTabBar->setFocusPolicy(policy);
	m_pinnedTabBar->setFocusPolicy(policy);
}

void ComboTabBar::setObjectName(const QString& name)
{
	m_mainTabBar->setObjectName(name);
	m_pinnedTabBar->setObjectName(name);
}

void ComboTabBar::setMouseTracking(bool enable)
{
	m_mainTabBarWidget->scrollArea()->setMouseTracking(enable);
	m_mainTabBarWidget->setMouseTracking(enable);
	m_mainTabBar->setMouseTracking(enable);

	m_pinnedTabBarWidget->scrollArea()->setMouseTracking(enable);
	m_pinnedTabBarWidget->setMouseTracking(enable);
	m_pinnedTabBar->setMouseTracking(enable);

	QWidget::setMouseTracking(enable);
}

void ComboTabBar::insertCloseButton(int index)
{
	index -= pinnedTabsCount();

	if (index < 0)
		return;

	QAbstractButton* closeButton{new TabCloseButton(this)};
	closeButton->setFixedSize(closeButtonSize());
	closeButton->setToolTip(m_closeButtonsToolTip);

	connect(closeButton, &QAbstractButton::clicked, this, &ComboTabBar::closeTabFromButton);

	m_mainTabBar->setTabButton(index, closeButtonPosition(), closeButton);
}

void ComboTabBar::setCloseButtonsToolTip(const QString& tip)
{
	m_closeButtonsToolTip = tip;
}

QTabBar::ButtonPosition ComboTabBar::iconButtonPosition() const
{
	return (closeButtonPosition() == QTabBar::RightSide ? QTabBar::LeftSide : QTabBar::RightSide);
}

QSize ComboTabBar::iconButtonSize() const
{
	QSize size{closeButtonSize()};

	size.setWidth(qMax(16, size.width()));
	size.setHeight(qMax(16, size.height()));

	return size;
}

QTabBar::ButtonPosition ComboTabBar::closeButtonPosition() const
{
	return static_cast<QTabBar::ButtonPosition>(style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, nullptr, this));
}

QSize ComboTabBar::closeButtonSize() const
{
	int width{style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, nullptr, this)};
	int height{style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, nullptr, this)};

	return QSize(width, height);
}

bool ComboTabBar::validIndex(int index) const
{
	return (index >= 0 && index < count());
}

void ComboTabBar::setCurrentNextEnabledIndex(int offset)
{
	for (int index{currentIndex() + offset}; validIndex(index); index += offset) {
		if (isTabEnabled(index)) {
			setCurrentIndex(index);
			break;
		}
	}
}

bool ComboTabBar::usesScrollButtons() const
{
	return m_mainTabBarWidget->usesScrollButtons();
}

void ComboTabBar::setUsesScrollButtons(bool useButtons)
{
	m_mainTabBarWidget->setUsesScrollButtons(useButtons);
}

void ComboTabBar::showDropIndicator(int index, DropIndicatorPosition position)
{
	clearDropIndicator();
	localTabBar(index)->showDropIndicator(toLocalIndex(index), position);
}

void ComboTabBar::clearDropIndicator()
{
	m_mainTabBar->clearDropIndicator();
	m_pinnedTabBar->clearDropIndicator();
}

bool ComboTabBar::isDragInProgress() const
{
	return (m_mainTabBar->isDragInProgress() || m_pinnedTabBar->isDragInProgress());
}

bool ComboTabBar::isScrollInProgress() const
{
	return (m_mainTabBarWidget->scrollBar()->isScrolling() || m_pinnedTabBarWidget->scrollBar()->isScrolling());
}

bool ComboTabBar::isMainBarOverflowed() const
{
	return m_mainBarOverFlowed;
}

int ComboTabBar::cornerWidth(Qt::Corner corner) const
{
	if (corner == Qt::TopLeftCorner) {
		return m_leftContainer->width();
	}
	else if (corner == Qt::TopRightCorner) {
		return m_rightContainer->width();
	}

	qFatal("ComboTabBar::cornerWidth Only TopLeft and TopRight corners are implemented!");
	return -1;
}

void ComboTabBar::addCornerWidget(QWidget* widget, Qt::Corner corner)
{
	if (corner == Qt::TopLeftCorner) {
		m_leftLayout->addWidget(widget);
	}
	else if (corner == Qt::TopRightCorner) {
		m_rightLayout->addWidget(widget);
	}
	else {
		qFatal("ComboTabBar::addCornerWidget Only TopLeft and TopRight corners are implemented!");
	}
}

void ComboTabBar::setUpLayout()
{
	/*int height{qMax(m_mainTabBar->height(), m_pinnedTabBar->height())};

	if (height < 1)
		height = qMax(m_mainTabBar->sizeHint().height(), m_pinnedTabBar->sizeHint().height());

	height = qMax(5, height);

	setFixedHeight(height);

	m_leftContainer->setFixedHeight(height);
	m_rightContainer->setFixedHeight(height);
	m_mainTabBarWidget->setUpLayout();
	m_pinnedTabBarWidget->setUpLayout();

	setMinimumWidths();

	if (isVisible() && height > 5) {
		m_pinnedTabBar->setFixedHeight(height);
		m_mainTabBar->setFixedHeight(height);
	}*/

	int height{qMax(m_mainTabBar->height(), m_pinnedTabBar->height())};
	height -= 2;

	height = qMax(5, height);

	setFixedHeight(height);
	m_pinnedTabBar->setFixedHeight(height);
	m_leftContainer->setFixedHeight(height);
	m_rightContainer->setFixedHeight(height);
	m_mainTabBarWidget->setUpLayout();
	m_pinnedTabBarWidget->setUpLayout();

	setMinimumWidths();

	if (isVisible() && m_mainTabBar->count() > 0) {
		m_pinnedTabBar->setFixedHeight(m_mainTabBar->sizeHint().height());
		m_mainTabBar->setFixedHeight(m_mainTabBar->sizeHint().height());
	}
}

void ComboTabBar::ensureVisible(int index, int xmargin)
{
	if (index == -1)
		index = currentIndex();

	if (index < pinnedTabsCount()) {
		if (xmargin == -1)
			xmargin = qMax(20, comboTabBarPixelMetric(PinnedTabWidth));

		m_pinnedTabBarWidget->ensureVisible(index, xmargin);
	}
	else {
		if (xmargin == -1)
			xmargin = comboTabBarPixelMetric(OverflowedTabWidth);

		index -= pinnedTabsCount();
		m_mainTabBarWidget->ensureVisible(index, xmargin);
	}
}

void ComboTabBar::setCurrentIndex(int index)
{
	return localTabBar(index)->setCurrentIndex(toLocalIndex(index));
}

int ComboTabBar::mainTabBarWidth() const
{
	return m_mainTabBar->width();
}

int ComboTabBar::pinTabBarWidth() const
{
	return m_pinnedTabBarWidget->isHidden() ? 0 : m_pinnedTabBarWidget->width();
}

bool ComboTabBar::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == m_mainTabBar && event->type() == QEvent::Resize) {
		QResizeEvent* evnt = dynamic_cast<QResizeEvent*>(event);

		if (evnt->oldSize().height() != evnt->size().height())
			setUpLayout();
	}

	if (event->type() == QEvent::Wheel) {
		wheelEvent(dynamic_cast<QWheelEvent*>(event));
		return true;
	}

	return QWidget::eventFilter(obj, event);
}

bool ComboTabBar::event(QEvent* event)
{
	const bool res{QWidget::event(event)};

	switch (event->type()) {
	case QEvent::ToolTip:
		if (!isDragInProgress() && !isScrollInProgress()) {
			int index{tabAt(mapFromGlobal(QCursor::pos()))};
			if (index >= 0)
				QToolTip::showText(QCursor::pos(), tabToolTip(index));
		}
		break;
	case QEvent::Resize:
		ensureVisible();
		break;
	case QEvent::Show:
		if (!event->spontaneous())
			QTimer::singleShot(0, this, &ComboTabBar::setUpLayout);
		break;
	case QEvent::Enter:
	case QEvent::Leave:
		QTimer::singleShot(100, this, &ComboTabBar::updateTabBars);
		break;
	default:
		break;
	}

	return res;
}

void ComboTabBar::wheelEvent(QWheelEvent* event)
{
	event->accept();

	if (m_mainTabBarWidget->underMouse()) {
		if (m_mainTabBarWidget->isOverflowed())
			m_mainTabBarWidget->scrollByWheel(event);
		else if (m_pinnedTabBarWidget->isOverflowed())
			m_pinnedTabBarWidget->scrollByWheel(event);
	}
	else if (m_pinnedTabBarWidget->underMouse()) {
		if (m_pinnedTabBarWidget->isOverflowed())
			m_pinnedTabBarWidget->scrollByWheel(event);
		else if (m_mainTabBarWidget->isOverflowed())
			m_mainTabBarWidget->scrollByWheel(event);
	}

	if (!m_mainTabBarWidget->isOverflowed() && !m_pinnedTabBarWidget->isOverflowed())
		setCurrentNextEnabledIndex(event->delta() > 0 ? -1 : 1);
}

void ComboTabBar::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
//	Q_UNUSED(event);
//
//	QStyleOption option{};
//	QPainter painter{this};
//	option.init(this);
//
//	style()->drawPrimitive(QStyle::PE_Widget, &option, &painter, this);
//
//#ifndef Q_OS_MAC
//
//	QStyleOptionTabBarBase optionTabBase{};
//	TabBar::initStyleBaseOption(&optionTabBase, m_mainTabBar, size());
//
//	optionTabBase.rect.setX(m_leftContainer->x());
//	optionTabBase.rect.setWidth(m_leftContainer->width());
//	style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &optionTabBase, &painter);
//
//	optionTabBase.rect.setX(m_rightContainer->x());
//	optionTabBase.rect.setWidth(m_rightContainer->width());
//	style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &optionTabBase, &painter);
//
//	if (m_mainBarOverFlowed) {
//		const int scrollButtonWidth{m_mainTabBarWidget->scrollButtonsWidth()};
//
//		optionTabBase.rect.setX(m_mainTabBarWidget->x());
//		optionTabBase.rect.setWidth(scrollButtonWidth);
//		style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &optionTabBase, &painter);
//
//		optionTabBase.rect.setX(m_mainTabBarWidget->x() + m_mainTabBarWidget->width() - scrollButtonWidth);
//		optionTabBase.rect.setWidth(scrollButtonWidth);
//		style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &optionTabBase, &painter);
//	}
//
//	if (normalTabsCount() == 0) {
//		optionTabBase.rect.setX(m_mainTabBarWidget->x());
//		optionTabBase.rect.setWidth(m_mainTabBarWidget->width());
//		style()->drawPrimitive(QStyle::PE_FrameTabBarBase, &optionTabBase, &painter);
//	}
//
//#endif
}

int ComboTabBar::comboTabBarPixelMetric(SizeType sizeType) const
{
	switch (sizeType) {
	case ExtraReservedWidth:
		return 0;
	case NormalTabMaximumWidth:
		return 150;
	case ActiveTabMinimumWidth:
	case NormalTabMinimumWidth:
	case OverflowedTabWidth:
		return 100;
	case PinnedTabWidth:
		return 30;
	default:
		break;
	}

	return -1;
}

QSize ComboTabBar::tabSizeHint(int index, bool fast) const
{
	Q_UNUSED(fast);

	return localTabBar(index)->baseClassTabSizeHint(toLocalIndex(index));
}

void ComboTabBar::tabInserted(int index)
{
	Q_UNUSED(index);
}

void ComboTabBar::tabRemoved(int index)
{
	Q_UNUSED(index);
}

void ComboTabBar::setMinimumWidths()
{
	if (!isVisible() || comboTabBarPixelMetric(PinnedTabWidth) < 0)
		return;

	const int tabBarsSpacing{3};
	int pinnedTabBarWidth{pinnedTabsCount() * comboTabBarPixelMetric(PinnedTabWidth)};

	m_pinnedTabBar->setMinimumWidth(pinnedTabBarWidth);
	m_pinnedTabBarWidget->setFixedWidth(pinnedTabBarWidth + tabBarsSpacing);

	int mainTabBarWidth{comboTabBarPixelMetric(NormalTabMinimumWidth) * (m_mainTabBar->count() - 1)
						+ comboTabBarPixelMetric(ActiveTabMinimumWidth) + comboTabBarPixelMetric(ExtraReservedWidth)};
	int realTabBarWidth{mainTabBarWidth + m_pinnedTabBarWidget->width() + cornerWidth(Qt::TopLeftCorner)
						+ cornerWidth(Qt::TopRightCorner)};

	if (realTabBarWidth <= width()) {
		if (m_mainBarOverFlowed) {
			m_mainBarOverFlowed = false;
			QTimer::singleShot(0, this, &ComboTabBar::emitOverFlowChanged);
		}

		m_mainTabBar->useFastTabSizeHint(false);
		m_mainTabBar->setMinimumWidth(mainTabBarWidth);
	}
	else {
		if (!m_mainBarOverFlowed) {
			m_mainBarOverFlowed = true;
			QTimer::singleShot(0, this, &ComboTabBar::emitOverFlowChanged);
		}

		m_mainTabBar->useFastTabSizeHint(true);
		m_mainTabBar->setMinimumWidth(m_mainTabBar->count() * comboTabBarPixelMetric(OverflowedTabWidth));
	}
}

void ComboTabBar::sCurrentChanged(int index)
{
	if (m_blockCurrentChangedSignal || count() < 1)
		return;

	if (sender() == m_pinnedTabBar) {
		if (index == -1 && m_mainTabBar->count() > 0) {
			m_mainTabBar->setActiveTabBar(true);
			m_pinnedTabBar->setActiveTabBar(false);
			emit currentChanged(pinnedTabsCount());
		}
		else {
			m_pinnedTabBar->setActiveTabBar(true);
			m_mainTabBar->setActiveTabBar(false);
			emit currentChanged(index);
		}
	}
	else {
		if (index == -1 && pinnedTabsCount() > 0) {
			m_pinnedTabBar->setActiveTabBar(true);
			m_mainTabBar->setActiveTabBar(false);
			emit currentChanged(pinnedTabsCount() - 1);
		}
		else {
			m_mainTabBar->setActiveTabBar(true);
			m_pinnedTabBar->setActiveTabBar(false);
			emit currentChanged(index + pinnedTabsCount());
		}
	}
}

void ComboTabBar::sTabCloseRequested(int index)
{
	if (sender() == m_pinnedTabBar)
		emit tabCloseRequested(index);
	else
		emit tabCloseRequested(index + pinnedTabsCount());
}

void ComboTabBar::sTabMoved(int from, int to)
{
	if (sender() == m_pinnedTabBar)
		emit tabMoved(from, to);
	else
		emit tabMoved(from + pinnedTabsCount(), to + pinnedTabsCount());
}

void ComboTabBar::closeTabFromButton()
{
	QWidget* button{qobject_cast<QWidget*>(sender())};
	int tabToClose{-1};

	for (int i{0}; i < m_mainTabBar->count(); ++i) {
		if (m_mainTabBar->tabButton(i, closeButtonPosition()) == button) {
			tabToClose = i;
			break;
		}
	}

	if (tabToClose != -1)
		emit tabCloseRequested(tabToClose + pinnedTabsCount());
}

void ComboTabBar::updateTabBars()
{
	m_mainTabBar->update();
	m_pinnedTabBar->update();
}

void ComboTabBar::emitOverFlowChanged()
{
	if (m_mainBarOverFlowed != m_lastAppliedOverflow) {
		emit overFlowChanged(m_mainBarOverFlowed);
		m_lastAppliedOverflow = m_mainBarOverFlowed;
	}
}

TabBar* ComboTabBar::mainTabBar() const
{
	return m_mainTabBar;
}

TabBar* ComboTabBar::localTabBar(int index) const
{
	if (index < 0 || index >= pinnedTabsCount())
		return m_mainTabBar;
	else
		return m_pinnedTabBar;
}

int ComboTabBar::toLocalIndex(int globalIndex) const
{
	if (globalIndex < 0)
		return -1;

	if (globalIndex >= pinnedTabsCount())
		return globalIndex - pinnedTabsCount();
	else
		return globalIndex;
}

QRect ComboTabBar::mapFromLocalTabRet(const QRect& rect, QWidget* tabBar) const
{
	if (!rect.isValid())
		return rect;

	QRect r{rect};

	if (tabBar == m_mainTabBar) {
		r.moveLeft(r.x() + mapFromGlobal(m_mainTabBar->mapToGlobal(QPoint(0, 0))).x());
		QRect widgetRect{m_mainTabBarWidget->scrollArea()->viewport()->rect()};
		widgetRect.moveLeft(widgetRect.x() + mapFromGlobal(m_mainTabBarWidget->scrollArea()->viewport()->mapToGlobal(QPoint(0, 0))).x());
		r = r.intersected(widgetRect);
	}
	else {
		r.moveLeft(r.x() + mapFromGlobal(m_pinnedTabBar->mapToGlobal(QPoint(0, 0))).x());
		QRect widgetRect{m_pinnedTabBarWidget->scrollArea()->viewport()->rect()};
		widgetRect.moveLeft(widgetRect.x() + mapFromGlobal(m_pinnedTabBarWidget->scrollArea()->viewport()->mapToGlobal(QPoint(0, 0))).x());
		r = r.intersected(widgetRect);
	}

	return r;
}

void ComboTabBar::updatePinnedTabBarVisibility()
{
	m_pinnedTabBarWidget->setVisible(pinnedTabsCount() > 0);
}

}
