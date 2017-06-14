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

#include "Widgets/AddressBar.hpp"

#include <QSpacerItem>
#include <QMenu>

#include <QMimeData>
#include <QAbstractItemView>

#include <QSettings>
#include <QApplication>
#include <QClipboard>

#include <QIcon>

#include <QStyle>

#include "History/HistoryManager.hpp"
#include "History/HistoryItem.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/Tab/TabbedWebView.hpp"

#include "Widgets/Tab/TabWidget.hpp"

#include "Utils/AddressCompleter.hpp"
#include "Utils/ToolButton.hpp"

#include "BrowserWindow.hpp"

namespace Sn {

QString AddressBar::urlToText(const QUrl& url)
{
	if (url.scheme().isEmpty())
		return QUrl::fromPercentEncoding(url.toEncoded());

	QString stringUrl{url.toString(QUrl::RemoveQuery | QUrl::RemoveFragment)};

	if (url.hasQuery())
		stringUrl += QLatin1Char('?') + url.query(QUrl::FullyEncoded);

	if (url.hasFragment())
		stringUrl += QLatin1Char('#') + url.fragment(QUrl::FullyEncoded);

	stringUrl.replace(QLatin1Char(' '), QLatin1String("%20"));

	//TODO: add all sielo:xxx
	if (stringUrl == QLatin1String("sielo::adblock") || stringUrl == QLatin1String("about:blank"))
		stringUrl.clear();

	return stringUrl;

}

SideWidget::SideWidget(QWidget* parent) :
	QWidget(parent)
{
	setCursor(Qt::ArrowCursor);
}

bool SideWidget::event(QEvent* event)
{
	if (event->type() == QEvent::LayoutRequest)
		emit sizeHintChanged();

	return QWidget::event(event);
}

AddressBar::AddressBar(BrowserWindow* window) :
	QLineEdit(window),
	m_window(window),
	m_minHeight(0),
	m_leftMargin(-1),
	m_oldTextLength(0),
	m_currentTextLength(0),
	m_loadProgress(0),
	m_ignoreMousePress(false),
	m_holdingAlt(false)
{
	setObjectName("addressbar");
	setDragEnabled(true);

	m_siteIcon = new ToolButton(this);
	m_siteIcon->setAutoRaise(true);
	m_siteIcon->setFocusPolicy(Qt::NoFocus);
	m_siteIcon->setIcon(QIcon(":icons/other/webpage.png"));
	m_siteIcon->setObjectName("addressbar-website-icon");

	m_reloadStopButton = new ToolButton(this);
	m_reloadStopButton->setAutoRaise(true);
	m_reloadStopButton->setToolTip(tr("Reload"));
	m_reloadStopButton->setFocusPolicy(Qt::NoFocus);
	m_reloadStopButton->setObjectName("addressbar-button-reload");

	m_goButton = new ToolButton(this);
	m_goButton->setAutoRaise(true);
	m_goButton->setToolTip(tr("Load"));
	m_goButton->setFocusPolicy(Qt::NoFocus);
	m_goButton->setObjectName("addressbar-button-go");

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(0, 0, 0, 0);
	m_layout->setSpacing(0);

	m_leftWidget = new SideWidget(this);
	m_leftWidget->resize(0, 0);

	m_rightWidget = new SideWidget(this);
	m_rightWidget->resize(0, 0);

	m_leftLayout = new QHBoxLayout(m_leftWidget);
	m_leftLayout->setContentsMargins(0, 0, 0, 0);
	m_leftLayout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);

