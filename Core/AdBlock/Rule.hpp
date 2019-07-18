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
#ifndef SIELOBROWSER_ADBRULE_HPP
#define SIELOBROWSER_ADBRULE_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QStringList>
#include <QStringMatcher>

#include <QChar>

#include <QUrl>

#include <QWebEngine/UrlRequestInfo.hpp>

#include "Utils/RegExp.hpp"

namespace Sn {
namespace ADB {
class Subscription;

class Matcher;

class SearchTree;

class SIELO_SHAREDLIB Rule {
	Q_DISABLE_COPY(Rule);

public:
	Rule(const QString& filter = QString(), Subscription* subscription = nullptr);
	~Rule();

	Rule* copy() const;

	Subscription* subscriptions() const { return m_subscription; }

	void setSubscription(Subscription* subscription);

	QString filter() const { return m_filter; }

	void setFilter(const QString& filter);

	bool isComment() const;
	bool isEnabled() const;
	void setEnabled(bool enabled);

	bool isCSSRule() const;
	QString CSSSelector() const;

	bool isDocument() const;
	bool isElementHide() const;

	bool isDomainRestricted() const;
	bool isException() const;

	bool isSlow() const;
	bool isInternalDisabled() const;

	bool urlMatch(const QUrl& url) const;
	bool networkMatch(const Engine::UrlRequestInfo& request, const QString& domain, const QString& encodedUrl) const;

	bool matchDomain(const QString& domain) const;
	bool matchThirdParty(const Engine::UrlRequestInfo& request) const;
	bool matchObject(const Engine::UrlRequestInfo& request) const;
	bool matchSubdocument(const Engine::UrlRequestInfo& request) const;
	bool matchXMLHttpRequest(const Engine::UrlRequestInfo& request) const;
	bool matchImage(const Engine::UrlRequestInfo& request) const;
	bool matchScript(const Engine::UrlRequestInfo& request) const;
	bool matchStyleSheet(const Engine::UrlRequestInfo& request) const;
	bool matchObjectSubrequest(const Engine::UrlRequestInfo& request) const;
	bool matchPing(const Engine::UrlRequestInfo& request) const;
	bool matchMedia(const Engine::UrlRequestInfo& request) const;
	bool matchOther(const Engine::UrlRequestInfo& request) const;

protected:
	bool stringMatch(const QString& domain, const QString& encodedUrl) const;
	bool isMatchingDomain(const QString& domain, const QString& filter) const;
	bool isMatchingRegExpString(const QString& url) const;

	QStringList parseRegExpFilter(const QString& filter) const;

private:
	enum RuleType {
		CSSRule = 0,
		DomainMatchRule = 1,
		RegExpMatchRule = 2,
		StringEndsMatchRule = 3,
		StringContainsMatchRule = 4,
		Invalide = 5
	};

	enum RuleOption {
		NoOption = 0,
		DomainRestrictedOption = 1,
		ThirdPartyOption = 2,
		ObjectOption = 4,
		SubdocumentOption = 8,
		XMLHttpRequestOption = 16,
		ImageOption = 32,
		ScriptOption = 64,
		StyleSheetOption = 128,
		ObjectSubrequestOption = 256,
		PingOption = 512,
		MediaOption = 1024,
		OtherOption = 2048,

		// Exception only options
				DocumentOption = 4096,
		ElementHideOption = 8192
	};
	Q_DECLARE_FLAGS(RuleOptions, RuleOption)

	inline bool hasOption(const RuleOption& option) const;
	inline void setOption(const RuleOption& option);
	inline bool hasException(const RuleOption& option) const;
	inline void setException(const RuleOption& option, bool on);

	void parseFilter();
	void parseDomains(const QString& domains, const QChar& separator);
	bool filterIsOnlyDomain(const QString& filter) const;
	bool filterIsOnlyEndsMatch(const QString& filter) const;
	QString createRegExpFromFilter(const QString& filter) const;
	QList<QStringMatcher> createStringMatchers(const QStringList& filters) const;

	Subscription* m_subscription{nullptr};

	struct ADBRegExp {
		RegExp regExp;
		QList<QStringMatcher> matchers;

	};

	ADBRegExp* m_regExp{nullptr};

	RuleType m_type;
	RuleOptions m_options;
	RuleOptions m_exceptions;

	QString m_filter{};
	QString m_matchString{};
	Qt::CaseSensitivity m_caseSensitivity{};

	QStringList m_allowedDomains;
	QStringList m_blockedDomains;

	bool m_isEnabled{true};
	bool m_isException{false};
	bool m_isInternalDisabled{false};

	friend class Matcher;

	friend class SearchTree;

	friend class Subscription;
};
}
}

#endif //SIELOBROWSER_ADBRULE_HPP
