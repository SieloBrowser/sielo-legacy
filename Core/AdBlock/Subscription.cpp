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

#include "AdBlock/Subscription.hpp"

#include <QFile>

#include <QTimer>

#include "Application.hpp"

#include "Network/NetworkManager.hpp"

#include "AdBlock/Manager.hpp"
#include "AdBlock/Rule.hpp"

namespace Sn {
namespace ADB {

static const QString ADBLOCK_EASYLIST_URL = "https://easylist-downloads.adblockplus.org/easylist.txt";

Subscription::Subscription(const QString& title, QObject* parent) :
		QObject(parent),
		m_title(title),
		m_updated(false)
{
	// Empty
}

Subscription::~Subscription()
{
	qDeleteAll(m_rules);
}

void Subscription::setFilePath(const QString& path)
{
	m_filePath = path;
}

void Subscription::setUrl(const QUrl& url)
{
	m_url = url;
}

void Subscription::loadSubscription(const QStringList& disabledRules)
{
	QFile file{m_filePath};

	if (!file.exists()) {
		QTimer::singleShot(0, this, &Subscription::updateSubscription);
		return;
	}

	if (!file.open(QFile::ReadOnly)) {
		qWarning() << "ADB::Subscription: " << __FUNCTION__ << "Unable to open adblock file for reading " << m_filePath;
		QTimer::singleShot(0, this, &Subscription::updateSubscription);
	}

	QTextStream textStream{&file};

	textStream.setCodec("UTF-8");
	textStream.readLine(1024);
	textStream.readLine(1024);

	QString header{textStream.readLine(1024)};

	if (!header.startsWith(QLatin1String("[Adblock")) || m_title.isEmpty()) {
		qWarning() << "ADB::Subscription: " << __FUNCTION__ << " invalid format of adblock file! " << m_filePath;
		QTimer::singleShot(0, this, &Subscription::updateSubscription);
		return;
	}

	m_rules.clear();

	while (!textStream.atEnd()) {
		Rule* rule{new Rule(textStream.readLine(), this)};

		if (disabledRules.contains(rule->filter()))
			rule->setEnabled(false);

		m_rules.append(rule);
	}

	if (m_rules.isEmpty() && !m_updated)
		QTimer::singleShot(0, this, &Subscription::updateSubscription);
}

void Subscription::saveSubscription()
{
	// Empty
}

const Rule* Subscription::rule(int offset) const
{
	if (!(offset >= 0 && m_rules.count() > offset))
		return nullptr;

	return m_rules[offset];
}

QVector<Rule*> Subscription::allRulles() const
{
	return m_rules;
}

const Rule* Subscription::enableRule(int offset)
{
	if (!(offset >= 0 && m_rules.count() > offset))
		return nullptr;

	Rule* rule{m_rules[offset]};
	rule->setEnabled(true);

	Manager::instance()->removeDisabledRule(rule->filter());

	emit subscriptionChanged();

	if (rule->isCSSRule())
		Application::instance()->reloadUserStyleSheet();

	return rule;
};

const Rule* Subscription::disableRule(int offset)
{
	if (!(offset >= 0 && m_rules.count() > offset))
		return nullptr;

	Rule* rule{m_rules[offset]};
	rule->setEnabled(false);

	Manager::instance()->addDisabledRule(rule->filter());

	emit subscriptionChanged();

	if (rule->isCSSRule())
		Application::instance()->reloadUserStyleSheet();

	return rule;
}

int Subscription::addRule(Rule* rule)
{
	Q_UNUSED(rule)
	return -1;
}

bool Subscription::removeRule(int offset)
{
	Q_UNUSED(offset)
	return false;
}

const Rule* Subscription::replaceRule(Rule* rule, int offset)
{
	Q_UNUSED(rule)
	Q_UNUSED(offset)
	return nullptr;
}

void Subscription::updateSubscription()
{
	if (m_reply || !m_url.isValid())
		return;

	m_reply = Application::instance()->networkManager()->get(QNetworkRequest(m_url));
	connect(m_reply, &QNetworkReply::finished, this, &Subscription::subscriptionDownloaded);
}

bool Subscription::saveDownloadedData(const QByteArray& data)
{
	QFile file{m_filePath};

	if (!file.open(QFile::ReadWrite | QFile::Truncate)) {
		qWarning() << "ADB::Subscription: " << __FUNCTION__ << " Unable to open adblock file for writing: " <<
				   m_filePath;
		return false;
	}

	file.write(QString("Title: %1\nUrl: %2\n").arg(title(), url().toString()).toUtf8());

	if (Manager::instance()->useLimitedEasyList() && m_url == QUrl(ADBLOCK_EASYLIST_URL)) {
		QByteArray part1 = data.left(data.indexOf(QLatin1String(
				"!-----------------------------Third-party adverts-----------------------------!")));
		QByteArray part2 = data.mid(data.indexOf(QLatin1String(
				"!---------------------------------Whitelists----------------------------------!")));

		file.write(part1);
		file.write(part2);
		file.close();

		return true;
	}

	file.write(data);
	file.close();

	return true;
};

void Subscription::subscriptionDownloaded()
{
	if (m_reply != qobject_cast<QNetworkReply*>(sender()))
		return;

	bool error{false};
	const QByteArray response{QString::fromUtf8(m_reply->readAll()).toUtf8()};

	if (m_reply->error() != QNetworkReply::NoError || !response.startsWith(QByteArray("[Adblock"))
		|| !saveDownloadedData(response)) {
		error = true;
	}

	m_reply->deleteLater();
	m_reply = nullptr;

	if (error) {
		emit subscriptionError(tr("Cannot load subscription!"));
		return;
	}

	loadSubscription(Manager::instance()->disabledRules());

	emit subscriptionUpdated();
	emit subscriptionChanged();
};

}
}