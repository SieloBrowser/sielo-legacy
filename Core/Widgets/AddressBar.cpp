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

#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QStyle>
#include <QMessageBox>

#include <QHeaderView>

#include "History/HistoryManager.hpp"
#include "History/HistoryItem.hpp"

#include "Utils/AddressCompletionModel.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/Tab/TabbedWebView.hpp"
#include "Web/Tab/WebTab.hpp"

#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabBar.hpp"

#include "Utils/AddressCompletionModel.hpp"
#include "Utils/ToolButton.hpp"

#include "BrowserWindow.hpp"

namespace Sn {

AddressDelegate::AddressDelegate(const QString& highlight, QObject* parent) :
	QStyledItemDelegate(parent),
	m_highlight(highlight)
{
	// Empty
}

void AddressDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QRect titleRectangle(option.rect);
	const bool isRightToLeft(option.direction == Qt::RightToLeft);

	if (static_cast<AddressCompletionModel::EntryType>(index.data(AddressCompletionModel::TypeRole).toInt())
		== AddressCompletionModel::HeaderType) {
		QStyleOptionViewItem headerOption(option);
		headerOption.rect = titleRectangle.marginsRemoved(QMargins(0, 2, (isRightToLeft ? 3 : 0), 2));
		headerOption.text = index.data(AddressCompletionModel::TitleRole).toString();

		if (index.row() > 0) {
			QPen pen(option.palette.color(QPalette::Disabled, QPalette::Text).lighter());
			pen.setWidth(1);
			pen.setStyle(Qt::SolidLine);

			painter->save();
			painter->setPen(pen);
			painter->drawLine((option.rect.left() + 5),
							  (option.rect.top() + 3),
							  (option.rect.right() - 5),
							  (option.rect.top() + 3));
			painter->restore();
		}

		QStyledItemDelegate::paint(painter, headerOption, index);

		return;
	}

	QAbstractTextDocumentLayout::PaintContext paintContext;
	QTextDocument document;
	document.setDefaultFont(option.font);

	QString url(index.data(Qt::DisplayRole).toString());
	QString description(index.data(AddressCompletionModel::TitleRole).toString());
	const int topPosition(titleRectangle.top() - ((titleRectangle.height() - painter->clipBoundingRect()
		.united(document.documentLayout()->blockBoundingRect(document.firstBlock())).height()) / 2));
	const bool isSearchSuggestion
		{static_cast<AddressCompletionModel::EntryType>(index.data(AddressCompletionModel::TypeRole).toInt())
		 == AddressCompletionModel::SearchSuggestionType};

	if (option.state.testFlag(QStyle::State_Selected)) {
		painter->fillRect(option.rect, option.palette.color(QPalette::Highlight));

		paintContext.palette.setColor(QPalette::Text, option.palette.color(QPalette::HighlightedText));
	}
	else if (!isSearchSuggestion) {
		paintContext.palette.setColor(QPalette::Text, option.palette.color(QPalette::Link));
	}

	QRect decorationRectangle(option.rect);

	if (isRightToLeft) {
		decorationRectangle.setLeft(option.rect.width() - option.rect.height() - 5);

		titleRectangle.setRight(option.rect.width() - option.rect.height() - 10);
	}
	else {
		decorationRectangle.setRight(option.rect.height());

		titleRectangle.setLeft(option.rect.height());
	}

	decorationRectangle = decorationRectangle.marginsRemoved(QMargins(2, 2, 2, 2));

	QIcon icon(index.data(Qt::DecorationRole).value<QIcon>());

	if (icon.isNull()) {
		icon = Application::getAppIcon("webpage");
	}

	icon.paint(painter, decorationRectangle, option.decorationAlignment);

	painter->save();

	url = option.fontMetrics.elidedText(url, Qt::ElideRight, (option.rect.width() - 40));

	if (isRightToLeft) {
		painter->translate((titleRectangle.right() - calculateLength(option, url)), topPosition);
	}
	else {
		painter->translate(titleRectangle.left(), topPosition);
	}

