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

#include "LineEdit.hpp"

#include <QMenu>

#include <QClipboard>

#include "Application.hpp"

namespace Sn
{
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

LineEdit::LineEdit(QWidget* parent):
	QLineEdit(parent)
{
	mainLayout = new QHBoxLayout(this);
	mainLayout->setContentsMargins(0, 0, 0, 0);
	mainLayout->setSpacing(0);

	m_leftWidget = new SideWidget(this);
	m_leftWidget->resize(0, 0);
	m_leftLayout = new QHBoxLayout(m_leftWidget);
	m_leftLayout->setContentsMargins(0, 0, 0, 0);
	m_leftLayout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);

	m_rightWidget = new SideWidget(this);
	m_rightWidget->resize(0, 0);
	m_rightLayout = new QHBoxLayout(m_rightWidget);
	m_rightLayout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);
	m_rightLayout->setContentsMargins(0, 0, 2, 0);

	mainLayout->addWidget(m_leftWidget, 0, static_cast<Qt::Alignment>(Qt::AlignVCenter | Qt::AlignLeft));
	mainLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
	mainLayout->addWidget(m_rightWidget, 0, static_cast<Qt::Alignment>(Qt::AlignVCenter | Qt::AlignRight));
	mainLayout->setDirection(isRightToLeft() ? QBoxLayout::RightToLeft : QBoxLayout::LeftToRight);

	setWidgetSpacing(3);

	connect(m_leftWidget, &SideWidget::sizeHintChanged, this, &LineEdit::updateTextMargins);
	connect(m_rightWidget, &SideWidget::sizeHintChanged, this, &LineEdit::updateTextMargins);

	QAction* undoAction{new QAction(Application::getAppIcon(QStringLiteral("edit-undo"), "edit"), tr("&Undo"), this)};
	undoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Z")));
	undoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(undoAction, &QAction::triggered, this, &LineEdit::undo);

	QAction* redoAction = new QAction(Application::getAppIcon(QStringLiteral("edit-redo"), "edit"), tr("&Redo"), this);
	redoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+Z")));
	redoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(redoAction, &QAction::triggered, this, &LineEdit::redo);

	QAction* cutAction = new QAction(Application::getAppIcon(QStringLiteral("edit-cut"), "edit"), tr("Cu&t"), this);
	cutAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+X")));
	cutAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(cutAction, &QAction::triggered, this, &LineEdit::cut);

	QAction* copyAction = new QAction(Application::getAppIcon(QStringLiteral("edit-copy"), "edit"), tr("&Copy"), this);
	copyAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+C")));
	copyAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(copyAction, &QAction::triggered, this, &LineEdit::copy);

	QAction* pasteAction = new QAction(Application::getAppIcon(QStringLiteral("edit-paste"), "edit"), tr("&Paste"), this);
	pasteAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+V")));
	pasteAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(pasteAction, &QAction::triggered, this, &LineEdit::paste);

	QAction* pasteAndGoAction = new QAction(this);
	pasteAndGoAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+V")));
	pasteAndGoAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);

	QAction* deleteAction = new QAction(Application::getAppIcon(QStringLiteral("edit-delete"), "edit"), tr("Delete"),
	                                    this);
	connect(deleteAction, &QAction::triggered, this, &LineEdit::slotDelete);

	QAction* clearAllAction = new QAction(Application::getAppIcon(QStringLiteral("edit-clear"), "edit"), tr("Clear All"),
	                                      this);
	connect(clearAllAction, &QAction::triggered, this, &LineEdit::clear);

	QAction* selectAllAction = new QAction(Application::getAppIcon(QStringLiteral("edit-select-all"), "edit"),
	                                       tr("Select All"), this);
	selectAllAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+A")));
	selectAllAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(selectAllAction, &QAction::triggered, this, &LineEdit::selectAll);

	m_editActions[Undo] = undoAction;
	m_editActions[Redo] = redoAction;
	m_editActions[Cut] = cutAction;
	m_editActions[Copy] = copyAction;
	m_editActions[Paste] = pasteAction;
	m_editActions[PasteAndGo] = pasteAndGoAction;
	m_editActions[Delete] = deleteAction;
	m_editActions[ClearAll] = clearAllAction;
	m_editActions[SelectAll] = selectAllAction;


	addAction(undoAction);
	addAction(redoAction);
	addAction(cutAction);
	addAction(copyAction);
	addAction(pasteAction);
	addAction(pasteAndGoAction);
	addAction(deleteAction);
	addAction(clearAllAction);
	addAction(selectAllAction);

	connect(this, &QLineEdit::textChanged, this, &LineEdit::updateActions);
	connect(this, &QLineEdit::selectionChanged, this, &LineEdit::updateActions);

	updateActions();
}

void LineEdit::addWidget(QWidget* widget, WidgetPosition position)
{
	if (!widget)
		return;

	if (position == LeftSide)
		m_leftLayout->addWidget(widget);
	else
		m_rightLayout->addWidget(widget);
}

void LineEdit::removeWidget(QWidget* widget)
{
	if (!widget)
		return;

	m_leftLayout->removeWidget(widget);
	m_rightLayout->removeWidget(widget);
	widget->hide();
}

