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

#pragma once
#ifndef SIELOBROWSER_ADBMANAGER_HPP
#define SIELOBROWSER_ADBMANAGER_HPP

#include "SharedDefines.hpp"

#include <QObject>
#include <QPointer>

#include <QStringList>
#include <QUrl>

#include <QWebEngine/UrlRequestInfo.hpp>

namespace Sn {
namespace ADB {
class Rule;

class Dialog;

class Matcher;

class CustomList;

class Subscription;

class UrlInterceptor;

class SIELO_SHAREDLIB Manager : public QObject {
Q_OBJECT

public:
	Manager(QObject* parent = nullptr);
	~Manager();

	void load();
	void save();

	bool isEnabled() const;
	bool canRunOnScheme(const QString& scheme) const;

	bool useLimitedEasyList() const;
	void setUseLimitedEasyList(bool useLimited);

	QString elementHidingRules(const QUrl& url) const;
	QString elementHidingRulesForDomain(const QUrl& url) const;

	Subscription* subscriptionByName(const QString& name) const;
	QList<Subscription*> subscriptions() const;

	bool addSubscriptionFromUrl(const QUrl& url);

	Subscription* addSubscription(const QString& title, const QString& url);
	bool removeSubscription(Subscription* subscription);

	bool block(Engine::UrlRequestInfo& request);

	QStringList disabledRules() const { return m_disabledRules; }

	void addDisabledRule(const QString& filter);
	void removeDisabledRule(const QString& filter);

	CustomList* customList() const;

	static Manager* instance();

signals:
	void enabledChanged(bool enabled);

public slots:
	void setEnabled(bool enabled);
	void showRule();

	void updateAllSubscriptions();

	Dialog* showDialog();

private:
	inline bool canBeBlocked(const QUrl& url) const;

	bool m_loaded{false};
	bool m_enabled{false};
	bool m_useLimitedEasyList{true};

	QList<Subscription*> m_subscriptions;
	QPointer<Dialog> m_adBlockDialog;
	Matcher* m_matcher{nullptr};
	UrlInterceptor* m_interceptor{nullptr};

	QStringList m_disabledRules;
};

}
}

#endif //SIELOBROWSER_ADBMANAGER_HPP