	document.setHtml(isSearchSuggestion ? url : highlightText(url));
	document.documentLayout()->draw(painter, paintContext);

	painter->restore();

	if (!description.isEmpty()) {
		const int urlLength(calculateLength(option, url + QLatin1Char(' ')));

		if ((urlLength + 40) < titleRectangle.width()) {
			painter->save();

			description = option.fontMetrics.elidedText(description,
														(isRightToLeft ? Qt::ElideLeft : Qt::ElideRight),
														(option.rect.width() - urlLength - 50));

			if (isRightToLeft) {
				description.append(QLatin1String(" -"));

				titleRectangle.setRight(option.rect.width() - calculateLength(option, description) - (urlLength + 33));

				painter->translate(titleRectangle.right(), topPosition);
			}
			else {
				description.insert(0, QLatin1String("- "));

				titleRectangle.setLeft(urlLength + 33);

				painter->translate(titleRectangle.left(), topPosition);
			}

			document.setHtml(highlightText(description));

			if (option.state.testFlag(QStyle::State_Selected)) {
				document.documentLayout()->draw(painter, paintContext);
			}
			else {
				document.drawContents(painter);
			}

			painter->restore();
		}
	}
}

QSize AddressDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize size(index.data(Qt::SizeHintRole).toSize());

	if (index.row() != 0
		&& static_cast<AddressCompletionModel::EntryType>(index.data(AddressCompletionModel::TypeRole).toInt())
		   == AddressCompletionModel::HeaderType) {
		size.setHeight(option.fontMetrics.lineSpacing() * 1.75);
	}
	else {
		size.setHeight(option.fontMetrics.lineSpacing() * 1.25);
	}

	return size;
}

QString AddressDelegate::highlightText(const QString& text, QString html) const
{
	const int index(text.indexOf(m_highlight, 0, Qt::CaseInsensitive));

	if (m_highlight.isEmpty() || index < 0) {
		return (html + text);
	}

	html += text.left(index);
	html += QStringLiteral("<b>%1</b>").arg(text.mid(index, m_highlight.length()));

	return highlightText(text.mid(index + m_highlight.length()), html);
}

int AddressDelegate::calculateLength(const QStyleOptionViewItem& option, const QString& text, int length) const
{
	const int index(text.indexOf(m_highlight, 0, Qt::CaseInsensitive));

	if (m_highlight.isEmpty() || index < 0) {
		return (length + option.fontMetrics.width(text));
	}

	length += option.fontMetrics.width(text.left(index));

	QStyleOptionViewItem highlightedOption(option);
	highlightedOption.font.setBold(true);

	length += highlightedOption.fontMetrics.width(text.mid(index, m_highlight.length()));

	return calculateLength(option, text.mid(index + m_highlight.length()), length);
}

PopupViewWidget::PopupViewWidget(AddressBar* parent) :
	QTreeView(nullptr),
	m_addressBar(parent)
{
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setFocusPolicy(Qt::NoFocus);
	setWindowFlags(Qt::Popup);
	header()->setStretchLastSection(true);
	header()->hide();
	viewport()->setAttribute(Qt::WA_Hover);
	viewport()->setMouseTracking(true);
	viewport()->installEventFilter(this);

	connect(this, &PopupViewWidget::needsActionsUpdate, this, &PopupViewWidget::updateHeight);
	connect(this, &PopupViewWidget::entered, this, &PopupViewWidget::handleIndexEntered);
}

void PopupViewWidget::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	QTreeView::currentChanged(current, previous);

	if (selectionModel() && selectionModel()->hasSelection()) {
		if (m_sourceModel) {
			emit canMoveUpChanged(canMoveUp());
			emit canMoveDownChanged(canMoveDown());
		}

		emit needsActionsUpdate();
	}
}

QModelIndex PopupViewWidget::getCurrentIndex(int column) const
{
	if (!selectionModel() || !selectionModel()->hasSelection()) {
		return QModelIndex();
	}

	if (column >= 0) {
		return currentIndex().sibling(currentIndex().row(), column);
	}

	return currentIndex();
}

