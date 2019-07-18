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

#include "Widgets/HTML5Permissions/HTML5PermissionsNotification.hpp"

#include <QtDebug>

#include <QCursor>

#include <QTimer>

#include "Application.hpp"

#include "Web/WebPage.hpp"
#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"

namespace Sn {

HTML5PermissionsNotification::HTML5PermissionsNotification(const QUrl& origin, Engine::WebPage* page,
														   const Engine::WebPage::Feature& feature) :
	QWidget(),
	m_origin(origin),
	m_page(page),
	m_feature(feature)
{
	setAutoFillBackground(true);
	setupUi();

	const QString site{m_origin.host().isEmpty() ? tr("This site") : QString("<b>%1</b>").arg(m_origin.host())};

	switch (feature) {
	case Engine::WebPage::Notifications:
		m_iconLabel->setPixmap(QPixmap(":icons/other/notification.png"));
		m_textLabel->setText(tr("Allow %1 to show desktop notifications?").arg(site));
		break;
	case Engine::WebPage::Geolocation:
		m_iconLabel->setPixmap(QPixmap(":icons/other/geolocation.png"));
		m_textLabel->setText(tr("Allow %1 to locate your position?").arg(site));
		break;
	case Engine::WebPage::MediaAudioCapture:
		m_iconLabel->setPixmap(QPixmap(":icons/other/audiocapture.png"));
		m_textLabel->setText(tr("Allow %1 to use your microphone?").arg(site));
		break;
	case Engine::WebPage::MediaVideoCapture:
		m_iconLabel->setPixmap(QPixmap(":icons/other/webcam.png"));
		m_textLabel->setText(tr("Allow %1 to use your camera?").arg(site));
		break;
	case Engine::WebPage::MediaAudioVideoCapture:
//TODO: find the icon		m_iconLabel->setPixmap(QPixmap(":icons/other/microphone-webcam.png"));
		m_textLabel->setText(tr("Allow %1 to use your microphone and camera?").arg(site));
		break;
	case Engine::WebPage::MouseLock:
		m_iconLabel->setPixmap(QPixmap(":icons/other/mouselock.png"));
		m_textLabel->setText(tr("Allow %1 to hide your pointer?").arg(site));
		break;
	default:
		qWarning() << "Unknown feature " << feature;
		break;
	}

	connect(m_allowButton, &QPushButton::clicked, this, &HTML5PermissionsNotification::grantPermissions);
	connect(m_denyButton, &QPushButton::clicked, this, &HTML5PermissionsNotification::denyPermissions);
}

HTML5PermissionsNotification::~HTML5PermissionsNotification()
{
	// Empty
}

void HTML5PermissionsNotification::grantPermissions()
{
	if (!m_page)
		return;

	QTimer::singleShot(0, this, [this]()
	{
		if (m_feature == Engine::WebPage::MouseLock) {
			QWidget* view{m_page->view()};
			QCursor::setPos(view->mapToGlobal(view->rect().center()));
		}

		m_page->setFeaturePermission(m_origin, m_feature, Engine::WebPage::PermissionGrantedByUser);
	});

	if (m_remember->isChecked())
		Application::instance()->permissionsManager()
			->rememberPermissions(m_origin, m_feature, Engine::WebPage::PermissionGrantedByUser);

	hide();

}

void HTML5PermissionsNotification::denyPermissions()
{
	if (!m_page)
		return;

	m_page->setFeaturePermission(m_origin, m_feature, Engine::WebPage::PermissionDeniedByUser);

	if (m_remember->isChecked())
		Application::instance()->permissionsManager()
			->rememberPermissions(m_origin, m_feature, Engine::WebPage::PermissionDeniedByUser);

	hide();

}

void HTML5PermissionsNotification::setupUi()
{
	setObjectName("HTML5PermissionsNotification");
	resize(820, 34);
	setMaximumHeight(34);

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(-1, 0, -1, 0);

	m_iconLabel = new QLabel(this);
	m_iconLabel->setMinimumSize(QSize(24, 24));
	m_iconLabel->setMaximumSize(QSize(24, 24));
	m_iconLabel->setScaledContents(true);

	m_textLabel = new QLabel(this);

	m_spacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_remember = new QCheckBox(tr("Remember"), this);

	m_allowButton = new QPushButton(tr("Allow"), this);

	m_denyButton = new QPushButton(tr("Deny"), this);

	m_layout->addWidget(m_iconLabel);
	m_layout->addWidget(m_textLabel);
	m_layout->addItem(m_spacer);
	m_layout->addWidget(m_remember);
	m_layout->addWidget(m_allowButton);
	m_layout->addWidget(m_denyButton);
}

}