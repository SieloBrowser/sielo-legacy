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

#ifndef SIELOBROWSER_TITLEBAR_HPP
#define SIELOBROWSER_TITLEBAR_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>

#include <QHBoxLayout>

#include <QStackedWidget>

#include <QMouseEvent>
#include <QContextMenuEvent>

namespace Sn {
class NavigationToolBar;
class BookmarksToolbar;

class BrowserWindow;

class SIELO_SHAREDLIB TitleBar : public QWidget {
Q_OBJECT

public:
	TitleBar(BrowserWindow* window, bool showBookmarks = true);
	~TitleBar() = default;

	BookmarksToolbar* bookmarksToolBar() const { return m_bookmarksToolbar; }
	NavigationToolBar* navigationToolBar() const { return m_navigationToolBar; }
	bool showBookmarks() const { return m_showBookmarks; }
	void setShowBookmark(bool show);

	bool isWindowMaximized() const;

	void hide();
	void show();

	bool isView();
	void setView(bool view);

	QWidget* control() const { return m_moveControlWidget; }
	QStackedWidget* addressBars() const { return m_addressBars; }

signals:
	void toggleBookmarksBar(bool shown);

protected:
	bool eventFilter(QObject* obj, QEvent* event);
	void contextMenuEvent(QObject* obj, QContextMenuEvent* event);

private slots:
	void build();

	void closeWindow();
	void toggleMaximize(bool forceMaximize = false);
	void minimize();

private:
	void setupUI();

	BrowserWindow* m_window{nullptr};

	QHBoxLayout* m_layout{nullptr};
	BookmarksToolbar* m_bookmarksToolbar{nullptr};
	NavigationToolBar* m_navigationToolBar{nullptr};
	QWidget* m_moveControlWidget{nullptr};

#ifdef Q_OS_WIN
	QToolButton* m_closeButton{nullptr};
	QToolButton* m_toggleMaximize{nullptr};
	QToolButton* m_minimize{nullptr};
#endif // Q_OS_WIN

	QStackedWidget* m_addressBars{nullptr};

	QRect m_geometry{};
	QPoint m_offset{};

	bool m_showBookmarks{true};
	bool m_isMaximized{false};
	bool m_isOnSide{false};
	bool m_canMove{true};
	bool m_show{true};
};
}

#endif //SIELOBROWSER_TITLEBAR_HPP
