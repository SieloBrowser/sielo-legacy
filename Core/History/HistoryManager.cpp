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

#include "HistoryManager.hpp"

#include <QClipboard>

#include <QMessageBox>

#include "History/History.hpp"
#include "History/HistoryTreeView.hpp"

#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{
HistoryManager::HistoryManager(BrowserWindow* window, QWidget* parent) :
	QDialog(parent),
	m_window(window)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowIcon(Application::getAppIcon("history"));
	setWindowTitle(tr("History"));

	setupUI();

	connect(m_view, &HistoryTreeView::urlActivated, this, &HistoryManager::urlActivated);
	connect(m_view, &HistoryTreeView::urlCtrlActivated, this, &HistoryManager::urlCtrlActivated);
	connect(m_view, &HistoryTreeView::urlShiftActivated, this, &HistoryManager::urlShiftActivated);
	connect(m_view, &HistoryTreeView::contextMenuRequested, this, &HistoryManager::createContextMenu);

	connect(m_search, &QLineEdit::textChanged, this, &HistoryManager::search);
	connect(m_clearButton, &QPushButton::clicked, m_view, &HistoryTreeView::removeSelectedItems);
	connect(m_deleteAllButton, &QPushButton::clicked, this, &HistoryManager::clearHistory);

	m_view->setFocus();
}

void HistoryManager::setMainWindow(BrowserWindow* window)
{
	if (window)
		m_window = window;
}

void HistoryManager::search(const QString& searchText)
{
	m_view->search(searchText);
}

void HistoryManager::urlActivated(const QUrl& url)
{
	openUrl(url);
}

void HistoryManager::urlCtrlActivated(const QUrl& url)
{
	openUrlInNewTab(url);
}

void HistoryManager::urlShiftActivated(const QUrl& url)
{
	openUrlInNewWindow(url);
}

void HistoryManager::openUrl(const QUrl& url)
{
	const QUrl u{!url.isEmpty() ? url : m_view->selectedUrl()};

	m_window->tabWidget()->webTab()->load(u);
}

void HistoryManager::openUrlInNewTab(const QUrl& url)
{
	const QUrl u{!url.isEmpty() ? url : m_view->selectedUrl()};

	m_window->tabWidget()->addView(u);
}

void HistoryManager::openUrlInNewWindow(const QUrl& url)
{
	const QUrl u{!url.isEmpty() ? url : m_view->selectedUrl()};

	Application::instance()->createWindow(Application::WT_NewWindow, u);
}

void HistoryManager::openUrlInNewPrivateWindow(const QUrl& url)
{
	const QUrl u{!url.isEmpty() ? url : m_view->selectedUrl()};

	Application::instance()->startPrivateBrowsing(u);
}

void HistoryManager::createContextMenu(const QPoint& pos)
{
	QMenu menu{};
	QAction* actionNewTab{menu.addAction(tr("Open In New Tab"))};
	QAction* actionNewWindow{menu.addAction(Application::getAppIcon("new-window"), tr("Open In New Window"))};
	QAction* actionNewPrivateWindow{menu.addAction(tr("Open In New Private Window"))};

	menu.addSeparator();

	QAction* actionCopyUrl{menu.addAction(tr("Copy Url"), this, &HistoryManager::copyUrl)};
	QAction* actionCopyTitle{menu.addAction(tr("Copy Title"), this, &HistoryManager::copyTitle)};

	menu.addSeparator();

	QAction* actionDelete{menu.addAction(tr("Delete"))};

	connect(actionNewTab, SIGNAL(triggered()), this, SLOT(openUrlInNewTab()));
	connect(actionNewWindow, SIGNAL(triggered()), this, SLOT(openUrlInNewWindow()));
	connect(actionNewPrivateWindow, SIGNAL(triggered()), this, SLOT(openUrlInNewPrivateWindow()));
	connect(actionDelete, &QAction::triggered, m_view, &HistoryTreeView::removeSelectedItems);

	if (m_view->selectedUrl().isEmpty()) {
		actionNewTab->setEnabled(false);
		actionNewWindow->setEnabled(false);
		actionNewPrivateWindow->setEnabled(false);

		actionCopyTitle->setEnabled(false);
		actionCopyUrl->setEnabled(false);
	}

	menu.exec(pos);
}

void HistoryManager::copyUrl()
{
	QApplication::clipboard()->setText(m_view->selectedUrl().toString());
}

void HistoryManager::copyTitle()
{
	QApplication::clipboard()->setText(m_view->currentIndex().data().toString());
}

void HistoryManager::clearHistory()
{
	QMessageBox::StandardButton button{
		QMessageBox::warning(this, tr("Confirmation"), tr("Are you sure you want to delete all history?"),
		                     QMessageBox::Yes | QMessageBox::No)
	};

	if (button != QMessageBox::Yes)
		return;

	Application::instance()->history()->clearHistory();
}

void HistoryManager::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case  Qt::Key_Delete:
		m_view->removeSelectedItems();
		break;
	}

	QWidget::keyPressEvent(event);
}

void HistoryManager::setupUI()
{
	resize(762, 477);

	m_layout = new QGridLayout(this);

	m_searchSpacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_search = new QLineEdit(this);
	m_search->setPlaceholderText(tr("Search..."));

	m_view = new HistoryTreeView(this);
	m_clearButton = new QPushButton(tr("Delete"), this);
	m_deleteAllButton = new QPushButton(tr("Clear All History"), this);
	m_spacer = new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layout->addItem(m_searchSpacer, 0, 0, 1, 2);
	m_layout->addWidget(m_search, 0, 2, 1, 1);
	m_layout->addWidget(m_view, 1, 0, 1, 3);
	m_layout->addWidget(m_clearButton, 2, 0, 1, 1);
	m_layout->addWidget(m_deleteAllButton, 2, 1, 1, 1);
	m_layout->addItem(m_spacer, 2, 2, 1, 1);
}

BrowserWindow *HistoryManager::getWindow()
{
	if (!m_window)
		m_window = Application::instance()->getWindow();

	return m_window.data();
}
}