QModelIndex PopupViewWidget::getIndex(int row, int column, const QModelIndex& parent) const
{
	return (model() ? model()->index(row, column, parent) : QModelIndex());
}

int PopupViewWidget::getCurrentRow() const
{
	return ((selectionModel() && selectionModel()->hasSelection()) ? currentIndex().row() : -1);
}

int PopupViewWidget::getRowCount(const QModelIndex& parent) const
{
	return (model() ? model()->rowCount(parent) : 0);
}

int PopupViewWidget::getColumnCount(const QModelIndex& parent) const
{
	return (model() ? model()->columnCount(parent) : 0);
}

bool PopupViewWidget::canMoveUp() const
{
	return (currentIndex().row() > 0 && getRowCount() > 1);
}

bool PopupViewWidget::canMoveDown() const
{
	const int currentRow(currentIndex().row());
	const int rowCount(getRowCount());

	return (currentRow >= 0 && rowCount > 1 && currentRow < (rowCount - 1));
}

void PopupViewWidget::keyPressEvent(QKeyEvent* event)
{
	if (!m_addressBar) {
		QTreeView::keyPressEvent(event);

		return;
	}

	m_addressBar->event(event);

	if (event->isAccepted()) {
		if (!m_addressBar->hasFocus()) {
			m_addressBar->hidePopup();
		}

		return;
	}

	switch (event->key()) {
	case Qt::Key_Up:
	case Qt::Key_Down:
	case Qt::Key_PageUp:
	case Qt::Key_PageDown:
	case Qt::Key_End:
		QTreeView::keyPressEvent(event);

		return;
	case Qt::Key_Enter:
	case Qt::Key_Return:
	case Qt::Key_Tab:
	case Qt::Key_Backtab:
	case Qt::Key_Escape:
	case Qt::Key_F4:
		if (event->key() == Qt::Key_F4 && !event->modifiers().testFlag(Qt::AltModifier)) {
			break;
		}

		if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
			const QModelIndex index(getCurrentIndex());

			if (index.isValid()) {
				emit clicked(index);
			}
		}

		m_addressBar->hidePopup();

		break;
	default:
		break;
	}
}

void PopupViewWidget::handleIndexEntered(const QModelIndex& index)
{
	if (index.isValid()) {
		setCurrentIndex(index);
	}

	QStatusTipEvent statusTipEvent(index.data(Qt::StatusTipRole).toString());

	QApplication::sendEvent(m_addressBar, &statusTipEvent);
}

void PopupViewWidget::updateHeight()
{
	int completionHeight(5);
	const int rowCount(qMin(20, getRowCount()));

	for (int i = 0; i < rowCount; ++i) {
		completionHeight += sizeHintForRow(i);
	}

	setFixedHeight(completionHeight);
	viewport()->setFixedHeight(completionHeight - 3);
}

