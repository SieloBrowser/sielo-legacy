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

#include "SearchToolBar.hpp"

#include <QPointer>

#include <QStyle>

#include <QShortcut>
#include <QKeyEvent>

#include "Web/WebView.hpp"
#include "Web/WebPage.hpp"

namespace Sn {

FocusSelectLineEdit::FocusSelectLineEdit(QWidget* parent) :
	QLineEdit(parent),
	m_mouseFocusReason(false)
{
	// Empty
}

void FocusSelectLineEdit::setFocus()
{
	selectAll();

	QLineEdit::setFocus();
}

void FocusSelectLineEdit::focusInEvent(QFocusEvent* event)
{
	m_mouseFocusReason = event->reason() == Qt::MouseFocusReason;
	selectAll();

	QLineEdit::focusInEvent(event);
}

void FocusSelectLineEdit::mousePressEvent(QMouseEvent* event)
{
	if (m_mouseFocusReason) {
		m_mouseFocusReason = false;
		return;
	}

	QLineEdit::mousePressEvent(event);
}

SearchToolBar::SearchToolBar(WebView* view, QWidget* parent) :
	QWidget(parent),
	m_view(view)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setObjectName(QLatin1String("search-toolbar"));

	setupUI();

	connect(m_closeButton, &QPushButton::clicked, this, &SearchToolBar::close);
	connect(m_toSearch, &FocusSelectLineEdit::textChanged, this, &SearchToolBar::findNext);
	connect(m_toSearch, &FocusSelectLineEdit::returnPressed, this, &SearchToolBar::findNext);
	connect(m_next, &QPushButton::clicked, this, &SearchToolBar::findNext);
	connect(m_previous, &QPushButton::clicked, this, &SearchToolBar::findPrevious);
	connect(m_caseSensitive, &QPushButton::clicked, this, &SearchToolBar::caseSensitivityChanged);

	QShortcut* findNextAction = new QShortcut(QKeySequence("F3"), this);
	QShortcut* findPreviousAction = new QShortcut(QKeySequence("Shift+F3"), this);

	connect(findNextAction, &QShortcut::activated, this, &SearchToolBar::findNext);
	connect(findPreviousAction, &QShortcut::activated, this, &SearchToolBar::findPrevious);

	parent->installEventFilter(this);
}

SearchToolBar::~SearchToolBar()
{
	// Empty
}

void SearchToolBar::showMinimalInPopupWindow()
{
	m_caseSensitive->hide();
	m_results->hide();

	m_layout->setSpacing(2);
	m_layout->setContentsMargins(2, 6, 2, 6);

	setMinimumWidth(260);
}

void SearchToolBar::focusSearchLine()
{
	m_toSearch->setFocus();
}

bool SearchToolBar::eventFilter(QObject* object, QEvent* event)
{
	Q_UNUSED(object)

	if (event->type() == QEvent::KeyPress && static_cast<QKeyEvent*>(event)->key() == Qt::Key_Escape)
		close();

	return false;
}

void SearchToolBar::searchText(const QString& text)
{
	QPointer<SearchToolBar> guard = this;

	m_view->findText(text, m_findFlags, [=](bool found)
	{
		if (!guard)
			return;

		if (m_toSearch->text().isEmpty())
			found = true;

		if (!found)
			m_results->setText(tr("No results found."));
		else
			m_results->clear();

		m_toSearch->setProperty("notfound", QVariant(!found));
		m_toSearch->style()->unpolish(m_toSearch);
		m_toSearch->style()->polish(m_toSearch);

		m_view->page()->runJavaScript(QStringLiteral("window.getSelection().empty();"));
	});
}

void SearchToolBar::updateFindFlags()
{
	if (m_caseSensitive->isChecked())
		m_findFlags = m_findFlags | Engine::WebPage::FindCaseSensitively;
	else
		m_findFlags = m_findFlags & ~Engine::WebPage::FindCaseSensitively;
}

void SearchToolBar::caseSensitivityChanged()
{
	updateFindFlags();

	searchText(QString());
	searchText(m_toSearch->text());
}

void SearchToolBar::findNext()
{
	m_findFlags = 0;
	updateFindFlags();

	searchText(m_toSearch->text());
}

void SearchToolBar::findPrevious()
{
	m_findFlags = Engine::WebPage::FindBackward;
	updateFindFlags();

	searchText(m_toSearch->text());
}

void SearchToolBar::close()
{
	hide();
	searchText(QString());

	m_view->setFocus();

	deleteLater();
}

void SearchToolBar::setupUI()
{
	resize(855, 40);

	QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	sizePolicy.setHorizontalStretch(0);
	sizePolicy.setVerticalStretch(0);

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(-1, 4, -1, 4);

	m_closeButton = new QPushButton(this);
	m_closeButton->setFlat(true);
	m_closeButton->setIcon(Application::getAppIcon("close"));
	sizePolicy.setHeightForWidth(m_closeButton->sizePolicy().hasHeightForWidth());
	m_closeButton->setSizePolicy(sizePolicy);

	m_desc = new QLabel(tr("Search:"), this);

	m_toSearch = new FocusSelectLineEdit(this);
	m_toSearch->setMinimumSize(QSize(100, 0));
	m_toSearch->setMaximumSize(QSize(200, 15545484));
	m_toSearch->setPlaceholderText(tr("Search..."));

	m_previous = new QPushButton(this);
	m_previous->setFlat(true);
	m_previous->setIcon(Application::getAppIcon("arrow-up"));
	sizePolicy.setHeightForWidth(m_previous->sizePolicy().hasHeightForWidth());
	m_previous->setSizePolicy(sizePolicy);
	m_previous->setShortcut(QStringLiteral("Ctrl+Shift+G"));

	m_next = new QPushButton(this);
	m_next->setFlat(true);
	m_next->setIcon(Application::getAppIcon("arrow-down"));
	sizePolicy.setHeightForWidth(m_next->sizePolicy().hasHeightForWidth());
	m_next->setSizePolicy(sizePolicy);
	m_next->setShortcut(QStringLiteral("Ctrl+G"));

	m_caseSensitive = new QPushButton(tr("&Match Case"), this);
	m_caseSensitive->setCheckable(true);

	m_results = new QLabel(this);

	m_spacer = new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_layout->addWidget(m_closeButton);
	m_layout->addWidget(m_desc);
	m_layout->addWidget(m_toSearch);
	m_layout->addWidget(m_previous);
	m_layout->addWidget(m_next);
	m_layout->addWidget(m_caseSensitive);
	m_layout->addWidget(m_results);
	m_layout->addItem(m_spacer);
}

}