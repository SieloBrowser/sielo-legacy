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

#include "Widgets/Tab/TabStackedWidget.hpp"

#include <QApplication>

#include <QTimer>

#include "Utils/Settings.hpp"
#include "Utils/SideBarManager.hpp"

#include "Widgets/NavigationBar.hpp"
#include "Widgets/Tab/ComboTabBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"

namespace Sn {
TabStackedWidget::TabStackedWidget(QWidget* parent) :
	QWidget(parent),
	m_currentIndex(-1),
	m_previousIndex(-1),
	m_sideBarManager(new SideBarManager(this))
{
	m_layout = new QVBoxLayout(this);
	m_layout->setSpacing(0);
	m_layout->setContentsMargins(0, 0, 0, 0);

	m_stack = new QStackedWidget(this);
	m_stack->setObjectName("tabwidget-stack");

	m_splitter = new QSplitter(this);
	m_splitter->setObjectName("sidebar-splitter");

	m_splitter->addWidget(m_stack);
	m_splitter->setCollapsible(0, false);

	m_layout->addWidget(m_splitter);

	connect(m_stack, &QStackedWidget::widgetRemoved, this, &TabStackedWidget::tabWasRemoved);
}

TabStackedWidget::~TabStackedWidget()
{
	// Empty
}

SideBar* TabStackedWidget::addSideBar()
{
	if (m_sideBar)
		return m_sideBar.data();

	m_sideBar = new SideBar(m_sideBarManager, dynamic_cast<TabWidget*>(this));

	m_splitter->insertWidget(0, m_sideBar.data());
	m_splitter->setCollapsible(0, false);
	m_splitter->setSizes({124, 2000});

	return m_sideBar.data();
}

void TabStackedWidget::createSideBarsMenu(QMenu* menu)
{
	m_sideBarManager->createMenu(menu);
}

void TabStackedWidget::saveSideBarSettings()
{
	Settings settings{};

	settings.beginGroup("SideBars");

	settings.setValue("Active", sideBarManager()->activeSideBar());
}

void TabStackedWidget::setTabBar(ComboTabBar* tab)
{
	Q_ASSERT(tab);

	if (tab->parentWidget() != this) {
		tab->setParent(this);
		tab->show();
	}

	delete m_comboTabBar;

	m_dirtyTabBar = true;
	m_comboTabBar = tab;
	setFocusProxy(m_comboTabBar);

	connect(m_comboTabBar, &ComboTabBar::currentChanged, this, &TabStackedWidget::showTab);
	connect(m_comboTabBar, &ComboTabBar::tabMoved, this, &TabStackedWidget::tabWasMoved);
	connect(m_comboTabBar, &ComboTabBar::overFlowChanged, this, &TabStackedWidget::setUpLayout);

	if (m_comboTabBar->tabsClosable())
		connect(m_comboTabBar, &ComboTabBar::tabCloseRequested, this, &TabStackedWidget::tabCloseRequested);

	setDocumentMode(m_comboTabBar->documentMode());

	m_comboTabBar->installEventFilter(this);
	setUpLayout();
}

void TabStackedWidget::setNavigationToolBar(NavigationToolBar* navigationToolBar)
{
	m_layout->insertWidget(0, navigationToolBar);
	setUpLayout();
}

bool TabStackedWidget::documentMode() const
{
	return m_comboTabBar->documentMode();
}

void TabStackedWidget::setDocumentMode(bool enable)
{
	m_comboTabBar->setDocumentMode(enable);
	m_comboTabBar->setDrawBase(enable);
	m_comboTabBar->setExpanding(!enable);
}

int TabStackedWidget::addTab(QWidget* widget, const QString& label, bool pinned)
{
	return insertTab(-1, widget, label, pinned);
}

int TabStackedWidget::insertTab(int index, QWidget* widget, const QString& label, bool pinned)
{
	if (!widget)
		return -1;

	if (pinned) {
		index = index < 0 ? m_comboTabBar->pinnedTabsCount() : qMin(index, m_comboTabBar->pinnedTabsCount());
		index = m_stack->insertWidget(index, widget);
		m_comboTabBar->insertTab(index, QIcon(), label, true);
	}
	else {
		index = index < 0 ? -1 : qMax(index, m_comboTabBar->pinnedTabsCount());
		index = m_stack->insertWidget(index, widget);
		m_comboTabBar->insertTab(index, QIcon(), label, false);
	}

	if (m_previousIndex >= index)
		++m_previousIndex;
	if (m_currentIndex >= index)
		++m_currentIndex;

	QTimer::singleShot(10, this, &TabStackedWidget::setUpLayout);

	return index;
}

QString TabStackedWidget::tabText(int index) const
{
	return m_comboTabBar->tabText(index);
}

void TabStackedWidget::setTabText(int index, const QString& label)
{
	m_comboTabBar->setTabText(index, label);
}

QString TabStackedWidget::tabToolTip(int index) const
{
	return m_comboTabBar->tabToolTip(index);
}

void TabStackedWidget::setTabToolTip(int index, const QString& tip)
{
	m_comboTabBar->setTabToolTip(index, tip);
}

int TabStackedWidget::pinUnPinTab(int index, const QString& title)
{
	QWidget* widget{m_stack->widget(index)};
	QWidget* currentWidget{m_stack->currentWidget()};

	if (!widget || !currentWidget)
		return -1;

	bool makePinned = index >= m_comboTabBar->pinnedTabsCount();
	QWidget* button = m_comboTabBar->tabButton(index, m_comboTabBar->iconButtonPosition());

	m_comboTabBar->m_blockCurrentChangedSignal = true;
	m_comboTabBar->setTabButton(index, m_comboTabBar->iconButtonPosition(), nullptr);
	m_stack->removeWidget(widget);

	int newIndex{insertTab(makePinned ? 0 : m_comboTabBar->pinnedTabsCount(), widget, title, makePinned)};

	m_comboTabBar->setTabButton(newIndex, m_comboTabBar->iconButtonPosition(), button);
	m_comboTabBar->m_blockCurrentChangedSignal = false;

	setCurrentWidget(currentWidget);

	emit pinStateChanged(newIndex, makePinned);

	return newIndex;
}

void TabStackedWidget::removeTab(int index)
{
	if (QWidget* widget = m_stack->widget(index)) {
		if (index == currentIndex() && count() > 1)
			selectTabOnRemove();

		m_stack->removeWidget(widget);
	}
}

void TabStackedWidget::moveTab(int from, int to)
{
	m_comboTabBar->moveTab(from, to);
}

int TabStackedWidget::currentIndex() const
{
	return m_comboTabBar->currentIndex();
}

int TabStackedWidget::indexOf(QWidget* widget) const
{
	return m_stack->indexOf(widget);
}

int TabStackedWidget::count() const
{
	return m_comboTabBar->count();
}

QWidget* TabStackedWidget::currentWidget() const
{
	return m_stack->currentWidget();
}

QWidget* TabStackedWidget::widget(int index) const
{
	return m_stack->widget(index);
}

void TabStackedWidget::setCurrentIndex(int index)
{
	m_comboTabBar->setCurrentIndex(index);
}

void TabStackedWidget::setCurrentWidget(QWidget* widget)
{
	m_comboTabBar->setCurrentIndex(indexOf(widget));
}

void TabStackedWidget::setUpLayout()
{
	int count = m_stack->count();

	if (!m_comboTabBar->isVisible()) {
		m_dirtyTabBar = true;
		return;
	}

	m_comboTabBar->setElideMode(m_comboTabBar->elideMode());
	m_dirtyTabBar = false;
}

bool TabStackedWidget::eventFilter(QObject* obj, QEvent* event)
{
	if (m_dirtyTabBar & (obj == m_comboTabBar) && (event->type() == QEvent::Show))
		setUpLayout();

	return false;
}

void TabStackedWidget::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Tab || event->key() == Qt::Key_Backtab) && count() > 1
		&& event->modifiers() & Qt::ControlModifier) {
		int pageCount{count()};
		int page{currentIndex()};
		int dx{(event->key() == Qt::Key_Backtab || event->modifiers() & Qt::ShiftModifier) ? -1 : 1};

		for (int pass{0}; pass < pageCount; ++pass) {
			page += dx;

			if (page < 0)
				page = count() - 1;
			else if (page >= pageCount)
				page = 0;

			if (m_comboTabBar->isTabEnabled(page)) {
				setCurrentIndex(page);
				break;
			}
		}

		if (!QApplication::focusWidget())
			m_comboTabBar->setFocus();
	}
	else
		event->ignore();
}

