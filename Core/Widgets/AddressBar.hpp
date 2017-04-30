/***********************************************************************************
** MIT License                                                                    **
**                                                                                **
** Copyright (c) 2017 Victor DENIS (victordenis01@gmail.com)                      **
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

#include <QLineEdit>
#include <QTextLayout>
#include <QAction>

#include <QCompleter>

#include <QHBoxLayout>

#include <QEvent>
#include <QContextMenuEvent>
#include <QShowEvent>
#include <QPaintEvent>
#include <QDropEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QMouseEvent>

#include <array>

namespace Sn {
class TabbedWebView;
class LoadRequest;

class BrowserWindow;

class AddressCompleter;
class ToolButton;

class SideWidget: public QWidget {
Q_OBJECT

public:
	SideWidget(QWidget* parent = nullptr);

signals:
	void sizeHintChanged();

protected:
	bool event(QEvent* event);
};

class AddressBar: public QLineEdit {
Q_OBJECT
	Q_PROPERTY(QSize fixedsize
				   READ
					   size
				   WRITE
				   setFixedSize)
	Q_PROPERTY(int leftMargin
				   READ
					   leftMargin
				   WRITE
				   setLeftMargin)
	Q_PROPERTY(int fixedwidth
				   READ
					   width
				   WRITE
				   setFixedWidth)
	Q_PROPERTY(int fixedheight
				   READ
					   height
				   WRITE
				   setFixedHeight)
	Q_PROPERTY(int minHeight
				   READ
					   minHeight
				   WRITE
				   setMinHeight)

public:
	using TextFormat = QList<QTextLayout::FormatRange>;

	enum WidgetPosition {
		LeftSide,
		RightSide
	};

	enum EditAction {
		Undo = 0,
		Redo = 1,
		Cut = 2,
		Copy = 3,
		Paste = 4,
		PasteAndGo = 5,
		Delete = 6,
		ClearAll = 7,
		SelectAll = 8
	};

	static QString urlToText(const QUrl& url);

	AddressBar(BrowserWindow* window);

	TabbedWebView* webView() const { return m_webView; }
	void setWebView(TabbedWebView* view);

	int widgetSpacing() const;
	int leftMargin() const;

	void addWidget(QWidget* widget, WidgetPosition position);
	void removeWidget(QWidget* widget);
	void setWidgetSpacing(int spacing);

	int minHeight() const { return m_minHeight; }
	void setMinHeight(int height);

	AddressCompleter* addressCompleter() const { return m_completer; }
	void setCompleter(AddressCompleter* completer);

	void setTextFormat(const TextFormat& format);
	void clearTextFormat();

	QSize sizeHint() const;

	QAction* editAction(EditAction action) const;

public slots:
	void setLeftMargin(int margin);
	void updateTextMargins();

	void setText(const QString& text);
	void showUrl(const QUrl& url);

protected:
	bool event(QEvent* event);
	void contextMenuEvent(QContextMenuEvent* event);
	void showEvent(QShowEvent* event);
	void focusInEvent(QFocusEvent* event);
	void focusOutEvent(QFocusEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void mousePressEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void dropEvent(QDropEvent* event);
	void paintEvent(QPaintEvent* event);

	QMenu* createContextMenu();

private slots:
	void updateActions();
	void updatePasteActions();
	void sDelete();

	void insertCompletion(const QString& url);
	void insertHighlighted(const QString& url);

	void textEdited(const QString& text);
	void requestLoadUrl();
	void pasteAndGo();

	void reloadStopClicked();

	void updateSiteIcon();

	void setGoButtonVisible(bool state);

	void loadStarted();
	void loadProgress(int progress);
	void loadFinished();
	void hideProgress();

	void loadFromCompleter(QString& text);
	void loadSettings();

private:
	LoadRequest createLoadRequest() const;
	void refreshTextFormat();
	void refreshCompleter();

	ToolButton* m_siteIcon{nullptr};
	ToolButton* m_reloadStopButton{nullptr};
	ToolButton* m_goButton{nullptr};

	BrowserWindow* m_window{nullptr};
	TabbedWebView* m_webView{nullptr};

	SideWidget* m_leftWidget{nullptr};
	SideWidget* m_rightWidget{nullptr};

	AddressCompleter* m_completer{nullptr};

	QHBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_leftLayout{nullptr};
	QHBoxLayout* m_rightLayout{nullptr};

	QString m_lastHighlighted{};

	std::array<QAction*, 9> m_editActions;

	int m_minHeight{0};
	int m_leftMargin{-1};
	int m_oldTextLength{0};
	int m_currentTextLength{0};
	int m_loadProgress{};

	bool m_ignoreMousePress{false};
	bool m_holdingAlt{false};
};

}

#endif //SIELOBROWSER_ADDRESSBAR_HPP
