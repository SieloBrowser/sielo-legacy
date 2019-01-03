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

#include "AdBlock/Manager.hpp"

#include <QTimer>

#include <QMessageBox>

#include <QTextStream>
#include <QUrlQuery>

#include <QDir>

#include "Application.hpp"

#include "Network/NetworkManager.hpp"

#include "Utils/DataPaths.hpp"
#include "Utils/Settings.hpp"

#include "AdBlock/Rule.hpp"
#include "AdBlock/Matcher.hpp"
#include "AdBlock/CustomList.hpp"
#include "AdBlock/Subscription.hpp"
#include "AdBlock/UrlInterceptor.hpp"

namespace Sn {
namespace ADB {

Q_GLOBAL_STATIC(Manager, sn_adblock_manager)

Manager::Manager(QObject* parent) :
		QObject(parent),
		m_loaded(false),
		m_enabled(false),
		m_useLimitedEasyList(true),
		m_matcher(new Matcher(this)),
		m_interceptor(new UrlInterceptor(this))
{
	load();
}

Manager::~Manager()
{
	qDeleteAll(m_subscriptions);
}

Manager* Manager::instance()
{
	return sn_adblock_manager();
}

void Manager::load()
{
	if (m_loaded)
		return;

	Settings settings{};

	settings.beginGroup("AdBlock-Settings");

	QDateTime lastUpdate{settings.value("lastUpdate", QDateTime()).toDateTime()};

	m_enabled = settings.value("enabled", m_enabled).toBool();
	m_useLimitedEasyList = settings.value("useLimitedEasyList", m_useLimitedEasyList).toBool();
	m_disabledRules = settings.value("disabledRules", QStringList()).toStringList();

	settings.endGroup();

	if (!m_enabled) {
		Application::instance()->networkManager()->removeUrlInterceptor(m_interceptor);
		return;
	}

	QDir adblockDir{DataPaths::currentProfilePath() + QLatin1String("/adblock")};

	if (!adblockDir.exists())
		QDir(DataPaths::currentProfilePath()).mkdir("adblock");

			foreach (const QString& fileName, adblockDir.entryList(QStringList("*.txt"), QDir::Files)) {
			if (fileName == QLatin1String("customlist.txt"))
				continue;

			const QString absolutePath{adblockDir.absoluteFilePath(fileName)};
			QFile file{absolutePath};

			if (!file.open(QFile::ReadOnly))
				continue;

			QTextStream textStream{&file};
			textStream.setCodec("UTF-8");
			QString title{textStream.readLine(1024).remove(QLatin1String("Title: "))};
			QUrl url{QUrl(textStream.readLine(1024).remove(QLatin1String("Url: ")))};

			if (title.isEmpty() || !url.isValid()) {
				qWarning() << "ADB::Manager: Invalid subscription file " << absolutePath;
				continue;
			}

			Subscription* subscription{new Subscription(title, this)};

			subscription->setUrl(url);
			subscription->setFilePath(absolutePath);

			m_subscriptions.append(subscription);
		}

	if (m_subscriptions.isEmpty()) {
		Subscription* easyList{new Subscription(tr("EasyList"), this)};

		easyList->setUrl(QUrl("https://easylist-downloads.adblockplus.org/easylist.txt"));
		easyList->setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/easylist.txt"));

		m_subscriptions.prepend(easyList);
	}

	CustomList* customList{new CustomList(this)};
	m_subscriptions.append(customList);

			foreach (Subscription* subscription, m_subscriptions) {
			subscription->loadSubscription(m_disabledRules);

			connect(subscription,
					&Subscription::subscriptionUpdated,
					Application::instance(),
					&Application::reloadUserStyleSheet);
			connect(subscription, &Subscription::subscriptionChanged, m_matcher, &Matcher::update);
		}

	if (lastUpdate.addDays(5) < QDateTime::currentDateTime())
		QTimer::singleShot(1000 * 60, this, &Manager::updateAllSubscriptions);

	m_matcher->update();
	m_loaded = true;

	Application::instance()->networkManager()->installUrlInterceptor(m_interceptor);
}

void Manager::save()
{
	if (!m_loaded)
		return;

			foreach (Subscription* subscription, m_subscriptions) subscription->saveSubscription();

	Settings settings{};

	settings.beginGroup("AdBlock-Settings");

	settings.setValue("enabled", m_enabled);
	settings.setValue("useLimitedEasyList", m_useLimitedEasyList);
	settings.setValue("disabledRules", m_disabledRules);

	settings.endGroup();
}

bool Manager::isEnabled() const
{
	return m_enabled;
}

bool Manager::canRunOnScheme(const QString& scheme) const
{
	return !(scheme == QLatin1String("file") || scheme == QLatin1String("qrc") || scheme == QLatin1String("sielo")
			 || scheme == QLatin1String("data") || scheme == QLatin1String("adb"));
}

bool Manager::useLimitedEasyList() const
{
	return m_useLimitedEasyList;
}

void Manager::setUseLimitedEasyList(bool useLimited)
{
	m_useLimitedEasyList = useLimited;

			foreach (Subscription* subscription, m_subscriptions) {
			if (subscription->url() == QUrl("https://easylist-downloads.adblockplus.org/easylist.txt"))
				subscription->updateSubscription();
		}
}

QString Manager::elementHidingRules(const QUrl& url) const
{
	if (!isEnabled() || !canRunOnScheme(url.scheme()) || !canBeBlocked(url))
		return QString();

	return m_matcher->elementHidingRules();
}

QString Manager::elementHidingRulesForDomain(const QUrl& url) const
{
	if (!isEnabled() || !canRunOnScheme(url.scheme()) || !canBeBlocked(url))
		return QString();

	return m_matcher->elementHidingRulesForDomain(url.host());
}

Subscription* Manager::subscriptionByName(const QString& name) const
{
			foreach (Subscription* subscription, m_subscriptions) {
			if (subscription->title() == name)
				return subscription;
		}

	return nullptr;
}

QList<Subscription*> Manager::subscriptions() const
{
	return m_subscriptions;
}

bool Manager::addSubscriptionFromUrl(const QUrl& url)
{
	const QList<QPair<QString, QString>> queryItems = QUrlQuery(url).queryItems(QUrl::FullyDecoded);
	QString subscriptionTitle{};
	QString subscriptionUrl{};

	for (int i{0}; i < queryItems.count(); ++i) {
		QPair<QString, QString> pair = queryItems[i];

		if (pair.first == QLatin1String("location"))
			subscriptionUrl = pair.second;
		else if (pair.first == QLatin1String("title"))
			subscriptionTitle = pair.second;
	}

	if (subscriptionTitle.isEmpty() || subscriptionUrl.isEmpty())
		return false;

	const QString& message{tr("Do you want to add <b>%1</b> subscription?").arg(subscriptionTitle)};

	QMessageBox::StandardButtons
			result{
			QMessageBox::question(nullptr, tr("AdBlock Subscription"), message, QMessageBox::Yes | QMessageBox::No)};

	if (result == QMessageBox::Yes) {
		Manager::instance()->addSubscription(subscriptionTitle, subscriptionUrl);
		Manager::instance()->showDialog();
	}

	return true;
}

Subscription* Manager::addSubscription(const QString& title, const QString& url)
{
	if (title.isEmpty() || url.isEmpty())
		return nullptr;

	QString fileName{title.toLower()};

	fileName.replace(QLatin1Char('/'), QLatin1Char('-'));
	fileName.remove(QLatin1Char('\\'));
	fileName.remove(QLatin1Char(':'));
	fileName.remove(QLatin1Char('*'));
	fileName.remove(QLatin1Char('?'));
	fileName.remove(QLatin1Char('"'));
	fileName.remove(QLatin1Char('<'));
	fileName.remove(QLatin1Char('>'));
	fileName.remove(QLatin1Char('|'));

	QString filePath{Application::ensureUniqueFilename(DataPaths::currentProfilePath() + QLatin1String("/adblock/") + fileName)};
	QByteArray data{QString("Title: %1\nUrl: %2\n[Adblock Plus 2.0]").arg(title, url).toLatin1()};
	QFile file{filePath};

	if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
		qWarning() << "ADB::Manager: Cannot write to file " << filePath;
		return nullptr;
	}