	m_rightLayout = new QHBoxLayout(m_rightWidget);
	m_rightLayout->setContentsMargins(0, 0, 0, 0);
	m_rightLayout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);

	m_layout->addWidget(m_leftWidget, 0, Qt::AlignLeft);
	m_layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	m_layout->addWidget(m_rightWidget, 0, Qt::AlignRight);
	m_layout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);

	setWidgetSpacing(0);

	connect(m_leftWidget, &SideWidget::sizeHintChanged, this, &AddressBar::updateTextMargins);
	connect(m_rightWidget, &SideWidget::sizeHintChanged, this, &AddressBar::updateTextMargins);

	addWidget(m_siteIcon, AddressBar::LeftSide);
	addWidget(m_goButton, AddressBar::RightSide);
	addWidget(m_reloadStopButton, AddressBar::RightSide);

	connect(m_reloadStopButton, &ToolButton::clicked, this, &AddressBar::reloadStopClicked);

	QAction* undoAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-undo")), tr("&Undo"), this);
	undoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Z")));
	undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* redoAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-redo")), tr("&Redo"), this);
	redoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+Z")));
	redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* cutAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-cut")), tr("Cu&t"), this);
	cutAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+X")));
	cutAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* copyAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-copy")), tr("&Copy"), this);
	copyAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+C")));
	copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* pasteAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), tr("&Paste"), this);
	pasteAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+V")));
	pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* pasteAndGoAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-paste")), tr("Paste and &Go"), this);
	pasteAndGoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+V")));
	pasteAndGoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* deleteAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-delete")), tr("Delete"), this);

	QAction* clearAllAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-clear")), tr("Clear All"), this);

	QAction* selectAllAction = new QAction(QIcon::fromTheme(QStringLiteral("edit-select-all")), tr("Select All"), this);
	selectAllAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+A")));
	selectAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	connect(undoAction, &QAction::triggered, this, &AddressBar::undo);
	connect(redoAction, &QAction::triggered, this, &AddressBar::redo);
	connect(cutAction, &QAction::triggered, this, &AddressBar::cut);
	connect(copyAction, &QAction::triggered, this, &AddressBar::copy);
	connect(deleteAction, &QAction::triggered, this, &AddressBar::sDelete);
	connect(clearAllAction, &QAction::triggered, this, &AddressBar::clear);
	connect(pasteAction, &QAction::triggered, this, &AddressBar::paste);
	connect(pasteAndGoAction, &QAction::triggered, this, &AddressBar::pasteAndGo);
	connect(selectAllAction, &QAction::triggered, this, &AddressBar::selectAll);

	m_editActions[Undo] = undoAction;
	m_editActions[Redo] = redoAction;
	m_editActions[Cut] = cutAction;
	m_editActions[Copy] = copyAction;
	m_editActions[Paste] = pasteAction;
	m_editActions[PasteAndGo] = pasteAndGoAction;
	m_editActions[Delete] = deleteAction;
	m_editActions[ClearAll] = clearAllAction;
	m_editActions[SelectAll] = selectAllAction;

	// Make action shortcuts available for webview
	addAction(undoAction);
	addAction(redoAction);
	addAction(cutAction);
	addAction(copyAction);
	addAction(pasteAction);
	addAction(pasteAndGoAction);
	addAction(deleteAction);
	addAction(clearAllAction);
	addAction(selectAllAction);

	connect(this, &AddressBar::textChanged, this, &AddressBar::updateActions);
	connect(this, &AddressBar::textChanged, this, &AddressBar::textEdited);
	connect(this, &AddressBar::selectionChanged, this, &AddressBar::updateActions);
	connect(m_goButton, &ToolButton::clicked, this, &AddressBar::requestLoadUrl);
	connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &AddressBar::updatePasteActions);

	updateActions();
	updatePasteActions();
	loadSettings();
	updateSiteIcon();

	QStringList suggestionList{};
		foreach (HistoryItem item, Application::instance()->historyManager()->history()) {
			suggestionList << item.url;
		}

	AddressCompleter* completer{new AddressCompleter(suggestionList, this)};
	completer->setCaseSensitivity(Qt::CaseInsensitive);

	setCompleter(completer);
}

void AddressBar::setWebView(TabbedWebView* view)
{
	m_webView = view;

	updateSiteIcon();

	connect(m_webView, &TabbedWebView::loadStarted, this, &AddressBar::loadStarted);
	connect(m_webView, &TabbedWebView::loadProgress, this, &AddressBar::loadProgress);
	connect(m_webView, &TabbedWebView::loadFinished, this, &AddressBar::loadFinished);

	connect(m_webView, SIGNAL(urlChanged(QUrl)), this, SLOT(showUrl(QUrl)));
	//TODO: privacy
	connect(m_webView, &TabbedWebView::iconChanged, this, &AddressBar::updateSiteIcon);
}

int AddressBar::widgetSpacing() const
{
	return m_leftLayout->spacing();
}

int AddressBar::leftMargin() const
{
	return m_leftMargin;
}

