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

#include "Network/NetworkManager.hpp"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkProxy>

#include <QDialog>

#include <QFormLayout>

#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QCheckBox>

#include "Application.hpp"

#include "Utils/Settings.hpp"

#include "Password/PasswordManager.hpp"
#include "Password/AutoFill/AutoFill.hpp"

#include "Network/BaseUrlInterceptor.hpp"
#include "Network/NetworkUrlInterceptor.hpp"

namespace Sn {

NetworkManager::NetworkManager(QObject* parent) :
	QNetworkAccessManager(parent)
{
	m_urlInterceptor = new NetworkUrlInterceptor(this);
	Application::instance()->webProfile()->setRequestInterceptor(m_urlInterceptor);

	Application::instance()->cookieJar();

	connect(this,
			&QNetworkAccessManager::authenticationRequired,
			this,
			[this](QNetworkReply* reply, QAuthenticator* authenticator)
			{
				authentication(reply->url(), authenticator);
			});

	connect(this,
			&QNetworkAccessManager::proxyAuthenticationRequired,
			this,
			[this](const QNetworkProxy& proxy, QAuthenticator* authenticator)
			{
				proxyAuthentication(proxy.hostName(), authenticator);
			});
}

void NetworkManager::authentication(const QUrl& url, QAuthenticator* auth, QWidget* parent)
{
	QDialog* dialog{new QDialog(parent)};

	QIcon icon = dialog->windowIcon();
	Qt::WindowFlags flags = dialog->windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	dialog->setWindowFlags(flags);
	dialog->setWindowIcon(icon);

	dialog->setWindowTitle(tr("Authorisation required"));

	QFormLayout* formLayout{new QFormLayout(dialog)};
	QLabel* descLabel{new QLabel(tr("A username and password are being requested by %1. The site says: \"%2\"")
									 .arg(url.host(), auth->realm().toHtmlEscaped()), dialog)};
	QLabel* userLabel{new QLabel(tr("Username: "), dialog)};
	QLabel* passwordLabel{new QLabel(tr("Password: "), dialog)};
	QLineEdit* userLineEdit{new QLineEdit(dialog)};
	QLineEdit* passwordLineEdit{new QLineEdit(dialog)};
	passwordLineEdit->setEchoMode(QLineEdit::Password);
	QCheckBox* save{new QCheckBox(tr("Save username and password for this site"))};
	QDialogButtonBox* buttonBox{new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog)};

	connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);
	connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);

	formLayout->addRow(descLabel);
	formLayout->addRow(userLabel, userLineEdit);
	formLayout->addRow(passwordLabel, passwordLineEdit);
	formLayout->addRow(save);
	formLayout->addWidget(buttonBox);

	AutoFill* fill{Application::instance()->autoFill()};
	QString storedUser{};
	QString storedPassword{};
	bool shouldUpdateEntry{false};

	if (fill->isStored(url)) {
		const QVector<PasswordEntry>& data = fill->getFormData(url);

		if (!data.isEmpty()) {
			save->setChecked(true);
			shouldUpdateEntry = true;
			storedUser = data[0].username;
			storedPassword = data[0].password;

			userLineEdit->setText(storedUser);
			passwordLineEdit->setText(storedPassword);
		}
	}

	if (Application::instance()->privateBrowsing())
		save->setVisible(false);

	if (dialog->exec() != QDialog::Accepted) {
		*auth = QAuthenticator();
		delete dialog;
		return;
	}

	auth->setUser(userLineEdit->text());
	auth->setPassword(passwordLineEdit->text());

	if (save->isChecked()) {
		if (shouldUpdateEntry) {
			if (storedUser != userLineEdit->text() || storedPassword != passwordLineEdit->text())
				fill->updateEntry(url, userLineEdit->text(), passwordLineEdit->text());
		}
		else
			fill->addEntry(url, userLineEdit->text(), passwordLineEdit->text());
	}

	delete dialog;
}

void NetworkManager::proxyAuthentication(const QString& proxyHost, QAuthenticator* auth, QWidget* parent)
{
	const QNetworkProxy proxy = QNetworkProxy::applicationProxy();

	if (!proxy.user().isEmpty() && !proxy.password().isEmpty()) {
		auth->setUser(proxy.user());
		auth->setPassword(proxy.password());
		return;
	}

	QDialog* dialog{new QDialog(parent)};

	QIcon icon = dialog->windowIcon();
	Qt::WindowFlags flags = dialog->windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	dialog->setWindowFlags(flags);
	dialog->setWindowIcon(icon);
	dialog->setWindowTitle(tr("Proxy authorisation required"));

	QFormLayout* formLayout{new QFormLayout(dialog)};

	QLabel
		* descLabel{new QLabel(tr("A username and password are being requested by proxy %1. ").arg(proxyHost), dialog)};
	QLabel* userLabel{new QLabel(tr("Username: "), dialog)};
	QLabel* passwordLabel{new QLabel(tr("Password: "), dialog)};

	QLineEdit* user{new QLineEdit(dialog)};
	QLineEdit* password{new QLineEdit(dialog)};

	password->setEchoMode(QLineEdit::Password);

	QDialogButtonBox* buttonBox{new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialog)};

	connect(buttonBox, &QDialogButtonBox::accepted, dialog, &QDialog::accept);
	connect(buttonBox, &QDialogButtonBox::rejected, dialog, &QDialog::reject);

	formLayout->addRow(descLabel);
	formLayout->addRow(userLabel, user);
	formLayout->addRow(passwordLabel, password);
	formLayout->addWidget(buttonBox);

	if (dialog->exec() != QDialog::Accepted) {
		*auth = QAuthenticator();
		delete dialog;
		return;
	}

	auth->setUser(user->text());
	auth->setPassword(password->text());

	delete dialog;

}

void NetworkManager::installUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	m_urlInterceptor->installUrlInterceptor(interceptor);
}

void NetworkManager::removeUrlInterceptor(BaseUrlInterceptor* interceptor)
{
	m_urlInterceptor->removeUrlInterceptor(interceptor);
}

void NetworkManager::loadSettings()
{
	m_urlInterceptor->loadSettings();

	Settings settings{};

	settings.beginGroup("Proxy-Settings");

	QNetworkProxy proxy{};

	proxy.setType(QNetworkProxy::ProxyType(settings.value("proxyType", QNetworkProxy::NoProxy).toInt()));
	proxy.setHostName(settings.value("hostName", QString()).toString());
	proxy.setPort(settings.value("port", 8080).toInt());
	proxy.setUser(settings.value("username", QString()).toString());
	proxy.setPassword(settings.value("password", QString()).toString());

	settings.endGroup();

	QNetworkProxy::setApplicationProxy(proxy);
}

QNetworkReply* NetworkManager::createRequest(Operation operation, const QNetworkRequest& request,
											 QIODevice* outgoingData)
{
	QNetworkRequest req = request;
	req.setAttribute(QNetworkRequest::SpdyAllowedAttribute, true);
	req.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);

	return QNetworkAccessManager::createRequest(operation, req, outgoingData);
}

}