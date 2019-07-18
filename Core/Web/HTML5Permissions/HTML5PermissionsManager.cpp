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

#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"

#include <QtDebug>

#include "Widgets/HTML5Permissions/HTML5PermissionsNotification.hpp"

#include "Utils/Settings.hpp"

#include "Web/WebPage.hpp"
#include "Web/WebView.hpp"


namespace Sn {

HTML5PermissionsManager::HTML5PermissionsManager(QObject* parent) :
	QObject(parent)
{
	loadSettings();
}

void HTML5PermissionsManager::requestPermissions(WebPage* page, const QUrl& origin,
												 const Engine::WebPage::Feature& feature)
{
	if (!page)
		return;

	if (!m_granted.contains(feature) || !m_denied.contains(feature)) {
		qWarning() << "HTML5PermissionsManager: Unknown feature " << feature;
		return;
	}

	if (m_granted[feature].contains(origin.toString())) {
		page->setFeaturePermission(origin, feature, Engine::WebPage::PermissionGrantedByUser);
		return;
	}

	if (m_denied[feature].contains(origin.toString())) {
		page->setFeaturePermission(origin, feature, Engine::WebPage::PermissionDeniedByUser);
		return;
	}

	HTML5PermissionsNotification* notification{new HTML5PermissionsNotification(origin, page, feature)};
	page->view()->addNotification(notification);

}

void HTML5PermissionsManager::rememberPermissions(const QUrl& origin, const Engine::WebPage::Feature& feature,
												  const Engine::WebPage::PermissionPolicy& policy)
{
	if (origin.isEmpty())
		return;

	if (policy == Engine::WebPage::PermissionGrantedByUser)
		m_granted[feature].append(origin.toString());
	else
		m_denied[feature].append(origin.toString());

	saveSettings();
}

void HTML5PermissionsManager::loadSettings()
{
	Settings settings{};

	settings.beginGroup("HTML5-Permissions");

	m_granted[Engine::WebPage::Notifications] = settings.value("NotificationsGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::Notifications] = settings.value("NotificationsDenied", QStringList()).toStringList();

	m_granted[Engine::WebPage::Geolocation] = settings.value("GeoLocationGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::Geolocation] = settings.value("GeoLocationDenied", QStringList()).toStringList();

	m_granted[Engine::WebPage::MediaAudioCapture] =
		settings.value("MediaAudioCaptureGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::MediaAudioCapture] =
		settings.value("MediaAudioCaptureDenied", QStringList()).toStringList();

	m_granted[Engine::WebPage::MediaVideoCapture] =
		settings.value("MediaVideoCaptureGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::MediaVideoCapture] =
		settings.value("MediaVideoCaptureDenied", QStringList()).toStringList();

	m_granted[Engine::WebPage::MediaAudioVideoCapture] =
		settings.value("MediaAudioVideoCaptureGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::MediaAudioVideoCapture] =
		settings.value("MediaAudioVideoCaptureDenied", QStringList()).toStringList();

	m_granted[Engine::WebPage::MouseLock] = settings.value("MouseLockGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::MouseLock] = settings.value("MouseLockDenied", QStringList()).toStringList();

	settings.endGroup();
}

void HTML5PermissionsManager::saveSettings()
{
	Settings settings{};

	settings.beginGroup("HTML5-Permissions");

	settings.setValue("NotificationsGranted", m_granted[Engine::WebPage::Notifications]);
	settings.setValue("NotificationsDenied", m_denied[Engine::WebPage::Notifications]);

	settings.setValue("GeoLocationGranted", m_granted[Engine::WebPage::Geolocation]);
	settings.setValue("GeoLocationDenied", m_denied[Engine::WebPage::Geolocation]);

	settings.setValue("MediaAudioCaptureGranted", m_granted[Engine::WebPage::MediaAudioCapture]);
	settings.setValue("MediaAudioCaptureDenied", m_denied[Engine::WebPage::MediaAudioCapture]);

	settings.setValue("MediaVideoCaptureGranted", m_granted[Engine::WebPage::MediaVideoCapture]);
	settings.setValue("MediaVideoCaptureDenied", m_denied[Engine::WebPage::MediaVideoCapture]);

	settings.setValue("MediaAudioVideoCaptureGranted", m_granted[Engine::WebPage::MediaAudioVideoCapture]);
	settings.setValue("MediaAudioVideoCaptureDenied", m_denied[Engine::WebPage::MediaAudioVideoCapture]);

	settings.setValue("MouseLockGranted", m_granted[Engine::WebPage::MouseLock]);
	settings.setValue("MouseLockDenied", m_denied[Engine::WebPage::MouseLock]);

	settings.endGroup();

}

}