bool PopupViewWidget::event(QEvent* event)
{
	switch (event->type()) {
	case QEvent::Close:
	case QEvent::Hide:
	case QEvent::Leave:
		if (m_addressBar) {
			QString statusTip;
			QStatusTipEvent statusTipEvent(statusTip);

			QApplication::sendEvent(m_addressBar, &statusTipEvent);
		}

		break;
	case QEvent::InputMethod:
	case QEvent::ShortcutOverride:
		if (m_addressBar) {
			QApplication::sendEvent(m_addressBar, event);
		}

		break;
	case QEvent::MouseButtonPress:
		if (m_addressBar && !viewport()->underMouse()) {
			m_addressBar->hidePopup();
		}

		break;
	default:
		break;
	}

	return QTreeView::event(event);
}

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
	m_completionModel(new AddressCompletionModel(this)),
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
	m_siteIcon->setIcon(Application::getAppIcon("webpage"));
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
	m_goButton->setIcon(Application::getAppIcon("go"));

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

	QAction* undoAction = new QAction(Application::getAppIcon("edit-undo", "edit"), tr("&Undo"), this);
	undoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Z")));
	undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* redoAction = new QAction(Application::getAppIcon("edit-redo", "edit"), tr("&Redo"), this);
	redoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+Z")));
	redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* cutAction = new QAction(Application::getAppIcon("edit-cut", "edit"), tr("Cu&t"), this);
	cutAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+X")));
	cutAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* copyAction = new QAction(Application::getAppIcon("edit-copy", "edit"), tr("&Copy"), this);
	copyAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+C")));
	copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* pasteAction = new QAction(Application::getAppIcon("edit-past", "edit"), tr("&Paste"), this);
	pasteAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+V")));
	pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* pasteAndGoAction = new QAction(Application::getAppIcon("edit-past", "edit"), tr("Paste and &Go"), this);
	pasteAndGoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+V")));
	pasteAndGoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* deleteAction = new QAction(Application::getAppIcon("edit-delete", "edit"), tr("Delete"), this);

	QAction* clearAllAction = new QAction(Application::getAppIcon("edit-clear", "edit"), tr("Clear All"), this);

	QAction* selectAllAction = new QAction(Application::getAppIcon("edit-select-all", "edit"), tr("Select All"), this);
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
	connect(this, &AddressBar::textChanged, this, &AddressBar::sTextEdited);
	connect(this, &AddressBar::textEdited, m_completionModel, &AddressCompletionModel::setFilter);
	connect(this, &AddressBar::selectionChanged, this, &AddressBar::updateActions);
	connect(m_goButton, &ToolButton::clicked, this, &AddressBar::requestLoadUrl);
	connect(QApplication::clipboard(), &QClipboard::dataChanged, this, &AddressBar::updatePasteActions);
	connect(m_completionModel, &AddressCompletionModel::completionReady, this, &AddressBar::setCompletion);

	updateActions();
	updatePasteActions();
	loadSettings();
	updateSiteIcon();
}

void AddressBar::showPopup()
{
	if (!m_popupViewWidget)
		m_popupViewWidget = new PopupViewWidget(this);

	m_popupViewWidget->updateHeight();
	m_popupViewWidget->move(mapToGlobal(contentsRect().bottomLeft()));
	m_popupViewWidget->setFixedWidth(width());
	m_popupViewWidget->setFocusProxy(this);
	m_popupViewWidget->show();
}

void AddressBar::hidePopup()
{
	if (m_popupViewWidget) {
		m_popupViewWidget->hide();
		m_popupViewWidget->deleteLater();
		m_popupViewWidget = nullptr;
	}
}

bool AddressBar::isPopupVisible() const
{
	return (m_popupViewWidget && m_popupViewWidget->isVisible());
}

