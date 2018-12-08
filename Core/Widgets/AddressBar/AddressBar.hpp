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
#ifndef SIELOBROWSER_ADDRESSBAR_HPP
#define SIELOBROWSER_ADDRESSBAR_HPP

#include "SharedDefines.hpp"

#include <QLineEdit>
#include <QTextLayout>
#include <QAction>

#include <QStringListModel>

#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTreeView>

#include <QHBoxLayout>

#include <QSize>

#include <QEvent>
#include <QContextMenuEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QDropEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include "Widgets/LineEdit.hpp"

namespace Sn {
class TabbedWebView;
class LoadRequest;

class TabWidget;

class AddressBarCompleter;

class ToolButton;
class SiteIcon;

class SIELO_SHAREDLIB AddressBar: public LineEdit {
Q_OBJECT

public:
	AddressBar(TabWidget* tabWidget);

	TabbedWebView* webView() const { return m_webView; }
	void setWebView(TabbedWebView* view);

	TabWidget* tabWidget() const { return m_tabWidget; }
	void setTabWidget(TabWidget* tabWidget);

	static QString urlToText(const QUrl &url);

public slots:
	void setText(const QString& text);
	void showUrl(const QUrl& url);

private slots:
	void textEdited(const QString& text);
	void requestLoadUrl();
	void pasteAndGo();
	void reloadStopClicked();

	void updateSiteIcon();
	
	void setGoButtonVisible(bool state);

	void showCompletion(const QString &completion, bool completeDomain);
	void showDomainCompletion(const QString &completion);
	void clearCompletion();

	void loadStarted();
	void loadProgress(int progress);
	void loadFinished();

	void loadSettings();

private:
	void contextMenuEvent(QContextMenuEvent* event);
	void showEvent(QShowEvent* event);
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void dropEvent(QDropEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);

	LoadRequest createLoadRequest() const;
	void refreshTextFormat();

	bool processMainCommand(const QString& command, const QStringList& args);

	AddressBarCompleter* m_completer{nullptr};
	QStringListModel* m_domainCompleterModel{nullptr};

	SiteIcon* m_siteIcon{nullptr};
	ToolButton* m_reloadStopButton{nullptr};
	ToolButton* m_goButton{nullptr};

	TabWidget* m_tabWidget{nullptr};
	TabbedWebView* m_webView{nullptr};

	bool m_holdingAlt{false};
	int m_oldTextLength{0};
	int m_currentTextLength{0};
};

}

#endif //SIELOBROWSER_ADDRESSBAR_HPP
