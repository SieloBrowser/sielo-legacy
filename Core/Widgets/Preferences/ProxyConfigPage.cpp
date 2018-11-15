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

#include "ProxyConfigPage.hpp"

#include <QNetworkProxy>

#include "Utils/Settings.hpp"

#include "Application.hpp"

namespace Sn {

ProxyConfigPage::ProxyConfigPage(QWidget* parent) :
	QWidget(parent)
{
	setAttribute(Qt::WA_DeleteOnClose);

	setupUI();
	loadSettings();

	connect(m_manualProxyRadio, &QRadioButton::toggled, this, &ProxyConfigPage::useManualProxyChanged);
}

ProxyConfigPage::~ProxyConfigPage()
{
	// Empty
}

void ProxyConfigPage::loadSettings()
{
	Settings settings{};

	settings.beginGroup("Proxy-Settings");

	QNetworkProxy::ProxyType
		proxyType = QNetworkProxy::ProxyType(settings.value("proxyType", QNetworkProxy::NoProxy).toInt());

	m_systemProxyRadio->setChecked(proxyType == QNetworkProxy::NoProxy);
	m_manualProxyRadio->setChecked(proxyType != QNetworkProxy::NoProxy);

	if (proxyType == QNetworkProxy::Socks5Proxy)
		m_proxyType->setCurrentIndex(1);
	else
		m_proxyType->setCurrentIndex(0);

	m_proxyServer->setText(settings.value("hostName", "").toString());
	m_proxyPort->setText(settings.value("port", 8080).toString());
	m_proxyUsername->setText(settings.value("username", "").toString());
	m_proxyPassword->setText(settings.value("password", "").toString());

	settings.endGroup();

	useManualProxyChanged(m_manualProxyRadio->isChecked());
}

void ProxyConfigPage::save()
{
	QNetworkProxy::ProxyType proxyType{};

	if (m_systemProxyRadio->isChecked())
		proxyType = QNetworkProxy::NoProxy;
	else if (m_proxyType->currentIndex() == 0)
		proxyType = QNetworkProxy::HttpProxy;
	else
		proxyType = QNetworkProxy::Socks5Proxy;

	Settings settings{};

	settings.beginGroup("Proxy-Settings");

	settings.setValue("proxyType", proxyType);
	settings.setValue("hostName", m_proxyServer->text());
	settings.setValue("port", m_proxyPort->text().toInt());
	settings.setValue("username", m_proxyUsername->text());
	settings.setValue("password", m_proxyPassword->text());

	settings.endGroup();
}

void ProxyConfigPage::useManualProxyChanged(bool enabled)
{
	m_proxyType->setEnabled(enabled);
	m_proxyServer->setEnabled(enabled);
	m_descProxyPort->setEnabled(enabled);
	m_proxyPort->setEnabled(enabled);

	m_descProxyUsername->setEnabled(enabled);
	m_proxyUsername->setEnabled(enabled);
	m_descProxyPassword->setEnabled(enabled);
	m_proxyPassword->setEnabled(enabled);
}

void ProxyConfigPage::setupUI()
{
	m_layout = new QGridLayout(this);
	m_serverLayout = new QHBoxLayout();
	m_userLayout = new QHBoxLayout();

	m_systemProxyRadio = new QRadioButton(tr("System proxy configuration"), this);
	m_manualProxyRadio = new QRadioButton(tr("Manual configuration"), this);

	m_proxyType = new QComboBox(this);
	m_proxyType->addItems(QStringList() << "HTTP" << "SOCKS5");
	m_proxyServer = new QLineEdit(this);
	m_descProxyPort = new QLabel(tr("Port:"), this);
	m_proxyPort = new QLineEdit(this);

	m_descProxyUsername = new QLabel(tr("Username:"), this);
	m_proxyUsername = new QLineEdit(this);
	m_descProxyPassword = new QLabel(tr("Password:"), this);
	m_proxyPassword = new QLineEdit(this);
	m_proxyPassword->setEchoMode(QLineEdit::Password);

	m_leftSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_rightSpacer = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Preferred);
	m_spacer = new QSpacerItem(20, 20, QSizePolicy::Preferred, QSizePolicy::Expanding);

	m_serverLayout->addWidget(m_proxyType);
	m_serverLayout->addWidget(m_proxyServer);
	m_serverLayout->addWidget(m_descProxyPort);
	m_serverLayout->addWidget(m_proxyPort);

	m_userLayout->addWidget(m_descProxyUsername);
	m_userLayout->addWidget(m_proxyUsername);
	m_userLayout->addWidget(m_descProxyPassword);
	m_userLayout->addWidget(m_proxyPassword);
	m_userLayout->addItem(m_rightSpacer);

	m_layout->addWidget(m_systemProxyRadio, 0, 0, 1, 2);
	m_layout->addWidget(m_manualProxyRadio, 1, 0, 1, 2);
	m_layout->addItem(m_leftSpacer, 2, 0, 1, 1);
	m_layout->addLayout(m_serverLayout, 2, 1, 1, 1);
	m_layout->addLayout(m_userLayout, 3, 1, 1, 1);
	m_layout->addItem(m_spacer, 4, 0, 1, 1);
}

}