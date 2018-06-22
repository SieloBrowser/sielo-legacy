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

#include "TitleBar.hpp"

#include <QDesktopWidget>
#include <QScreen>

#include <QSettings>

#include "Bookmarks/BookmarksUtils.hpp"
#include "Bookmarks/BookmarksToolbar.hpp"

#include "Widgets/Tab/TabWidget.hpp"


#include "Application.hpp"
#include "BrowserWindow.hpp"

namespace Sn {
TitleBar::TitleBar(BrowserWindow* window, bool showBookmarks) :
		QWidget(window),
		m_window(window),
		m_showBookmarks(showBookmarks)
{
	m_bookmarksToolbar = new BookmarksToolbar(m_window, m_window);
	m_controlsToolbar = new QToolBar(m_window);

	m_bookmarksToolbar->setFloatable(false);
	m_bookmarksToolbar->installEventFilter(this);
	m_bookmarksToolbar->setContextMenuPolicy(Qt::CustomContextMenu);
	m_controlsToolbar->setFloatable(false);
	m_controlsToolbar->installEventFilter(this);
	m_controlsToolbar->setObjectName(QLatin1String("title-bar"));
	m_controlsToolbar->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(m_bookmarksToolbar, &BookmarksToolbar::orientationChanged, this, &TitleBar::build);
	connect(m_controlsToolbar, &QToolBar::orientationChanged, this, &TitleBar::build);

	restoreToolBarsPositions();

	m_sizePreview = new QFrame(this);
	m_sizePreview->setObjectName(QLatin1String("window-sizepreview"));
	m_sizePreview->setStyleSheet("window-sizepreview{background: rgba(66, 134, 244, 0.5);}");
	m_sizePreview->setAttribute(Qt::WA_TransparentForMouseEvents);
	m_sizePreview->hide();

	build();
}

TitleBar::~TitleBar()
{
	// Empty
}

void TitleBar::setTitle(const QString& title)
{
	if (!m_title)
		return;

	m_title->setText(title);
}

void TitleBar::setShowBookmark(bool show)
{
	m_showBookmarks = show;
	build();
}

void TitleBar::saveToolBarsPositions()
{
	QSettings settings{};

	settings.beginGroup("TitleBar");

	settings.setValue("bookmarks/area", static_cast<int>(m_window->toolBarArea(m_bookmarksToolbar)));
	settings.setValue("bookmarks/locked", m_bookmarksToolbar->isMovable());
	settings.setValue("controls/area", static_cast<int>(m_window->toolBarArea(m_controlsToolbar)));
	settings.setValue("controls/locked", m_controlsToolbar->isMovable());

	settings.endGroup();
}

void TitleBar::restoreToolBarsPositions()
{
	QSettings settings{};

	settings.beginGroup("TitleBar");

	m_window->addToolBar(static_cast<Qt::ToolBarArea>(settings.value("bookmarks/area", Qt::TopToolBarArea).toInt()), m_bookmarksToolbar);
	m_bookmarksToolbar->setMovable(settings.value("bookmarks/locked", true).toBool());
	m_window->addToolBar(static_cast<Qt::ToolBarArea>(settings.value("controls/area", Qt::TopToolBarArea).toInt()), m_controlsToolbar);
	m_controlsToolbar->setMovable(settings.value("controls/locked", true).toBool());

	settings.endGroup();
}

void TitleBar::hide()
{
	setView(false);
}

void TitleBar::show()
{
	setView(true);
}

bool TitleBar::isView()
{
	return m_show;
}

void TitleBar::setView(bool view)
{
	m_show = view;
	build();
}

bool TitleBar::isWindowMaximized() const
{
	//return m_window->geometry() == Application::desktop()->availableGeometry(m_window);
	return m_isMaximized;
}

bool TitleBar::eventFilter(QObject* obj, QEvent* event)
{
	if (event->type() == QEvent::MouseButtonPress) {
		mousePressEvent(static_cast<QToolBar*>(obj), static_cast<QMouseEvent*>(event));
	}
	else if (event->type() == QEvent::MouseMove) {
		mouseMoveEvent(static_cast<QMouseEvent*>(event));
	}
	else if (event->type() == QEvent::MouseButtonRelease) {
		mouseReleaseEvent(static_cast<QMouseEvent*>(event));
	}
	else if (event->type() == QEvent::MouseButtonDblClick) {
		mouseDoubleClickEvent(static_cast<QMouseEvent*>(event));
	}
	else if (event->type() == QEvent::ContextMenu) {
		contextMenuEvent(obj, static_cast<QContextMenuEvent*>(event));
	}
	return QObject::eventFilter(obj, event);
}


void TitleBar::mousePressEvent(QToolBar* toolBar, QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && ((event->x() >= 10 && toolBar->orientation() == Qt::Horizontal) ||
											  (event->y() >= 10 && toolBar->orientation() == Qt::Vertical))) {
		m_offset = event->globalPos() - m_window->frameGeometry().topLeft();
		m_canMove = true;
		event->accept();
	}
	else
		m_canMove = false;

