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

#include "Web/WebPage.hpp"

#include <QPointer>
#include <QTimer>
#include <QDesktopServices>
#include <QFileInfo>

#include <QWebChannel>

#include <QMessageBox>

#include "Web/WebHitTestResult.hpp"
#include "Web/WebView.hpp"
#include "Web/Tab/TabbedWebView.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"
#include "Web/Scripts.hpp"

#include "Download/DownloadManager.hpp"

#include "AdBlock/Manager.hpp"

#include "Password/AutoFill/AutoFill.hpp"

#include "Network/NetworkManager.hpp"

#include "Widgets/CheckBoxDialog.hpp"
#include "Widgets/Tab/TabWidget.hpp"

#include "Utils/DelayedFileWatcher.hpp"
#include "Utils/ExternalJsObject.hpp"
#include "Utils/Settings.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Application.hpp"
#include "BrowserWindow.hpp"

namespace Sn {

QString WebPage::setCSS(const QString& css)
{
	QString source = QLatin1String("(function() {"
									   "var head = document.getElementsByTagName('head')[0];"
									   "if (!head) return;"
									   "var css = document.createElement('style');"
									   "css.setAttribute('type', 'text/css');"
									   "css.appendChild(document.createTextNode('%1'));"
									   "head.appendChild(css);"
									   "})()");

	QString style = css;
	style.replace(QLatin1String("'"), QLatin1String("\\'"));
	style.replace(QLatin1String("\n"), QLatin1String("\\n"));
	return source.arg(style);
}

WebPage::WebPage(QObject* parent) :
	Engine::WebPage(Application::instance()->webProfile(), parent),
	m_runningLoop(nullptr),
	m_loadProgress(-1),
	m_blockAlerts(false),
	m_secureStatus(false),
	m_adjustingSheduled(false)
{

	setupWebChannel();

	connect(this, &Engine::WebPage::loadProgress, this, &WebPage::progress);
	connect(this, &Engine::WebPage::loadFinished, this, &WebPage::finished);
	connect(this, &Engine::WebPage::urlChanged, this, &WebPage::urlChanged);
	connect(this, &Engine::WebPage::featurePermissionRequested, this, &WebPage::featurePermissionRequested);
	connect(this, &Engine::WebPage::windowCloseRequested, this, &WebPage::windowCloseRequested);

	connect(this, &Engine::WebPage::authenticationRequired, this, [this](const QUrl& url, QAuthenticator* authenticator)
	{
		Application::instance()->networkManager()->authentication(url, authenticator, view());
	});

	// Workaround for broken load started/finished signals in QtWebEngine 5.10
	if (qstrncmp(qVersion(), "5.10.", 5) == 0) {
		connect(this, &Engine::WebPage::loadProgress, this, [this](int progress) {
			if (progress == 100) {
				emit loadFinished(true);
			}
		});
	}

	connect(this,
			&Engine::WebPage::proxyAuthenticationRequired,
			this,
			[this](const QUrl& url, QAuthenticator* authenticator, const QString& proxyHost)
			{
				Q_UNUSED(url);
				Application::instance()->networkManager()->proxyAuthentication(proxyHost, authenticator, view());
			});
}

WebPage::~WebPage()
{
	Application::instance()->plugins()->emitWebPageDeleted(this);

	if (m_runningLoop) {
		m_runningLoop->exit(1);
		m_runningLoop = nullptr;
	}
}

WebView* WebPage::view() const
{
	return static_cast<WebView*>(Engine::WebPage::view());
}

bool WebPage::execPrintPage(QPrinter* printer, int timeout)
{
	QPointer<QEventLoop> loop{new QEventLoop};
	bool result{false};

	QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

	print(printer, [loop, &result](bool res) {
		if (loop && loop->isRunning()) {
			result = res;
			loop->quit();
		}
	});

	loop->exec();
	delete loop;

	return result;
}

QVariant WebPage::executeJavaScript(const QString& scriptSrc, quint32 worldId, int timeout)
{
	QPointer<QEventLoop> loop{new QEventLoop};
	QVariant result{};
	QTimer::singleShot(timeout, loop.data(), &QEventLoop::quit);

	runJavaScript(scriptSrc, worldId, [loop, &result](const QVariant& res)
	{
		if (loop && loop->isRunning()) {
			result = res;
			loop->quit();
		}
	});

	loop->exec();
	delete loop;

	return result;
}

QPointF WebPage::mapToViewport(const QPointF& pos) const
{
	return QPointF(pos.x() / zoomFactor(), pos.y() / zoomFactor());
}

WebHitTestResult WebPage::hitTestContent(const QPoint& pos) const
{
	return WebHitTestResult(this, pos);
}

void WebPage::scroll(int x, int y)
{
	runJavaScript(QStringLiteral("window.scrollTo(window.scrollX + %1, window.scrollY + %2)").arg(x).arg(y),
				  Engine::WebProfile::ScriptWorldId::ApplicationWorld);
}

void WebPage::setScrollPosition(const QPointF& pos)
{
	const QPointF v{mapToViewport(pos.toPoint())};
	runJavaScript(QStringLiteral("window.scrollTo(%1, %2)").arg(v.x()).arg(v.y()));
}

void WebPage::javaScriptAlert(const QUrl& securityOrigin, const QString& msg)
{
	Q_UNUSED(securityOrigin)

	if (m_blockAlerts || m_runningLoop)
		return;

	QString title{tr("JavaScript Alert")};

	if (!url().host().isEmpty())
		title.append(QString(" - %1").arg(url().host()));

	CheckBoxDialog dialog(QMessageBox::Ok, view());
	dialog.setWindowTitle(title);
	dialog.setText(msg);
	dialog.setCheckBoxText(tr("Prevent this page from creating additional dialogues box."));
	dialog.setIcon(QMessageBox::Information);
	dialog.exec();

	m_blockAlerts = dialog.isChecked();
}

bool WebPage::isRunningLoop()
{
	return m_runningLoop;
}

bool WebPage::isLoading() const
{
	return m_loadProgress < 100;
}

void WebPage::setupWebChannel()
{
	QWebChannel* old = webChannel();
	const QString objectName = QStringLiteral("sielo_object");
	QWebChannel* channel{new QWebChannel(this)};

	channel->registerObject(QStringLiteral("sielo_object"), new ExternalJsObject(this));
	setWebChannel(channel);

	if (old) {
		delete old->registeredObjects().value(objectName);
		delete old;
	}
}

void WebPage::progress(int progression)
{
	m_loadProgress = progression;

	bool secStatus = url().scheme() == QLatin1String("https");

	if (secStatus != m_secureStatus) {
		m_secureStatus = secStatus;
		emit privacyChanged(secStatus);
	}
}

void WebPage::finished()
{
	progress(100);

	if (m_adjustingSheduled) {
		m_adjustingSheduled = false;
		setZoomFactor(zoomFactor() + 1);
		setZoomFactor(zoomFactor() - 1);
	}

	if (url().scheme() == QLatin1String("file")) {
		QFileInfo info{url().toLocalFile()};

		if (info.isFile()) {
			if (!m_fileWatcher) {
				m_fileWatcher = new DelayedFileWatcher(this);
				connect(m_fileWatcher, &DelayedFileWatcher::delayedFileChanged, this, &WebPage::watchedFileChanged);
			}

			const QString filePath{url().toLocalFile()};

			if (QFile::exists(filePath) && !m_fileWatcher->files().contains(filePath))
				m_fileWatcher->addPath(filePath);

		}
		else if (m_fileWatcher && !m_fileWatcher->files().isEmpty())
			m_fileWatcher->removePaths(m_fileWatcher->files());
	}

	cleanBlockedObject();

	m_passwordEntries = Application::instance()->autoFill()->completePage(this, url());

	emit pageRendering();
}

void WebPage::cleanBlockedObject()
{
	ADB::Manager* manager = ADB::Manager::instance();

	if (!manager->isEnabled())
		return;

	const QString elementHiding{manager->elementHidingRules(url())};

	if (!elementHiding.isEmpty())
		runJavaScript(WebPage::setCSS(elementHiding), Engine::WebProfile::ScriptWorldId::ApplicationWorld);

	const QString siteElementHiding{manager->elementHidingRulesForDomain(url())};

	if (!siteElementHiding.isEmpty())
		runJavaScript(WebPage::setCSS(siteElementHiding), Engine::WebProfile::ScriptWorldId::ApplicationWorld);
}

void WebPage::urlChanged(const QUrl& url)
{

	Q_UNUSED(url)

	if (isLoading())
		m_blockAlerts = false;
}

void WebPage::watchedFileChanged(const QString& file)
{
	if (url().toLocalFile() == file)
		triggerAction(Engine::WebPage::Reload);
}

void WebPage::windowCloseRequested()
{
	if (!view())
		return;

	view()->closeView();
}

void WebPage::featurePermissionRequested(const QUrl& origin, const Engine::WebPage::Feature& feature)
{
	if (feature == MouseLock && view()->isFullScreen())
		setFeaturePermission(origin, feature, PermissionGrantedByUser);
	else
		Application::instance()->permissionsManager()->requestPermissions(this, origin, feature);
}

bool WebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	if (Application::instance()->isClosing())
		return Engine::WebPage::acceptNavigationRequest(url, type, isMainFrame);