void LineEdit::setWidgetSpacing(int spacing)
{
	m_leftLayout->setSpacing(spacing);
	m_rightLayout->setSpacing(spacing);
	updateTextMargins();
}

int LineEdit::widgetSpacing() const
{
	return m_leftLayout->spacing();
}

void LineEdit::setTextFormat(const QList<QTextLayout::FormatRange>& format)
{
	QList<QInputMethodEvent::Attribute> attributes{};

	foreach(const QTextLayout::FormatRange &fr, format) {
		QInputMethodEvent::AttributeType type{QInputMethodEvent::TextFormat};
		int start{fr.start - cursorPosition()};
		int length{fr.length};
		QVariant value{fr.format};
		attributes.append(QInputMethodEvent::Attribute(type, start, length, value));
	}

	QInputMethodEvent ev(QString(), attributes);
	event(&ev);
}

void LineEdit::clearTextFormat()
{
	setTextFormat(QList<QTextLayout::FormatRange>());
}

void LineEdit::setMinHeight(int height)
{
	m_minHeight = height;
}

QSize LineEdit::sizeHint() const
{
	QSize size{QLineEdit::sizeHint()};

	if (size.height() < m_minHeight)
		size.setHeight(m_minHeight);

	return size;
}

QAction *LineEdit::editAction(EditAction action) const
{
	return m_editActions[action];
}

void LineEdit::setLeftMargin(int margin)
{
	m_leftMargin = margin;
}

void LineEdit::updateTextMargins()
{
	int left{m_leftWidget->sizeHint().width()};
	int right{m_rightWidget->sizeHint().width()};
	int top{0};
	int bottom{0};

	if (m_leftMargin >= 0)
		left = m_leftMargin;

	setTextMargins(left, top, right, bottom);
}

void LineEdit::focusInEvent(QFocusEvent* event)
{
	if (event->reason() == Qt::MouseFocusReason) {
		m_ignoreMousePress = true;
		selectAll();
	}

	QLineEdit::focusInEvent(event);
}

void LineEdit::mousePressEvent(QMouseEvent* event)
{
	if (m_ignoreMousePress) {
		m_ignoreMousePress = false;
		return;
	}

	QLineEdit::mousePressEvent(event);
}

void LineEdit::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->buttons() == Qt::LeftButton) {
		selectAll();
		return;
	}

	QLineEdit::mouseDoubleClickEvent(event);
}

void LineEdit::resizeEvent(QResizeEvent* event)
{
	QLineEdit::resizeEvent(event);

	m_leftWidget->setFixedHeight(height());
	m_rightWidget->setFixedHeight(height());
}

bool LineEdit::event(QEvent* event)
{
	if (event->type() == QEvent::LayoutDirectionChange) {
		if (isRightToLeft()) {
			mainLayout->setDirection(QBoxLayout::RightToLeft);
			m_leftLayout->setDirection(QBoxLayout::RightToLeft);
			m_rightLayout->setDirection(QBoxLayout::RightToLeft);
		}
		else {
			mainLayout->setDirection(QBoxLayout::LeftToRight);
			m_leftLayout->setDirection(QBoxLayout::LeftToRight);
			m_rightLayout->setDirection(QBoxLayout::LeftToRight);
		}
	}

	return QLineEdit::event(event);
}

QMenu *LineEdit::createContextMenu()
{
	QMenu* popup{new QMenu(this)};
	popup->setObjectName(QLatin1String("edit-menu"));

	if (!isReadOnly()) {
		popup->addAction(m_editActions[Undo]);
		popup->addAction(m_editActions[Redo]);
		popup->addSeparator();
		popup->addAction(m_editActions[Cut]);
	}

	popup->addAction(m_editActions[Copy]);

	if (!isReadOnly()) {
		updatePasteActions();
		popup->addAction(m_editActions[Paste]);
		if (!m_editActions[PasteAndGo]->text().isEmpty()) {
			popup->addAction(m_editActions[PasteAndGo]);
		}
		popup->addAction(m_editActions[Delete]);
		popup->addAction(m_editActions[ClearAll]);
	}

	popup->addSeparator();
	popup->addAction(m_editActions[SelectAll]);

	return popup;
}

void LineEdit::updateActions()
{
	m_editActions[Undo]->setEnabled(!isReadOnly() && isUndoAvailable());
	m_editActions[Redo]->setEnabled(!isReadOnly() && isRedoAvailable());
	m_editActions[Cut]->setEnabled(!isReadOnly() && hasSelectedText() && echoMode() == QLineEdit::Normal);
	m_editActions[Copy]->setEnabled(hasSelectedText() && echoMode() == QLineEdit::Normal);
	m_editActions[Delete]->setEnabled(!isReadOnly() && hasSelectedText());
	m_editActions[SelectAll]->setEnabled(!text().isEmpty() && selectedText() != text());
	m_editActions[Paste]->setEnabled(true);
	m_editActions[PasteAndGo]->setEnabled(true);
}

void LineEdit::updatePasteActions()
{
	bool pasteEnabled{!isReadOnly() && !QApplication::clipboard()->text().isEmpty()};

	m_editActions[Paste]->setEnabled(pasteEnabled);
	m_editActions[PasteAndGo]->setEnabled(pasteEnabled);
}

void LineEdit::slotDelete()
{
	if (hasSelectedText())
		del();
}
}