PopupViewWidget* AddressBar::getPopup()
{
	if (!m_popupViewWidget)
		m_popupViewWidget = new PopupViewWidget(this);

	return m_popupViewWidget;
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
	if ((event->key() == Qt::Key_Backspace || event->key() == Qt::Key_Delete) && !m_completion.isEmpty())
		m_shouldIgnoreCompletion = true;

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
		if (text().left(1) == "!") {
			QString fullCommand = text();
			fullCommand = fullCommand.right(fullCommand.count() - 1);

			QStringList args = fullCommand.split(" ");
			QString command = args[0];

			args.removeAt(0);
				foreach (const QString& str, args) {
					QMessageBox::information(nullptr, "DEBUG", str);
					if (str == " ")
						args.removeOne(str);
					else if (str.isEmpty())
						args.removeOne(str);
				}

			if (!processMainCommand(command, args))
				Application::instance()->processCommand(command, args);

			showUrl(m_webView->url());
		}
		else if (!isPopupVisible()) {
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

	bool isShortcut = (event->modifiers() & Qt::ControlModifier) && event->key() == Qt::Key_E;
	if (!isShortcut)
		QLineEdit::keyPressEvent(event); // Don't send the shortcut (CTRL-E) to the text edit.

}

void AddressBar::resizeEvent(QResizeEvent* event)
{
	QLineEdit::resizeEvent(event);

	if (m_popupViewWidget) {
		m_popupViewWidget->move(mapToGlobal(contentsRect().bottomLeft()));
		m_popupViewWidget->setFixedWidth(width());
	}
}

void AddressBar::showCompletion()
{
	PopupViewWidget* popupViewWidget{getPopup()};
	popupViewWidget->setModel(m_completionModel);
	popupViewWidget->setItemDelegate(new AddressDelegate(text(), popupViewWidget));

	AddressCompletionModel::CompletionTypes types{AddressCompletionModel::UnknownCompletionType};

	types |= AddressCompletionModel::SearchSuggestionCompletionType;
	types |= AddressCompletionModel::HistoryCompletionType;
	types |= AddressCompletionModel::LocalPathSuggestionsCompletionType;

	m_completionModel->setTypes(types);

	if (!isPopupVisible()) {
		connect(popupViewWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(openUrl(QModelIndex)));

		showPopup();
	}

	popupViewWidget->setCurrentIndex(m_completionModel->index(0, 0));
	popupViewWidget->setFocus();
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

void AddressBar::setCompletion(const QString& filter)
{
	if (filter.isEmpty() || m_completionModel->rowCount() == 0) {
		hidePopup();

		m_completion = QString();

		if (m_shouldIgnoreCompletion)
			m_shouldIgnoreCompletion = false;

		return;
	}

	showCompletion();
}

void AddressBar::sTextEdited(const QString& text)
{
	m_oldTextLength = m_currentTextLength;
	m_currentTextLength = text.length();

	setGoButtonVisible(true);

//	emit textEdited(text);
}

void AddressBar::openUrl(const QModelIndex& index)
{
	hidePopup();

	if (!index.isValid())
		return;

	QString url{};

	if (static_cast<AddressCompletionModel::EntryType>(index.data(AddressCompletionModel::TypeRole).toInt())
		== AddressCompletionModel::SearchSuggestionType) {
		url = index.data(AddressCompletionModel::TextRole).toString();
	}
	else
		url = index.data(AddressCompletionModel::UrlRole).toUrl().toString();

	setText(url);
	requestLoadUrl();
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
	QIcon icon{m_webView ? m_webView->icon() : Application::getAppIcon("webpage")};

	m_siteIcon->setIcon(icon);
}

void AddressBar::setGoButtonVisible(bool state)
{
	m_goButton->setVisible(state);

	updateTextMargins();
}

void AddressBar::loadStarted()
{
	m_siteIcon->setIcon(Application::getAppIcon("webpage"));

	m_reloadStopButton->setToolTip(tr("Stop"));
	m_reloadStopButton->setObjectName("addressbar-button-stop");
	m_reloadStopButton->setIcon(Application::getAppIcon("stop"));

	Application::style()->unpolish(m_reloadStopButton);
	Application::style()->polish(m_reloadStopButton);
}

void AddressBar::loadProgress(int progress)
{
	Q_UNUSED(progress);
	// TODO: show loading in address bar
}

void AddressBar::loadFinished()
{
	updateSiteIcon();

	m_reloadStopButton->setToolTip(tr("Reload"));
	m_reloadStopButton->setObjectName("addressbar-button-reload");
	m_reloadStopButton->setIcon(Application::getAppIcon("reload"));

	Application::style()->unpolish(m_reloadStopButton);
	Application::style()->polish(m_reloadStopButton);
}

void AddressBar::loadFromCompleter(QString& text)
{
	Q_UNUSED(text);
	//TODO: do
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

bool AddressBar::processMainCommand(const QString& command, const QStringList& args)
{

	bool succes{false};

	if (command == "newtab") {
		LoadRequest request{};
		if (args.count() == 1)
			request.setUrl(args[0]);
		else
			request.setUrl(webView()->webTab()->tabBar()->tabWidget()->urlOnNewTab());

		webView()->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);

		succes = true;
	}
	else if (command == "closetab") {
		webView()->webTab()->tabBar()->tabWidget()
			->requestCloseTab(webView()->webTab()->tabBar()->tabWidget()->currentIndex());

		succes = true;
	}
	else if (command == "reload") {
		if (args.count() == 1)
			webView()->webTab()->tabBar()->tabWidget()->reloadAllTabs();
		else
			webView()->reload();
	}

	return succes;
}

}
