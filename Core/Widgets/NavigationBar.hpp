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

#pragma once
#ifndef SIELO_BROWSER_NAVIGATIONBAR_HPP
#define SIELO_BROWSER_NAVIGATIONBAR_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QHBoxLayout>
#include <QSplitter>
#include <QStackedWidget>

namespace Sn {

class ToolButton;

class BrowserWindow;

class SIELO_SHAREDLIB NavigationToolBar: public QWidget {
Q_OBJECT

public:
	NavigationToolBar(BrowserWindow* window, QStackedWidget* addressBars);

	Q_PROPERTY(int layoutMargin
			   READ
			   layoutMargin
			   WRITE
			   setLayoutMargin)
		Q_PROPERTY(int layoutSpacing
				   READ
				   layoutSpacing
				   WRITE
				   setLayoutSpacing)

		void loadSettings();

	int layoutMargin() const;
	void setLayoutMargin(int margin);

	int layoutSpacing() const;
	void setLayoutSpacing(int layoutSpacing);

	void hideBookmarksHistory();
	void showBookmarksHistory();
	void hideControls();
	void showControls();

	void addExtensionAction(QWidget* widget);

public slots:
	void refreshBackForwardButtons();

private slots:
	void goBack();
	void goBackInNewTab();
	void goForward();
	void goForwardInNewTab();
	void goHome();
	void goHomeInNewTab();
	void showBookmarksDialog();
	void showAddBookmarkDialog();
	void showHistoryDialog();

	void contextMenuRequested(const QPoint& pos);

private:
	BrowserWindow* m_window{nullptr};

	QHBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_extensionLayout{nullptr};
	QHBoxLayout* m_bookmarksHistoryLayout{nullptr};

	QWidget* m_extensionWidget{nullptr};
	QWidget* m_bookmarksHistoryWidget{nullptr};

	ToolButton* m_buttonBack{nullptr};
	ToolButton* m_buttonForward{nullptr};
	ToolButton* m_buttonHome{nullptr};

	ToolButton* m_buttonViewBookmarks{nullptr};
	ToolButton* m_buttonAddBookmark{nullptr};
	ToolButton* m_buttonViewHistory{nullptr};
};

}

#endif //SIELO_BROWSER_NAVIGATIONBAR_HPP
