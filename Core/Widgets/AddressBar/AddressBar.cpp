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

#include "AddressBar.hpp"

#include <QSpacerItem>
#include <QMenu>

#include <QCompleter>

#include <QMimeData>
#include <QAbstractItemView>

#include <QApplication>
#include <QClipboard>

#include <QIcon>

#include <QAbstractTextDocumentLayout>
#include <QTextBlock>
#include <QStyle>
#include <QMessageBox>

#include <QHeaderView>

#include "History/HistoryItem.hpp"

#include "Web/LoadRequest.hpp"
#include "Web/Tab/TabbedWebView.hpp"
#include "Web/Tab/WebTab.hpp"

#include "Widgets/SiteInfo.hpp"
#include "Widgets/SiteIcon.hpp"
#include "Widgets/AddressBar/AddressBarCompleter.hpp"
#include "Widgets/Tab/TabWidget.hpp"
#include "Widgets/Tab/MainTabBar.hpp"
#include "Widgets/Tab/TabBar.hpp"

#include "Utils/ToolButton.hpp"

#include "BrowserWindow.hpp"
#include "Application.hpp"

namespace Sn
{

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

AddressBar::AddressBar(TabWidget* tabWidget) :
	LineEdit(tabWidget),
	m_tabWidget(tabWidget)
{
	setObjectName("addressbar");
	setDragEnabled(true);

	m_siteIcon = new SiteIcon(m_tabWidget, this);

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

	addWidget(m_siteIcon, LineEdit::LeftSide);
	addWidget(m_goButton, LineEdit::RightSide);
	addWidget(m_reloadStopButton, LineEdit::RightSide);

	connect(m_reloadStopButton, &ToolButton::clicked, this, &AddressBar::reloadStopClicked);

	// TODO: potential issue...
	m_completer = new AddressBarCompleter(this);
	m_completer->setTabWidget(m_tabWidget);
	m_completer->setAddressBar(this);
	connect(m_completer, &AddressBarCompleter::showCompletion, this, &AddressBar::showCompletion);
	connect(m_completer, &AddressBarCompleter::showDomainCompletion, this, &AddressBar::showDomainCompletion);
	connect(m_completer, &AddressBarCompleter::loadCompletion, this, &AddressBar::requestLoadUrl);
	connect(m_completer, &AddressBarCompleter::clearCompletion, this, &AddressBar::clearCompletion);

	m_domainCompleterModel = new QStringListModel(this);
	QCompleter* domainCompleter{new QCompleter(this)};
	domainCompleter->setCompletionMode(QCompleter::InlineCompletion);
	domainCompleter->setModel(m_domainCompleterModel);

	setCompleter(domainCompleter);

	connect(this, &QLineEdit::textEdited, this, &AddressBar::textEdited);
	connect(m_goButton, &ToolButton::clicked, this, &AddressBar::requestLoadUrl);

	loadSettings();
	updateSiteIcon();

	m_goButton->setVisible(false);
	this->setPlaceholderText(tr("Enter a search or an address"));
}

void AddressBar::setWebView(TabbedWebView* view)
{
	m_webView = view;

	m_siteIcon->setWebView(m_webView);

	connect(m_webView, &TabbedWebView::loadStarted, this, &AddressBar::loadStarted);
	connect(m_webView, &TabbedWebView::loadProgress, this, &AddressBar::loadProgress);
	connect(m_webView, &TabbedWebView::loadFinished, this, &AddressBar::loadFinished);
	connect(m_webView, &TabbedWebView::iconChanged, this, &AddressBar::updateSiteIcon);

	connect(m_webView, SIGNAL(urlChanged(QUrl)), this, SLOT(showUrl(QUrl)));
}


void AddressBar::setTabWidget(TabWidget* tabWidget)
{
	m_tabWidget = tabWidget;
	m_completer->setTabWidget(m_tabWidget);
}

void AddressBar::setText(const QString& text)
{
	if (text.contains("doosearch.sielo.app")) {
		clear();
		return;
	}

	m_oldTextLength = text.length();
	m_currentTextLength = m_oldTextLength;

	LineEdit::setText(text);

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

	// TODO: check bookmark
}

void AddressBar::textEdited(const QString& text)
{
	m_oldTextLength = m_currentTextLength;
	m_currentTextLength = text.length();

	if (!text.isEmpty())
		m_completer->complete(text);
	else
		m_completer->closePopup();

	setGoButtonVisible(true);
}

void AddressBar::requestLoadUrl()
{
	const LoadRequest request{createLoadRequest()};
	const QString urlString{urlToText(request.url())};

	m_completer->closePopup();
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
	if (m_webView && SiteInfo::canShowSiteInfo(m_webView->url())) {
		if (m_webView && m_webView->url().scheme() == QLatin1String("https"))
			m_siteIcon->updateIcon(true);
		else
			m_siteIcon->updateIcon(false);
	}
	else
		m_siteIcon->setIcon(Application::getAppIcon("webpage"));
}

void AddressBar::setGoButtonVisible(bool state)
{
	m_goButton->setVisible(state);

	updateTextMargins();
}

void AddressBar::showCompletion(const QString& completion, bool completeDomain)
{
	LineEdit::setText(completion);

	end(false);

	if (completeDomain)
		completer()->complete();
}

void AddressBar::showDomainCompletion(const QString& completion)
{
	m_domainCompleterModel->setStringList(QStringList() << completion);

	if (m_oldTextLength < m_currentTextLength)
		completer()->complete();
}

void AddressBar::clearCompletion()
{
	m_webView->setFocus();
	showUrl(m_webView->url());
}


void AddressBar::loadStarted()
{
	m_reloadStopButton->setToolTip(tr("Stop"));
	m_reloadStopButton->setObjectName("addressbar-button-stop");
	m_reloadStopButton->setIcon(Application::getAppIcon("stop"));

	m_siteIcon->setIcon(Application::getAppIcon("webpage"));

	Application::style()->unpolish(m_reloadStopButton);
	Application::style()->polish(m_reloadStopButton);
}

void AddressBar::loadProgress(int progress)
{
	Q_UNUSED(progress);
}

void AddressBar::loadFinished()
{
	m_reloadStopButton->setToolTip(tr("Reload"));
	m_reloadStopButton->setObjectName("addressbar-button-reload");
	m_reloadStopButton->setIcon(Application::getAppIcon("reload"));

	updateSiteIcon();

	Application::style()->unpolish(m_reloadStopButton);
	Application::style()->polish(m_reloadStopButton);
}


void AddressBar::loadSettings()
{
	//TODO: nothing to do now
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
	LineEdit::showEvent(event);

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
	LineEdit::focusInEvent(event);
}

void AddressBar::focusOutEvent(QFocusEvent* event)
{
	if (event->reason() == Qt::PopupFocusReason)
		return;

	LineEdit::focusOutEvent(event);

	setGoButtonVisible(false);

	if (text().trimmed().isEmpty())
		clear();

	refreshTextFormat();
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
			LineEdit::focusOutEvent(&evnt);

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

	LineEdit::dropEvent(event);
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

	case Qt::Key_Down:
		m_completer->complete(text());
		break;
	case Qt::Key_Left:
		m_completer->closePopup();
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
		// TODO: enable again command system
		switch (event->modifiers()) {
		case Qt::ControlModifier:
			if (!text().endsWith(QLatin1String(".com")))
				setText(text().append(QLatin1String(".com")));

			requestLoadUrl();
			m_holdingAlt = false;
			break;

		case Qt::AltModifier:
			m_completer->closePopup();
			m_tabWidget->addView(createLoadRequest());
			m_holdingAlt = false;
			break;

		default:
			if (text().left(1) == "!") {
				QString fullCommand = text();
				fullCommand = fullCommand.right(fullCommand.count() - 1);

				QStringList args = fullCommand.split(" ");
				QString command = args[0];

				args.removeAt(0);
				foreach(const QString& str, args)
				{
					if (str == " ")
						args.removeOne(str);
					else if (str.isEmpty())
						args.removeOne(str);
				}

				if (!processMainCommand(command, args))
					Application::instance()->processCommand(command, args);

				showUrl(m_webView->url());
				return;
			}

			requestLoadUrl();
			m_holdingAlt = false;
			break;
		}

		/*
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
				}*/

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

	LineEdit::keyPressEvent(event);
}

void AddressBar::mousePressEvent(QMouseEvent* event)
{
	emit m_tabWidget->focusIn(m_tabWidget);

	LineEdit::mousePressEvent(event);
}

LoadRequest AddressBar::createLoadRequest() const
{
	LoadRequest request{};
	const QString& text{this->text().trimmed()};

	//TODO: add shortcuts

	if (request.isEmpty()) {
		if (!text.contains(QLatin1Char(' ')) && !text.contains(QLatin1Char('.')))
			request.setUrl(text);
		else {
			const QUrl& guessed{QUrl::fromUserInput(text)};

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

	QList<QTextLayout::FormatRange> textFormat{};
	QString hostName{m_webView->url().isEmpty() ? QUrl(text()).host() : m_webView->url().host()};

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
			request.setUrl(webView()->webTab()->tabWidget()->urlOnNewTab());

		webView()->loadInNewTab(request, Application::NTT_CleanSelectedTabAtEnd);

		succes = true;
	}
	else if (command == "closetab") {
		webView()->webTab()->tabWidget()
			->requestCloseTab(webView()->webTab()->tabWidget()->currentIndex());

		succes = true;
	}
	else if (command == "reload") {
		if (args.count() == 1)
			webView()->webTab()->tabWidget()->reloadAllTabs();
		else
			webView()->reload();
	}

	return succes;
}
}