	file.write(data);
	file.close();

	Subscription* subscription{new Subscription(title, this)};

	subscription->setUrl(QUrl(url));
	subscription->setFilePath(filePath);
	subscription->loadSubscription(m_disabledRules);

	m_subscriptions.insert(m_subscriptions.count() - 1, subscription);

	connect(subscription,
			&Subscription::subscriptionUpdated,
			Application::instance(),
			&Application::reloadUserStyleSheet);
	connect(subscription, &Subscription::subscriptionChanged, m_matcher, &Matcher::update);

	return subscription;
}

bool Manager::removeSubscription(Subscription* subscription)
{
	if (!m_subscriptions.contains(subscription) || !subscription->canBeRemoved())
		return false;

	QFile(subscription->filePath()).remove();
	m_subscriptions.removeOne(subscription);

	m_matcher->update();
	delete subscription;

	return true;
}

bool Manager::block(Engine::UrlRequestInfo& request)
{
	const QString urlString{request.requestUrl().toEncoded().toLower()};
	const QString urlDomain{request.requestUrl().host().toLower()};
	const QString urlScheme{request.requestUrl().scheme().toLower()};

	if (!isEnabled() || !canRunOnScheme(urlScheme))
		return false;

	bool res{false};
	const Rule* blockedRule{m_matcher->match(request, urlDomain, urlString)};

	if (blockedRule) {
		res = true;

		if (request.resourceType() == Engine::UrlRequestInfo::ResourceTypeMainFrame) {
			QUrl url{QStringLiteral("sielo:adblock")};
			QUrlQuery query{};

			query.addQueryItem(QStringLiteral("rule"), blockedRule->filter());
			query.addQueryItem(QStringLiteral("subscription"), blockedRule->subscriptions()->title());
			url.setQuery(query);
			request.redirect(url);
		}
		else
			request.block(true);
	}

	return res;
}

void Manager::addDisabledRule(const QString& filter)
{
	m_disabledRules.append(filter);
}

void Manager::removeDisabledRule(const QString& filter)
{
	m_disabledRules.removeOne(filter);
}

CustomList* Manager::customList() const
{
			foreach (Subscription* subscription, m_subscriptions) {
			CustomList* list = qobject_cast<CustomList*>(subscription);

			if (list)
				return list;
		}

	return nullptr;
}

void Manager::setEnabled(bool enabled)
{
	if (m_enabled == enabled)
		return;

	m_enabled = enabled;
	emit enabledChanged(enabled);

	Settings settings{};

	settings.beginGroup("AdBlock-Settings");

	settings.setValue("enabled", m_enabled);

	settings.endGroup();

	if (enabled == false)
		m_loaded = false;
	else
		load();

	Application::instance()->reloadUserStyleSheet();
}

void Manager::showRule()
{
	//TODO: do
}

void Manager::updateAllSubscriptions()
{
			foreach (Subscription* subscription, m_subscriptions) subscription->updateSubscription();

	Settings settings{};

	settings.beginGroup("AdBlock-Settings");

	settings.setValue("lastUpdate", QDateTime::currentDateTime());

	settings.endGroup();
}

Dialog* Manager::showDialog()
{
	//TODO: do
	return nullptr;
}

bool Manager::canBeBlocked(const QUrl& url) const
{
	return !m_matcher->adBlockDisabledForUrl(url);
}

}
}