	QWidget::mousePressEvent(event);
}

void TitleBar::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton && m_canMove) {
		if (isWindowMaximized() || m_isOnSide) {
			m_window->resize(m_geometry.size());
			m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-maximize"));
			m_toggleMaximize->setIcon(Application::getAppIcon("tb-maximize", "titlebar"));

			m_isMaximized = false;
			m_isOnSide = false;
		}

		m_window->move(event->globalPos() - m_offset);
		event->accept();
	}

	if (QCursor::pos(Application::screenAt(QCursor::pos())).y() <= 0) {
		m_sizePreview->move(0, 0);
		m_sizePreview->resize(1920, 1080);
		m_sizePreview->show();
	}
	else {
		m_sizePreview->hide();
	}

	QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent* event)
{
	QScreen *screen = Application::screenAt(QCursor::pos());
	QRect scrrect = screen->geometry();

	if (QCursor::pos(screen).y() <= scrrect.y()) {
		toggleMaximize(true);
	}

	// The move function don't move properly, i need to remove 6 and 8 pixels.
	if (QCursor::pos(screen).x() <= scrrect.x()) {
		m_geometry = m_window->geometry();
		m_window->resize(screen->availableGeometry().width() / 2, screen->availableGeometry().height() - 7);
		m_window->move(scrrect.x() - 6, scrrect.y());
    
		m_isOnSide = true;
	}
	// There is 1 px missing on Windows for mouse position
	else if (QCursor::pos(screen).x() >= scrrect.x() + scrrect.width() - 1) {
		m_geometry = m_window->geometry();
		m_window->resize(screen->availableGeometry().width() / 2, screen->availableGeometry().height() - 7);
		m_window->move(scrrect.x() + (screen->availableGeometry().width() / 2) - 8, scrrect.y());

		m_isOnSide = true;
	}

	m_sizePreview->hide();

	QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		toggleMaximize();
	}
}

void TitleBar::contextMenuEvent(QObject* obj, QContextMenuEvent* event)
{
	QMenu menu{};
	QAction* hideShowAction{menu.addAction(tr("Show Bookmarks Bar"))};

	hideShowAction->setCheckable(true);
	hideShowAction->setChecked(m_showBookmarks);
	connect(hideShowAction, &QAction::toggled, this, [=]() {
		setShowBookmark(!m_showBookmarks);
		emit toggleBookmarksBar(m_showBookmarks);
	});

	if (obj != nullptr) {
		QToolBar* toolbar = qobject_cast<QToolBar*>(obj);
		QAction* lockToolbar(menu.addAction(tr("Lock Toolbar")));

		lockToolbar->setCheckable(true);
		lockToolbar->setChecked(!toolbar->isMovable());

		connect(lockToolbar, &QAction::toggled, this, [=]() {
			toolbar->setMovable(!toolbar->isMovable());
			Application::instance()->saveSession();
		});

		if (toolbar->objectName() == "bookmarks-toolbar") {
			menu.addSeparator();
			m_bookmarksToolbar->createContextMenu(menu, event->pos());
		}
	}

	const QPoint position{ event->globalPos() };
	QPoint point{ position.x(), position.y() + 1 };
	menu.exec(point);

	m_bookmarksToolbar->contextMenuCreated();
}