void AddressBar::addWidget(QWidget* widget, WidgetPosition position)
{
	if (!widget)
		return;

	if (position == LeftSide)
		m_leftLayout->addWidget(widget);
	else
		m_rightLayout->addWidget(widget);
}

void AddressBar::removeWidget(QWidget* widget)
{
	if (!widget)
		return;

	m_leftLayout->removeWidget(widget);
	m_rightLayout->removeWidget(widget);
	widget->hide();
}

void AddressBar::setWidgetSpacing(int spacing)
{
	m_leftLayout->setSpacing(spacing);
	m_rightLayout->setSpacing(spacing);
	updateTextMargins();
}

void AddressBar::setMinHeight(int height)
{
	m_minHeight = height;
}

void AddressBar::setCompleter(AddressCompleter* completer)
{
	if (m_completer)
		disconnect(m_completer, nullptr, this, nullptr);

	m_completer = completer;

	if (!m_completer)
		return;

	m_completer->setWidget(this);
	connect(m_completer, SIGNAL(activated(
									const QString&)), this, SLOT(insertCompletion(
																	 const QString&)));
//	connect(m_completer, SIGNAL(highlighted(const QString&)), this, SLOT(insertHighlighted(const QString&)));
}

void AddressBar::setTextFormat(const AddressBar::TextFormat& format)
{
	QList<QInputMethodEvent::Attribute> attributes;

		foreach (const QTextLayout::FormatRange& formatRange, format) {
			QInputMethodEvent::AttributeType type = QInputMethodEvent::TextFormat;
			int start{formatRange.start - cursorPosition()};
			int lenght{formatRange.length};
			QVariant value{formatRange.format};

			attributes.append(QInputMethodEvent::Attribute(type, start, lenght, value));
		}

	QInputMethodEvent evnt{QString(), attributes};
	event(&evnt);
}

void AddressBar::clearTextFormat()
{
	setTextFormat(AddressBar::TextFormat());
}

QSize AddressBar::sizeHint() const
{
	QSize size{QLineEdit::sizeHint()};

	if (size.height() < m_minHeight)
		size.setHeight(m_minHeight);

	return size;
}

QAction* AddressBar::editAction(EditAction action) const
{
	return m_editActions[action];
}

void AddressBar::setLeftMargin(int margin)
{
	m_leftMargin = margin;
}

void AddressBar::updateTextMargins()
{
	int left{m_leftWidget->sizeHint().width()};
	int right{m_rightWidget->sizeHint().width()};
	int top{0};
	int bottom{0};

	if (m_leftMargin >= 0)
		left = m_leftMargin;

	setTextMargins(left, top, right, bottom);
}

void AddressBar::setText(const QString& text)
{
	m_oldTextLength = text.length();
	m_currentTextLength = m_oldTextLength;

	QLineEdit::setText(text);

	refreshTextFormat();
}

void AddressBar::showUrl(const QUrl& url)
{
	if (hasFocus() && url.isEmpty())
		return;

	const QString stringUrl{urlToText(url)};

	if (text() == stringUrl) {
		home(false);
		refreshTextFormat();
		return;
	}

	setText(stringUrl);
	home(false);
}

bool AddressBar::event(QEvent* event)
{
	if (event->type() == QEvent::LayoutDirectionChange) {
		if (isRightToLeft()) {
			m_layout->setDirection(QBoxLayout::RightToLeft);
			m_leftLayout->setDirection(QBoxLayout::RightToLeft);
			m_rightLayout->setDirection(QBoxLayout::RightToLeft);
		}
		else {
			m_layout->setDirection(QBoxLayout::LeftToRight);
			m_leftLayout->setDirection(QBoxLayout::LeftToRight);
			m_rightLayout->setDirection(QBoxLayout::LeftToRight);
		}
	}

	return QLineEdit::event(event);
}

void AddressBar::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu* menu{createContextMenu()};
	menu->setAttribute(Qt::WA_DeleteOnClose);

	QPoint position{event->globalPos()};
	position.setY(position.y() + 1);

	menu->popup(position);
}

void AddressBar::showEvent(QShowEvent* event)
{
	QLineEdit::showEvent(event);

	refreshTextFormat();
}

