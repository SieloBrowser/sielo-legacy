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
#ifndef SIELO_BROWSER_SEARCHTOOLBAR_HPP
#define SIELO_BROWSER_SEARCHTOOLBAR_HPP

#include "SharedDefines.hpp"

#include <QWidget>

#include <QFocusEvent>

#include <QHBoxLayout>

#include <QPushButton>
#include <QLabel>
#include <QLineEdit>

#include <QWebEngine/WebPage.hpp>

namespace Sn {
class WebView;

class SIELO_SHAREDLIB FocusSelectLineEdit: public QLineEdit {
Q_OBJECT
public:
	FocusSelectLineEdit(QWidget* parent = 0);

public slots:
	void setFocus();

protected:
	void focusInEvent(QFocusEvent* event);
	void mousePressEvent(QMouseEvent* event);

	bool m_mouseFocusReason{false};

};

class SIELO_SHAREDLIB SearchToolBar: public QWidget {
Q_OBJECT

public:
	SearchToolBar(WebView* view, QWidget* parent = nullptr);
	~SearchToolBar();

	void showMinimalInPopupWindow();

	void focusSearchLine();
	bool eventFilter(QObject* object, QEvent* event);

public slots:
	void searchText(const QString& text);
	void updateFindFlags();
	void caseSensitivityChanged();

	void findNext();
	void findPrevious();

	void close();

private:
	void setupUI();

	QHBoxLayout* m_layout{nullptr};

	QPushButton* m_closeButton{nullptr};
	QLabel* m_desc{nullptr};
	FocusSelectLineEdit* m_toSearch{nullptr};
	QPushButton* m_previous{nullptr};
	QPushButton* m_next{nullptr};
	QPushButton* m_caseSensitive{nullptr};
	QLabel* m_results{nullptr};
	QSpacerItem* m_spacer{nullptr};

	WebView* m_view{nullptr};

	Engine::WebPage::FindFlags m_findFlags{};
};

}

#endif //SIELO_BROWSER_SEARCHTOOLBAR_HPP