void TitleBar::build()
{
#ifdef Q_OS_WIN
	if (m_showBookmarks && m_show)
		m_bookmarksToolbar->show();
	else
		m_bookmarksToolbar->hide();

	if (m_show)
		m_controlsToolbar->show();
	else
		m_controlsToolbar->hide();

	m_controlsToolbar->clear();

	m_title = new QLabel(m_window->windowTitle(), m_controlsToolbar);
	m_closeButton = new QToolButton(m_controlsToolbar);
	m_toggleMaximize = new QToolButton(m_controlsToolbar);
	m_minimize = new QToolButton(m_controlsToolbar);

	m_title->setObjectName(QLatin1String("titlebar-title"));

	m_closeButton->setObjectName(QLatin1String("titlebar-button-close"));
	m_closeButton->setIcon(Application::getAppIcon("tb-close", "titlebar"));

	m_toggleMaximize->setObjectName(
			QLatin1String(isWindowMaximized() ? "titlebar-button-reverse-maximize" : "titlebar-button-maximize"));
	m_toggleMaximize->setIcon(isWindowMaximized() ? Application::getAppIcon("tb-revert-maximize", "titlebar") :
							  Application::getAppIcon("tb-maximize", "titlebar"));

	m_minimize->setObjectName(QLatin1String("titlebar-button-minimize"));
	m_minimize->setIcon(Application::getAppIcon("tb-minimize", "titlebar"));

	connect(m_closeButton, &QToolButton::clicked, this, &TitleBar::closeWindow);
	connect(m_toggleMaximize, &QToolButton::clicked, this, &TitleBar::toggleMaximize);
	connect(m_minimize, &QToolButton::clicked, this, &TitleBar::minimize);

	QWidget* firstSpacer{new QWidget(m_controlsToolbar)};
	QWidget* secondSpacer{new QWidget(m_controlsToolbar)};

	if (m_controlsToolbar->orientation() == Qt::Horizontal) {
		firstSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
		secondSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
	}
	else if (m_controlsToolbar->orientation() == Qt::Vertical) {
		firstSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
		secondSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
	}

	m_controlsToolbar->addWidget(firstSpacer);
	m_controlsToolbar->addWidget(m_title);
	m_controlsToolbar->addWidget(secondSpacer);
	m_controlsToolbar->addWidget(m_minimize);
	m_controlsToolbar->addWidget(m_toggleMaximize);
	m_controlsToolbar->addWidget(m_closeButton);
#else
	if (m_showBookmarks && m_show)
		m_bookmarksToolbar->show();
	else
		m_bookmarksToolbar->hide();
#endif

	Application::instance()->saveSession();
}

void TitleBar::closeWindow()
{
	m_window->close();
}

void TitleBar::toggleMaximize(bool forceMaximize)
{
	if (isWindowMaximized() && !forceMaximize) {
		if (m_window->isFullScreen())
			m_window->showNormal();

		m_window->setGeometry(m_geometry);
		m_isMaximized = false;

		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-maximize"));
		m_toggleMaximize->setIcon(Application::getAppIcon("tb-maximize", "titlebar"));
	}
	else {
		if (!m_isOnSide)
			m_geometry = m_window->geometry();

		m_window->setGeometry(Application::desktop()->availableGeometry(m_window));
		m_isMaximized = true;

		m_window->showNormal();

		m_toggleMaximize->setObjectName(QLatin1String("titlebar-button-reverse-maximize"));
		m_toggleMaximize->setIcon(Application::getAppIcon("tb-revert-maximize", "titlebar"));
	}
}

void TitleBar::minimize()
{
	m_window->showMinimized();
}
}