void AddressBar::focusInEvent(QFocusEvent* event)
{
	if (m_webView) {
		const QString stringUrl{urlToText(m_webView->url())};

		if (stringUrl != text())
			setGoButtonVisible(true);
	}

	clearTextFormat();

	QSettings settings{};
	bool selectAllOnClick{settings.value("AddressBar/selectAllOnClick", true).toBool()};

	if (event->reason() == Qt::MouseFocusReason && selectAllOnClick) {
		m_ignoreMousePress = true;
		selectAll();
	}

	QLineEdit::focusInEvent(event);
}

void AddressBar::focusOutEvent(QFocusEvent* event)
{
	if (event->reason() == Qt::PopupFocusReason)
		return;

	QLineEdit::focusOutEvent(event);

	setGoButtonVisible(false);

	if (text().trimmed().isEmpty())
		clear();

	refreshTextFormat();
}

void AddressBar::mousePressEvent(QMouseEvent* event)
{
	if (m_ignoreMousePress) {
		m_ignoreMousePress = false;
		return;
	}

	refreshCompleter();

	QLineEdit::mousePressEvent(event);
}

void AddressBar::mouseReleaseEvent(QMouseEvent* event)
{
	if (!dragEnabled()) {
		QLineEdit::mouseReleaseEvent(event);
		return;
	}

	bool wasSelectedText = !selectedText().isEmpty();

	QLineEdit::mouseReleaseEvent(event);

	bool isSelectedText = !selectedText().isEmpty();

	if (wasSelectedText && !isSelectedText) {
		QMouseEvent event{QEvent::MouseButtonPress, event.pos(), event.button(), event.buttons(), event.modifiers()};
		mousePressEvent(&event);
	}
}

void AddressBar::mouseDoubleClickEvent(QMouseEvent* event)
{
	QSettings settings{};
	bool selectAllOnDoubleClick{settings.value("AddressBar/selectAllOnDoubleClick", true).toBool()};

	if (event->buttons() == Qt::LeftButton && selectAllOnDoubleClick) {
		selectAll();
		return;
	}

	QLineEdit::mouseDoubleClickEvent(event);
}

void AddressBar::dropEvent(QDropEvent* event)
{
	if (event->mimeData()->hasUrls()) {
		const QUrl dropUrl{event->mimeData()->urls()[0]};

		if (WebView::isUrlValide(dropUrl)) {
			setText(dropUrl.toString());

			m_webView->setFocus();
			m_webView->userLoadAction(dropUrl);

			QFocusEvent evnt{QFocusEvent::FocusOut};
			QLineEdit::focusOutEvent(&evnt);

			return;
		}
	}
	else if (event->mimeData()->hasText()) {
		const QString dropText{event->mimeData()->text().trimmed()};
		const QUrl dropUrl{QUrl(dropText)};

		if (WebView::isUrlValide(dropUrl)) {
			setText(dropUrl.toString());

			m_webView->setFocus();
			m_webView->userLoadAction(dropUrl);

			QFocusEvent evnt{QFocusEvent::FocusOut};
			QLineEdit::focusOutEvent(&evnt);

			return;
		}
		else {
			setText(dropText);
			setFocus();

			return;
		}
	}

	QLineEdit::dropEvent(event);
}

void AddressBar::paintEvent(QPaintEvent* event)
{
	QLineEdit::paintEvent(event);

	//TODO: if we want to show loading progress...
}