void TabStackedWidget::showTab(int index)
{
	if (validIndex(index))
		m_stack->setCurrentIndex(index);

	m_previousIndex = m_currentIndex;
	m_currentIndex = index;

	emit currentChanged(index);
}

void TabStackedWidget::tabWasMoved(int from, int to)
{
	m_stack->blockSignals(true);

	QWidget* widget{m_stack->widget(from)};
	m_stack->removeWidget(widget);
	m_stack->insertWidget(to, widget);

	m_stack->blockSignals(false);
}

void TabStackedWidget::tabWasRemoved(int index)
{
	if (m_previousIndex == index)
		m_previousIndex = -1;
	else if (m_previousIndex > index)
		--m_previousIndex;

	if (m_currentIndex == index)
		m_currentIndex = -1;
	else if (m_currentIndex > index)
		--m_currentIndex;

	m_comboTabBar->removeTab(index);
}

bool TabStackedWidget::validIndex(int index) const
{
	return (index < m_stack->count() && index >= 0);
}

void TabStackedWidget::selectTabOnRemove()
{
	Q_ASSERT(count() > 1);

	int index{-1};

	switch (m_comboTabBar->selectionBehaviorOnRemove()) {
	case QTabBar::SelectPreviousTab:
		if (validIndex(m_previousIndex)) {
			index = m_previousIndex;
			break;
		}
	case QTabBar::SelectLeftTab:
		index = currentIndex() - 1;
		if (!validIndex(index))
			index = 1;
		break;
	case QTabBar::SelectRightTab:
		index = currentIndex() + 1;
		if (!validIndex(index))
			index = currentIndex() - 1;
		break;
	default:
		break;
	}

	Q_ASSERT(validIndex(index));
	setCurrentIndex(index);
}
}