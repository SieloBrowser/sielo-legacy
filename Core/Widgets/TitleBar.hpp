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

	void loadSettings();

	NavigationToolBar* navigationToolBar() const { return m_navigationToolBar; }

	bool isWindowMaximized() const;

	QWidget* control() const { return m_moveControlWidget; }
	QStackedWidget* addressBars() const { return m_addressBars; }

private slots:
	void closeWindow();
	void toggleMaximize(bool forceMaximize = false);
	void minimize();

private:
	void setupUI();

	BrowserWindow* m_window{nullptr};

	QHBoxLayout* m_layout{nullptr};
	QVBoxLayout* m_main_layout{nullptr};
	NavigationToolBar* m_navigationToolBar{nullptr};
	QWidget* m_moveControlWidget{nullptr};
	QWidget* m_moveTopControlWidget{nullptr};

#ifdef Q_OS_WIN
	QToolButton* m_closeButton{nullptr};
	QToolButton* m_toggleMaximize{nullptr};
	QToolButton* m_minimize{nullptr};
#endif // Q_OS_WIN

	QStackedWidget* m_addressBars{nullptr};

	QRect m_geometry{};
	bool m_isMaximized{false};
	bool m_isOnSide{false};
};
}

#endif //SIELOBROWSER_TITLEBAR_HPP
