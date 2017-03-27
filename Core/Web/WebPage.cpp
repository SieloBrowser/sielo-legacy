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

#include "Web/WebPage.hpp"

#include <QPointer>
#include <QTimer>
#include <QDesktopServices>
#include <QFileInfo>

#include <QWebEngineSettings>

#include <QSettings>

#include "Web/WebHitTestResult.hpp"
#include "Web/WebView.hpp"

#include "Widgets/CheckBoxDialog.hpp"
#include "Utils/DelayedFileWatcher.hpp"

#include "Plugins/PluginProxy.hpp"

#include "Application.hpp"

namespace Sn {
WebPage::WebPage(QObject* parent, DelayedFileWatcher* m_fileWatcher) :
	QWebEnginePage(Application::instance()->webProfile(), parent),
	m_fileWatcher(m_fileWatcher),
	m_runningLoop(nullptr),
	m_loadProgress(-1),
	m_blockAlerts(false),
	m_secureStatus(false),
	m_adjustingSheduled(false)
{
	connect(this, &QWebEnginePage::loadProgress, this, &WebPage::progress);
	connect(this, &QWebEnginePage::loadFinished, this, &WebPage::finished);
	connect(this, &QWebEnginePage::urlChanged, this, &WebPage::urlChanged);
	connect(this, &QWebEnginePage::featurePermissionRequested, this, &WebPage::featurePermissionRequested);
	connect(this, &QWebEnginePage::windowCloseRequested, this, &WebPage::windowCloseRequested);
	connect(this, &QWebEnginePage::fullScreenRequested, this, &WebPage::fullScreenRequested);

	//TODO: Connect with network manager
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
	return static_cast<WebView*>(QWebEnginePage::view());
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
				  QWebEngineScript::ApplicationWorld);
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

	QString title{tr("Alerte JavaScripte")};

	if (!url().host().isEmpty())
		title.append(QString(" - %1").arg(url().host()));

	CheckBoxDialog dialog(QDialogButtonBox::Ok, view());
	dialog.setWindowTitle(title);
	dialog.setText(msg);
	dialog.setCheckBoxText(tr("Empêcher la page de créer des boites de dialog supplémentaires."));
	dialog.exec();

	m_blockAlerts = dialog.isChecked();
}

void WebPage::setJavaScriptEnable(bool enabled)
{
	settings()->setAttribute(QWebEngineSettings::JavascriptEnabled, enabled);
}

bool WebPage::isRunningLoop()
{
	return m_runningLoop;
}

bool WebPage::isLoading() const
{
	return m_loadProgress < 100;
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
		triggerAction(QWebEnginePage::Reload);
}

void WebPage::windowCloseRequested()
{
	if (!view())
		return;

	//TODO: view()->closeView();
}

void WebPage::fullScreenRequested(QWebEngineFullScreenRequest fullScreenRequest)
{
	//TODO: view()->requestFullScreen(fullScreenRequest.toggleOn());

	const bool accepted = fullScreenRequest.toggleOn() == view()->isFullScreen();

	if (accepted)
		fullScreenRequest.accept();
	else
		fullScreenRequest.reject();
}

void WebPage::featurePermissionRequested(const QUrl& origin, const QWebEnginePage::Feature& feature)
{
	if (feature == MouseLock && view()->isFullScreen())
		setFeaturePermission(origin, feature, PermissionGrantedByUser);
//    else
//      //TODO: Manage permission by app
}

bool WebPage::acceptNavigationRequest(const QUrl& url, NavigationType type, bool isMainFrame)
{
	//TODO: Plugins implementation

	return QWebEnginePage::acceptNavigationRequest(url, type, isMainFrame);
}

QWebEnginePage* WebPage::createWindow(QWebEnginePage::WebWindowType type)
{
	return Q_NULLPTR;

	//TODO: Implement this methode

}

void WebPage::handleUnknowProtocol(const QUrl& url)
{
	QSettings settings{};

	const QString protocol = url.scheme();
	QStringList autoOpenProtocols{settings.value("AutomaticalyOpenProtocols", QStringList()).toStringList()};
	QStringList blockedProtocols{settings.value("BlockedProtocols", QStringList()).toStringList()};

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

	CheckBoxDialog dialog(QDialogButtonBox::Yes | QDialogButtonBox::No, view());

	const QString text{tr("Sielo n'arrive pas à traiter les protocoles <b>%1</b>. "
							  "Voulez vous que Sielo tente malgré tout d'ouvrir le "
							  "lien <b>%2%</b> avec un application système ?").arg(protocol, url.toString())};
	dialog.setText(text);
	dialog.setCheckBoxText(tr("Se rappler de mon choix pour ce protocole"));
	dialog.setWindowTitle(tr("Requête d'un protocole externe"));

	switch (dialog.exec()) {
	case QDialog::Accepted:
		if (dialog.isChecked()) {
			autoOpenProtocols.append(protocol);
			settings.setValue("AutomaticalyOpenProtocols", autoOpenProtocols);
		}

		QDesktopServices::openUrl(url);
		break;
	case QDialog::Rejected:
		if (dialog.isChecked()) {
			blockedProtocols.append(protocol);
			settings.setValue("BlockedProtocols", blockedProtocols);
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