	if (!Application::instance()->plugins()->acceptNavigationRequest(this, url, type, isMainFrame))
		return false;
	
	if (url.scheme() == QLatin1String("abp") && ADB::Manager::instance()->addSubscriptionFromUrl(url))
		return false;

	return Engine::WebPage::acceptNavigationRequest(url, type, isMainFrame);
}

Engine::WebPage* WebPage::createNewWindow(Engine::WebPage::WebWindowType type)
{
	TabbedWebView* tabbedWebView = qobject_cast<TabbedWebView*>(view());
	BrowserWindow* window = tabbedWebView ? tabbedWebView->webTab()->tabWidget()->window() : Application::instance()->getWindow();

	auto createTab = [=](Application::NewTabTypeFlags tabType)
	{
		int index{window->tabWidget()->addView(QUrl(), tabType)};
		TabbedWebView* view{window->tabWidget()->webTab(index)->webView()};
		view->setPage(new WebPage);
		return view->page();
	};

	switch (type) {
	case Engine::WebPage::WebBrowserWindow: {
		BrowserWindow* window{Application::instance()->createWindow(Application::WT_NewWindow)};
		WebPage* page{new WebPage};
		window->setStartPage(page);
		return page;
	}
	case Engine::WebPage::WebDialog: //TODO: do
	case Engine::WebPage::WebBrowserTab:
		return createTab(Application::NTT_CleanSelectedTab);
	case Engine::WebPage::WebBrowserBackgroundTab:
		return createTab(Application::NTT_CleanNotSelectedTab);
	default:
		break;
	}

	return Q_NULLPTR;

}