void AddressBar::keyPressEvent(QKeyEvent* event)
{
	switch (event->key()) {
	case Qt::Key_V:
		if (event->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
			pasteAndGo();
			event->accept();
			return;
		}
		break;
		//TODO: key for completer
	case Qt::Key_Down:
		break;
	case Qt::Key_Left:
		break;
	case Qt::Key_Escape:
		m_webView->setFocus();
		showUrl(m_webView->url());
		event->accept();
		break;
	case Qt::Key_Alt:
		m_holdingAlt = true;
		break;
	case Qt::Key_Return:
	case Qt::Key_Enter:
		switch (event->modifiers()) {
		case Qt::ControlModifier:
			if (!text().endsWith(QLatin1String(".com")))
				setText(text().append(QLatin1String(".com")));

			requestLoadUrl();
			m_holdingAlt = false;
			break;
		case Qt::AltModifier:
			m_window->tabWidget()->addView(createLoadRequest());
			m_holdingAlt = false;
			break;
		default:
			requestLoadUrl();
			m_holdingAlt = false;
		}

		break;
	case Qt::Key_0:
	case Qt::Key_1:
	case Qt::Key_2:
	case Qt::Key_3:
	case Qt::Key_4:
	case Qt::Key_5:
	case Qt::Key_6:
	case Qt::Key_7:
	case Qt::Key_8:
	case Qt::Key_9:
		if (event->modifiers() & Qt::AltModifier || event->modifiers() & Qt::ControlModifier) {
			event->ignore();
			m_holdingAlt = false;
			return;
		}

		break;
	default:
		m_holdingAlt = false;
	}

	if (m_completer && m_completer->popup()->isVisible()) {
// The following keys are forwarded by the completer to the widget
		switch (event->key()) {
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Escape:
		case Qt::Key_Tab:
		case Qt::Key_Backtab:
			event->ignore();
			return; // Let the completer do default behavior
		}
	}

	bool isShortcut = (event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E;
	if (!isShortcut)
		QLineEdit::keyPressEvent(event); // Don't send the shortcut (CTRL-E) to the text edit.

	if (!m_completer)
		return;

	bool ctrlOrShift = event->modifiers() & (Qt::ControlModifier | Qt::ShiftModifier);
	if (!isShortcut && !ctrlOrShift && event->modifiers() != Qt::NoModifier) {
		m_completer->popup()->hide();
		return;
	}

	m_completer->update(text());
	m_completer->popup()->setCurrentIndex(m_completer->completionModel()->index(0, 0));

}

QMenu* AddressBar::createContextMenu()
{
	QMenu* popup{new QMenu(this)};

	if (!isReadOnly()) {
		popup->addAction(m_editActions[Undo]);
		popup->addAction(m_editActions[Redo]);
		popup->addSeparator();
		popup->addAction(m_editActions[Cut]);
	}

	popup->addAction(m_editActions[Copy]);

	if (!isReadOnly()) {
		popup->addAction(m_editActions[Paste]);

		if (!m_editActions[PasteAndGo]->text().isEmpty())
			popup->addAction(m_editActions[PasteAndGo]);

		popup->addAction(m_editActions[Delete]);
		popup->addAction(m_editActions[ClearAll]);
	}

	popup->addSeparator();

	popup->addAction(m_editActions[SelectAll]);

	QMenu* temp{createStandardContextMenu()};

	temp->setParent(popup);
	temp->hide();

	QAction* lastAction{!temp->actions().isEmpty() ? temp->actions().last() : 0};

	if (lastAction && lastAction->menu() && lastAction->menu()->inherits("QUnicodeControlCharacterMenu"))
		popup->addAction(lastAction);

	return popup;
}

void AddressBar::updateActions()
{
	m_editActions[Undo]->setEnabled(!isReadOnly() && isUndoAvailable());
	m_editActions[Redo]->setEnabled(!isReadOnly() && isRedoAvailable());
	m_editActions[Cut]->setEnabled(!isReadOnly() && hasSelectedText() && echoMode() == QLineEdit::Normal);
	m_editActions[Copy]->setEnabled(hasSelectedText() && echoMode() == QLineEdit::Normal);
	m_editActions[Delete]->setEnabled(!isReadOnly() && hasSelectedText());
	m_editActions[SelectAll]->setEnabled(!text().isEmpty() && selectedText() != text());
}

void AddressBar::updatePasteActions()
{
	bool pasteEnabled{!isReadOnly() && !QApplication::clipboard()->text().isEmpty()};

	m_editActions[Paste]->setEnabled(pasteEnabled);
	m_editActions[PasteAndGo]->setEnabled(pasteEnabled);
}

void AddressBar::sDelete()
{
	if (hasSelectedText())
		del();
}

void AddressBar::insertCompletion(const QString& url)
{
	setText(url);
	requestLoadUrl();
}

void AddressBar::insertHighlighted(const QString& url)
{
	QString selection{selectedText()};
	QString originalText{text()};

	originalText.remove(selection);

	setText(url);

	setCursorPosition(url.indexOf(originalText) + originalText.size());
	setSelection(url.indexOf(originalText) + originalText.size(), url.size());
}

void AddressBar::textEdited(const QString& text)
{
	m_oldTextLength = m_currentTextLength;
	m_currentTextLength = text.length();

	setGoButtonVisible(true);
}

void AddressBar::requestLoadUrl()
{
	const LoadRequest request{createLoadRequest()};
	const QString urlString{urlToText(request.url())};

	m_webView->setFocus();

	if (urlString != text())
		setText(urlString);

	m_webView->userLoadAction(request);
}

void AddressBar::pasteAndGo()
{
	clear();
	paste();
	requestLoadUrl();
}

void AddressBar::reloadStopClicked()
{
	if (m_webView->isLoading())
		m_webView->stop();
	else
		m_webView->reload();
}

void AddressBar::updateSiteIcon()
{
	QIcon icon{m_webView ? m_webView->icon() : QIcon(":icons/other/webpage.png")};

	m_siteIcon->setIcon(icon);
}

void AddressBar::setGoButtonVisible(bool state)
{
	m_goButton->setVisible(state);

	updateTextMargins();
}

void AddressBar::loadStarted()
{
	m_siteIcon->setIcon(QIcon(":icons/other/webpage.png"));

	m_reloadStopButton->setToolTip(tr("Stop"));
	m_reloadStopButton->setObjectName("addressbar-button-stop");

	m_reloadStopButton->style()->unpolish(m_reloadStopButton);
	m_reloadStopButton->style()->polish(m_reloadStopButton);
}

void AddressBar::loadProgress(int progress)
{
	// TODO: show loading in address bar
}

void AddressBar::loadFinished()
{
	updateSiteIcon();

	m_reloadStopButton->setToolTip(tr("Reload"));
	m_reloadStopButton->setObjectName("addressbar-button-reload");

	m_reloadStopButton->style()->unpolish(m_reloadStopButton);
	m_reloadStopButton->style()->polish(m_reloadStopButton);
}

void AddressBar::loadFromCompleter(QString& text)
{

}

void AddressBar::loadSettings()
{
	//TODO: nothing to do now
}

void AddressBar::hideProgress()
{
	//TODO: if show progress
}

LoadRequest AddressBar::createLoadRequest() const
{
	LoadRequest request{};
	const QString& text = this->text().trimmed();
	int firstSpacePos = text.indexOf(QLatin1Char(' '));

	//TODO: add shortcuts

	if (request.isEmpty()) {
		if (!text.contains(QLatin1Char(' ')) && !text.contains(QLatin1Char('.')))
			request.setUrl(text);
		else {
			const QUrl& guessed = QUrl::fromUserInput(text);

			if (!guessed.isEmpty())
				request.setUrl(guessed);
			else
				request.setUrl(QUrl::fromEncoded(text.toUtf8()));
		}
	}

	return request;
}

void AddressBar::refreshTextFormat()
{
	if (!m_webView)
		return;

	TextFormat textFormat{};
	QString hostName = m_webView->url().isEmpty() ? QUrl(text()).host() : m_webView->url().host();

	if (!hostName.isEmpty()) {
		const int hostPosition{text().indexOf(hostName)};

		if (hostPosition > 0) {
			// palette().color(QPalette::Base)
			// palette().color(QPalette::Text)
			// 1
			// 1
			QColor color1{palette().color(QPalette::Base)};
			QColor color2{palette().color(QPalette::Text)};
			int r, g, b, a;

			//TODO: possibility of bug in the future...
			r = (color1.red() + color2.red()) / 2;
			g = (color1.green() + color2.green()) / 2;
			b = (color1.blue() + color2.blue()) / 2;
			a = (color1.alpha() + color2.alpha()) / 2;

			QColor finalColor{r, g, b, a};

			QTextCharFormat format;
			format.setForeground(finalColor);

			QTextLayout::FormatRange schemePart{};
			schemePart.start = 0;
			schemePart.length = hostPosition;
			schemePart.format = format;

			QTextLayout::FormatRange hostPart{};
			hostPart.start = hostPosition;
			hostPart.length = hostName.size();

			QTextLayout::FormatRange remainingPart{};
			remainingPart.start = hostPosition + hostName.size();
			remainingPart.length = text().size() - remainingPart.start;
			remainingPart.format = format;

			textFormat.append(schemePart);
			textFormat.append(hostPart);
			textFormat.append(remainingPart);
		}
	}

	setTextFormat(textFormat);
}

void AddressBar::refreshCompleter()
{
	if (!m_completer)
		return;

	QStringList suggestionList{};
		foreach (HistoryItem item, Application::instance()->historyManager()->history()) {
			suggestionList << item.url;
		}

	m_completer->setModel(new QStringListModel(suggestionList));
}

}