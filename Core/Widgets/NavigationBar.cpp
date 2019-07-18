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

#include "NavigationBar.hpp"

#include <QStyle>

#include <QWebEngine/WebHistory.hpp>

#include "Utils/ToolButton.hpp"
#include "Utils/Settings.hpp"

//#include "History/HistoryDialog.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Web/WebPage.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/TitleBar.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"

namespace Sn
{
NavigationToolBar::NavigationToolBar(BrowserWindow* window, QStackedWidget* addressBars) :
	QWidget(window),
	m_window(window)
{
	setObjectName(QStringLiteral("navigationbar"));
	QSize iconSize{28, 28};

	setMaximumHeight(100);

	m_layout = new QHBoxLayout(this);
	m_layout->setMargin(0);
	m_layout->setSpacing(0);

	m_buttonBack = new ToolButton(this);
	m_buttonBack->setObjectName("navigation-button-back");
	m_buttonBack->setToolTip(tr("Back"));
	m_buttonBack->setIcon(Application::getAppIcon("arrow-left"));
	m_buttonBack->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonBack->setToolBarButtonLook(true);
	m_buttonBack->setAutoRaise(true);
	m_buttonBack->setEnabled(false);
	m_buttonBack->setFocusPolicy(Qt::NoFocus);

	m_buttonForward = new ToolButton(this);
	m_buttonForward->setObjectName("navigation-button-forward");
	m_buttonForward->setToolTip(tr("Forward"));
	m_buttonForward->setIcon(Application::getAppIcon("arrow-right"));
	m_buttonForward->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonForward->setToolBarButtonLook(true);
	m_buttonForward->setAutoRaise(true);
	m_buttonForward->setEnabled(false);
	m_buttonForward->setFocusPolicy(Qt::NoFocus);

	m_buttonHome = new ToolButton(this);
	m_buttonHome->setObjectName("navigation-button-home");
	m_buttonHome->setToolTip(tr("Home"));
	m_buttonHome->setIcon(Application::getAppIcon("home"));
	m_buttonHome->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonHome->setToolBarButtonLook(true);
	m_buttonHome->setAutoRaise(true);
	m_buttonHome->setFocusPolicy(Qt::NoFocus);

	m_buttonViewBookmarks = new ToolButton(this);
	m_buttonViewBookmarks->setObjectName("navigation-button-view-bookmarks");
	m_buttonViewBookmarks->setToolTip(tr("View Bookmarks"));
	m_buttonViewBookmarks->setIcon(Application::getAppIcon("bookmarks"));
	m_buttonViewBookmarks->setMinimumSize(iconSize);
	m_buttonViewBookmarks->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonViewBookmarks->setToolBarButtonLook(true);
	m_buttonViewBookmarks->setAutoRaise(true);
	m_buttonViewBookmarks->setFocusPolicy(Qt::NoFocus);

	m_buttonAddBookmark = new ToolButton(this);
	m_buttonAddBookmark->setObjectName("navigation-button-add-bookmark");
	m_buttonAddBookmark->setToolTip(tr("Add Bookmark"));
	m_buttonAddBookmark->setIcon(Application::getAppIcon("add-bookmark"));
	m_buttonAddBookmark->setMinimumSize(iconSize);
	m_buttonAddBookmark->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonAddBookmark->setToolBarButtonLook(true);
	m_buttonAddBookmark->setAutoRaise(true);
	m_buttonAddBookmark->setFocusPolicy(Qt::NoFocus);

	m_buttonViewHistory = new ToolButton(this);
	m_buttonViewHistory->setObjectName("navigation-button-view-history");
	m_buttonViewHistory->setToolTip(tr("View History"));
	m_buttonViewHistory->setIcon(Application::getAppIcon("history"));
	m_buttonViewHistory->setMinimumSize(iconSize);
	m_buttonViewHistory->setToolButtonStyle(Qt::ToolButtonIconOnly);
	//m_buttonViewHistory->setToolBarButtonLook(true);
	m_buttonViewHistory->setAutoRaise(true);
	m_buttonViewHistory->setFocusPolicy(Qt::NoFocus);

	m_extensionWidget = new QWidget(this);

	m_extensionLayout = new QHBoxLayout(m_extensionWidget);
	m_extensionLayout->setContentsMargins(0, 0, 0, 0);
	m_extensionLayout->setSpacing(0);

	m_bookmarksHistoryWidget = new QWidget(this);

	m_bookmarksHistoryLayout = new QHBoxLayout(m_bookmarksHistoryWidget);
	m_bookmarksHistoryLayout->setContentsMargins(0, 0, 0, 0);
	m_bookmarksHistoryLayout->setSpacing(0);
	m_bookmarksHistoryLayout->addWidget(m_buttonViewBookmarks);
	m_bookmarksHistoryLayout->addWidget(m_buttonAddBookmark);
	m_bookmarksHistoryLayout->addWidget(m_buttonViewHistory);

	m_layout->addWidget(m_buttonBack);
	m_layout->addWidget(m_buttonForward);
	m_layout->addWidget(m_buttonHome);
	m_layout->addWidget(addressBars);
	m_layout->addWidget(m_extensionWidget);
	m_layout->addWidget(m_bookmarksHistoryWidget);

	connect(m_buttonBack, &ToolButton::clicked, this, &NavigationToolBar::goBack);
	connect(m_buttonBack, &ToolButton::middleMouseClicked, this, &NavigationToolBar::goBackInNewTab);
	connect(m_buttonBack, &ToolButton::controlClicked, this, &NavigationToolBar::goBackInNewTab);
	connect(m_buttonForward, &ToolButton::clicked, this, &NavigationToolBar::goForward);
	connect(m_buttonForward, &ToolButton::middleMouseClicked, this, &NavigationToolBar::goForwardInNewTab);
	connect(m_buttonForward, &ToolButton::controlClicked, this, &NavigationToolBar::goForwardInNewTab);
	connect(m_buttonHome, &ToolButton::clicked, this, &NavigationToolBar::goHome);
	connect(m_buttonHome, &ToolButton::middleMouseClicked, this, &NavigationToolBar::goHomeInNewTab);
	connect(m_buttonHome, &ToolButton::controlClicked, this, &NavigationToolBar::goHomeInNewTab);

	connect(m_buttonViewBookmarks, &ToolButton::clicked, this, &NavigationToolBar::showBookmarksDialog);
	connect(m_buttonAddBookmark, &ToolButton::clicked, this, &NavigationToolBar::showAddBookmarkDialog);
	connect(m_buttonViewHistory, &ToolButton::clicked, this, &NavigationToolBar::showHistoryDialog);

	foreach(QWidget* widget, Application::instance()->plugins()->navigationBarButton(m_window))
	{
		addExtensionAction(widget);
	}
}

void NavigationToolBar::loadSettings()
{
	if (Application::instance()->hideBookmarksHistoryActions())
		hideBookmarksHistory();
	else
		showBookmarksHistory();

	if (Application::instance()->hideToolbarControls())
		hideControls();
	else
		showControls();
}

int NavigationToolBar::layoutMargin() const
{
	return m_layout->margin();
}

void NavigationToolBar::setLayoutMargin(int margin)
{
	m_layout->setMargin(margin);
}

int NavigationToolBar::layoutSpacing() const
{
	return m_layout->spacing();
}

void NavigationToolBar::setLayoutSpacing(int layoutSpacing)
{
	m_layout->setSpacing(layoutSpacing);
}

void NavigationToolBar::hideBookmarksHistory()
{
	m_bookmarksHistoryWidget->hide();
}

void NavigationToolBar::showBookmarksHistory()
{
	m_bookmarksHistoryWidget->show();
}

void NavigationToolBar::hideControls()
{
	m_buttonBack->hide();
	m_buttonForward->hide();
	m_buttonHome->hide();
}

void NavigationToolBar::showControls()
{
	m_buttonBack->show();
	m_buttonForward->show();
	m_buttonHome->show();
}

void NavigationToolBar::addExtensionAction(QWidget* widget)
{
	if (!widget)
		return;

	m_extensionLayout->addWidget(widget);
}

void NavigationToolBar::refreshBackForwardButtons()
{
	if (Application::instance()->isClosing() || !m_window->tabWidget()->webTab())
		return;

	Engine::WebHistory* history{m_window->tabWidget()->webTab()->webView()->page()->history()};
	m_buttonBack->setEnabled(history->canGoBack());
	m_buttonForward->setEnabled(history->canGoForward());
}

void NavigationToolBar::goBack()
{
	m_window->tabWidget()->webTab()->webView()->back();
}

void NavigationToolBar::goBackInNewTab()
{
	m_window->tabWidget()->addView(m_window->tabWidget()->webTab()->webView()->history()->backUrl());
}

void NavigationToolBar::goForward()
{
	m_window->tabWidget()->webTab()->webView()->forward();
}

void NavigationToolBar::goForwardInNewTab()
{
	m_window->tabWidget()->addView(m_window->tabWidget()->webTab()->webView()->history()->forwardUrl());
}

void NavigationToolBar::goHome()
{
	m_window->tabWidget()->webTab()->sGoHome();
}

void NavigationToolBar::goHomeInNewTab()
{
	// TODO: go home in new tab
}

void NavigationToolBar::showBookmarksDialog()
{
	m_window->tabWidget()->openBookmarksDialog();
}

void NavigationToolBar::showAddBookmarkDialog()
{
	m_window->tabWidget()->window()->bookmarkPage();
}

void NavigationToolBar::showHistoryDialog()
{
	m_window->tabWidget()->openHistoryDialog();
}

void NavigationToolBar::contextMenuRequested(const QPoint& pos)
{
	Q_UNUSED(pos);
	// TODO: do context menu for navigation tool bar
}
}