void WebPage::handleUnknowProtocol(const QUrl& url)
{
	Settings settings{};

	const QString protocol = url.scheme();
	QStringList
		autoOpenProtocols{settings.value("Web-Settings/AutomaticallyOpenProtocols", QStringList()).toStringList()};
	QStringList blockedProtocols{settings.value("Web-Settings/BlockedProtocols", QStringList()).toStringList()};

	if (protocol == QLatin1String("mailto")) {
		desktopServiceOpen(url);
		return;
	}

	if (blockedProtocols.contains(protocol))
		return;

	if (autoOpenProtocols.contains(protocol)) {
		desktopServiceOpen(url);
		return;
	}

	CheckBoxDialog dialog(QMessageBox::Yes | QMessageBox::No, view());

	const QString text{tr("Sielo is unable to process the <b>%1</b> protocol. "
							  "Do you want Sielo to try to open the "
							  "llink b>%2%</b> with an external application?").arg(protocol, url.toString())};
	dialog.setText(text);
	dialog.setCheckBoxText(tr("Remember my action for this protocol"));
	dialog.setWindowTitle(tr("Request of an external protocol"));

	switch (dialog.exec()) {
	case QDialog::Accepted:
		if (dialog.isChecked()) {
			autoOpenProtocols.append(protocol);
			settings.setValue("Web-Settings/AutomaticallyOpenProtocols", autoOpenProtocols);
		}

		QDesktopServices::openUrl(url);
		break;
	case QDialog::Rejected:
		if (dialog.isChecked()) {
			blockedProtocols.append(protocol);
			settings.setValue("Web-Settings/BlockedProtocols", blockedProtocols);
		}
	default:
		break;
	}

}

void WebPage::desktopServiceOpen(const QUrl& url)
{
	//TODO: do some actions
	QDesktopServices::openUrl(url);
}

}
