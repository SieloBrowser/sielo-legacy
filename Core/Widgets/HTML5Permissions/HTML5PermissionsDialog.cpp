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

#include "HTML5PermissionsDialog.hpp"

#include <QHeaderView>

#include "Application.hpp"

#include "Utils/Settings.hpp"

#include "Web/HTML5Permissions/HTML5PermissionsManager.hpp"

namespace Sn {

HTML5PermissionsDialog::HTML5PermissionsDialog(QWidget* parent) :
	QDialog(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();
	loadSettings();

	connect(m_remove, &QPushButton::clicked, this, &HTML5PermissionsDialog::removeEntry);
	connect(m_feature, SIGNAL(currentIndexChanged(int)), this, SLOT(featureIndexChanged()));

	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &HTML5PermissionsDialog::saveSettings);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &HTML5PermissionsDialog::reject);

	showFeaturePermissions(currentFeature());
}

HTML5PermissionsDialog::~HTML5PermissionsDialog()
{
	// Empty
}

void HTML5PermissionsDialog::showFeaturePermissions(Engine::WebPage::Feature feature)
{
	if (!m_granted.contains(feature) || !m_denied.contains(feature))
		return;

	m_permissionsTree->clear();

		foreach (const QString& site, m_granted.value(feature)) {
			QTreeWidgetItem* item{new QTreeWidgetItem(m_permissionsTree)};
			item->setText(0, site);
			item->setText(1, tr("Allow"));
			item->setData(0, Qt::UserRole + 10, Allow);

			m_permissionsTree->addTopLevelItem(item);
		}

		foreach (const QString& site, m_denied.value(feature)) {
			QTreeWidgetItem* item{new QTreeWidgetItem(m_permissionsTree)};
			item->setText(0, site);
			item->setText(1, tr("Deny"));
			item->setData(0, Qt::UserRole + 10, Deny);

			m_permissionsTree->addTopLevelItem(item);
		}
}

void HTML5PermissionsDialog::removeEntry()
{
	QTreeWidgetItem* item = m_permissionsTree->currentItem();

	if (!item)
		return;

	Role role = static_cast<Role>(item->data(0, Qt::UserRole + 10).toInt());
	const QString origin{item->text(0)};

	if (role == Allow)
		m_granted[currentFeature()].removeOne(origin);
	else
		m_denied[currentFeature()].removeOne(origin);

	delete item;
}

void HTML5PermissionsDialog::featureIndexChanged()
{
	showFeaturePermissions(currentFeature());
}

void HTML5PermissionsDialog::saveSettings()
{
	Settings settings{};

	settings.beginGroup("HTML5-Permissions");

	settings.setValue("NotificationsGranted", m_granted[Engine::WebPage::Notifications]);
	settings.setValue("NotificationDenied", m_denied[Engine::WebPage::Notifications]);

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

	Application::instance()->permissionsManager()->loadSettings();
}

void HTML5PermissionsDialog::setupUI()
{
	resize(483, 332);

	m_layout = new QGridLayout(this);
	m_featureLayout = new QHBoxLayout();
	m_removeLayout = new QVBoxLayout();

	m_descFeature = new QLabel(this);
	m_descFeature->setText(tr("Permission for:"));

	m_feature = new QComboBox(this);
	m_feature->insertItems(0, QStringList() << tr("Notifications")
											<< tr("Geolocation")
											<< tr("Microphone")
											<< tr("Camera")
											<< tr("Microphone and Camera")
											<< tr("Hide Pointer"));

	horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

	m_permissionsTree = new QTreeWidget(this);
	m_permissionsTree->setIndentation(0);
	m_permissionsTree->setSortingEnabled(true);
	m_permissionsTree->setAllColumnsShowFocus(true);
	m_permissionsTree->header()->resizeSection(0, 220);

	QTreeWidgetItem* treeHeader = m_permissionsTree->headerItem();
	treeHeader->setText(0, tr("Site"));
	treeHeader->setText(1, tr("Behaviour"));

	m_removeSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_remove = new QPushButton(this);
	m_remove->setText(tr("Remove"));

	m_buttonBox = new QDialogButtonBox(this);
	m_buttonBox->setOrientation(Qt::Horizontal);
	m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

	m_featureLayout->addWidget(m_descFeature);
	m_featureLayout->addWidget(m_feature);
	m_featureLayout->addItem(horizontalSpacer);

	m_removeLayout->addItem(m_removeSpacer);
	m_removeLayout->addWidget(m_remove);

	m_layout->addLayout(m_featureLayout, 0, 0, 1, 2);
	m_layout->addWidget(m_permissionsTree, 1, 0, 1, 1);
	m_layout->addLayout(m_removeLayout, 1, 1, 1, 1);
	m_layout->addWidget(m_buttonBox, 2, 0, 1, 2);

}

void HTML5PermissionsDialog::loadSettings()
{
	Settings settings{};

	settings.beginGroup("HTML5-Permissions");

	m_granted[Engine::WebPage::Notifications] = settings.value("NotificationsGranted", QStringList()).toStringList();
	m_denied[Engine::WebPage::Notifications] = settings.value("NotificationDenied", QStringList()).toStringList();

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

Engine::WebPage::Feature HTML5PermissionsDialog::currentFeature() const
{
	switch (m_feature->currentIndex()) {
	case 0:
		return Engine::WebPage::Notifications;
	case 1:
		return Engine::WebPage::Geolocation;
	case 2:
		return Engine::WebPage::MediaVideoCapture;
	case 3:
		return Engine::WebPage::MediaVideoCapture;
	case 4:
		return Engine::WebPage::MediaAudioVideoCapture;
	case 5:
		return Engine::WebPage::MouseLock;
	default:
		Q_UNREACHABLE();
		return Engine::WebPage::Notifications;
	}
}

}