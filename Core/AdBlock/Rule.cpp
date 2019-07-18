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

#include "AdBlock/Rule.hpp"

#include <QList>

#include "AdBlock/SearchTree.hpp"
#include "AdBlock/Subscription.hpp"

namespace Sn {
namespace ADB {

static QString toSecondLevelDomain(const QUrl& url)
{
	const QString topLevelDomain{url.topLevelDomain()};
	const QString urlHost{url.host()};

	if (topLevelDomain.isEmpty() || url.host().isEmpty())
		return QString();

	QString domain{urlHost.left(urlHost.size() - topLevelDomain.size())};

	if (domain.count(QLatin1Char('.')) != 0)
		return urlHost;

	while (domain.count(QLatin1Char('.')) != 0)
		domain = domain.mid(domain.indexOf(QLatin1Char('.')) + 1);

	return domain + topLevelDomain;
}

Rule::Rule(const QString& filter, Subscription* subscription) :
		m_subscription(subscription),
		m_type(StringContainsMatchRule),
		m_caseSensitivity(Qt::CaseInsensitive),
		m_isEnabled(true),
		m_isException(false),
		m_isInternalDisabled(false)
{
	setFilter(filter);
}

Rule::~Rule()
{
	delete m_regExp;
}

Rule* Rule::copy() const
{
	Rule* rule{new Rule()};

	rule->m_subscription = m_subscription;
	rule->m_type = m_type;
	rule->m_options = m_options;
	rule->m_exceptions = m_exceptions;
	rule->m_filter = m_filter;
	rule->m_matchString = m_matchString;
	rule->m_caseSensitivity = m_caseSensitivity;
	rule->m_allowedDomains = m_allowedDomains;
	rule->m_blockedDomains = m_blockedDomains;
	rule->m_isEnabled = m_isEnabled;
	rule->m_isException = m_isException;
	rule->m_isInternalDisabled = m_isInternalDisabled;

	if (m_regExp) {
		rule->m_regExp = new ADBRegExp;
		rule->m_regExp->regExp = m_regExp->regExp;
		rule->m_regExp->matchers = m_regExp->matchers;
	}

	return rule;
}

void Rule::setSubscription(Subscription* subscription)
{
	m_subscription = subscription;
}

void Rule::setFilter(const QString& filter)
{
	m_filter = filter;
	parseFilter();
}

bool Rule::isComment() const
{
	return m_filter.startsWith(QLatin1Char('!'));
}

bool Rule::isEnabled() const
{
	return m_isEnabled;
}

void Rule::setEnabled(bool enabled)
{
	m_isEnabled = enabled;
}

bool Rule::isCSSRule() const
{
	return m_type == CSSRule;
}

QString Rule::CSSSelector() const
{
	return m_matchString;
}

bool Rule::isDocument() const
{
	return hasOption(DocumentOption);
}

bool Rule::isElementHide() const
{
	return hasOption(ElementHideOption);
}

bool Rule::isDomainRestricted() const
{
	return hasOption(DomainRestrictedOption);
}

bool Rule::isException() const
{
	return m_isException;
}

bool Rule::isSlow() const
{
	return m_regExp != nullptr;
}

bool Rule::isInternalDisabled() const
{
	return m_isInternalDisabled;
}

bool Rule::urlMatch(const QUrl& url) const
{
	if (!hasOption(DocumentOption) && !hasOption(ElementHideOption))
		return false;

	const QString& encodedUrl{url.toEncoded()};
	const QString& domain{url.host()};

	return stringMatch(domain, encodedUrl);
}

bool Rule::networkMatch(const Engine::UrlRequestInfo& request, const QString& domain,
						const QString& encodedUrl) const
{
	if (m_type == CSSRule || !m_isEnabled || m_isInternalDisabled)
		return false;

	bool matched{stringMatch(domain, encodedUrl)};

	if (matched) {
		if (hasOption(DomainRestrictedOption) && !matchDomain(request.firstPartyUrl().host()))
			return false;
		if (hasOption(ThirdPartyOption) && !matchThirdParty(request))
			return false;
		if (hasOption(ObjectOption) && !matchObject(request))
			return false;
		if (hasOption(SubdocumentOption) && !matchSubdocument(request))
			return false;
		if (hasOption(XMLHttpRequestOption) && !matchXMLHttpRequest(request))
			return false;
		if (hasOption(ImageOption) && !matchImage(request))
			return false;
		if (hasOption(ScriptOption) && !matchScript(request))
			return false;
		if (hasOption(StyleSheetOption) && !matchStyleSheet(request))
			return false;
		if (hasOption(ObjectSubrequestOption) && !matchObjectSubrequest(request))
			return false;
		if (hasOption(PingOption) && !matchPing(request))
			return false;
		if (hasOption(MediaOption) && !matchMedia(request))
			return false;

	}

	return matched;
}

bool Rule::matchDomain(const QString& domain) const
{
	if (!m_isEnabled) {
		return false;
	}

	if (!hasOption(DomainRestrictedOption)) {
		return true;
	}

	if (m_blockedDomains.isEmpty()) {
				foreach (const QString& d, m_allowedDomains) {
				if (isMatchingDomain(domain, d)) {
					return true;
				}
			}
	}
	else if (m_allowedDomains.isEmpty()) {
				foreach (const QString& d, m_blockedDomains) {
				if (isMatchingDomain(domain, d)) {
					return false;
				}
			}
		return true;
	}
	else {
				foreach (const QString& d, m_blockedDomains) {
				if (isMatchingDomain(domain, d)) {
					return false;
				}
			}

				foreach (const QString& d, m_allowedDomains) {
				if (isMatchingDomain(domain, d)) {
					return true;
				}
			}
	}

	return false;
}

bool Rule::matchThirdParty(const Engine::UrlRequestInfo& request) const
{
	const QString firstPartyHost{toSecondLevelDomain(request.firstPartyUrl())};
	const QString host{toSecondLevelDomain(request.requestUrl())};

	bool match{firstPartyHost != host};

	return hasException(ThirdPartyOption) == !match;
}

bool Rule::matchObject(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeObject};

	return hasException(ObjectOption) == !match;
}

bool Rule::matchSubdocument(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeSubFrame};

	return hasException(SubdocumentOption) == !match;
}

bool Rule::matchXMLHttpRequest(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeXhr};

	return hasException(XMLHttpRequestOption) == !match;
}

bool Rule::matchImage(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeImage};

	return hasException(ImageOption) == !match;
}

bool Rule::matchScript(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeScript};

	return hasException(ScriptOption) == !match;
}

bool Rule::matchStyleSheet(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeStylesheet};

	return hasException(StyleSheetOption) == !match;
}

bool Rule::matchObjectSubrequest(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypePluginResource};

	return hasException(ObjectSubrequestOption) == !match;
}

bool Rule::matchPing(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypePing};

	return hasException(PingOption) == !match;
}

bool Rule::matchMedia(const Engine::UrlRequestInfo& request) const
{
	bool match{request.resourceType() == Engine::UrlRequestInfo::ResourceTypeMedia};

	return hasException(MediaOption) == !match;
}

bool Rule::matchOther(const Engine::UrlRequestInfo& request) const
{
	bool match{
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeFontResource ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeSubResource ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeWorker ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeSharedWorker ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypePrefetch ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeFavicon ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeServiceWorker ||
			request.resourceType() == Engine::UrlRequestInfo::ResourceTypeUnknown
	};

	return hasException(MediaOption) == !match;
}

bool Rule::stringMatch(const QString& domain, const QString& encodedUrl) const
{
	if (m_type == StringContainsMatchRule)
		return encodedUrl.contains(m_matchString, m_caseSensitivity);
	else if (m_type == DomainMatchRule)
		return isMatchingDomain(domain, m_matchString);
	else if (m_type == StringEndsMatchRule)
		return encodedUrl.endsWith(m_matchString, m_caseSensitivity);
	else if (m_type == RegExpMatchRule) {
		if (!isMatchingRegExpString(encodedUrl))
			return false;

		return (m_regExp->regExp.indexIn(encodedUrl) != -1);
	}

	return false;
}

bool Rule::isMatchingDomain(const QString& domain, const QString& filter) const
{
	if (filter == domain)
		return true;

	if (!domain.endsWith(filter))
		return false;

	int index{domain.indexOf(filter)};

	return (index > 0 && domain[index - 1] == QLatin1Char('.'));
}

bool Rule::isMatchingRegExpString(const QString& url) const
{
	Q_ASSERT(m_regExp);

			foreach (const QStringMatcher& matcher, m_regExp->matchers) {
			if (matcher.indexIn(url) == -1)
				return false;
		}

	return true;
}

QStringList Rule::parseRegExpFilter(const QString& filter) const
{
	QStringList list;
	int startPosition{-1};

	for (int i{0}; i < filter.size(); ++i) {
		const QChar c{filter[i]};

		if (c == QLatin1Char('|') || c == QLatin1Char('*') || c == QLatin1Char('^')) {
			const QString sub{filter.mid(startPosition, i - startPosition)};

			if (sub.size() > 1)
				list.append(sub);

			startPosition = i + 1;
		}
	}

	const QString sub{filter.mid(startPosition)};

	if (sub.size() > 1)
		list.append(sub);

	list.removeDuplicates();

	return list;
}

bool Rule::hasOption(const RuleOption& option) const
{
	return (m_options & option);
}

void Rule::setOption(const RuleOption& option)
{
	m_options |= option;
}

bool Rule::hasException(const RuleOption& option) const
{
	return (m_exceptions & option);
}

void Rule::setException(const RuleOption& option, bool on)
{
	if (on)
		m_exceptions |= option;
}

void Rule::parseFilter()
{
	QString parsedLine{m_filter};

	if (m_filter.trimmed().isEmpty() || m_filter.startsWith(QLatin1Char('!'))) {
		m_isEnabled = false;
		m_isInternalDisabled = true;
		m_type = Invalide;

		return;
	}

	if (parsedLine.contains(QLatin1String("##")) || parsedLine.contains(QLatin1String("#@#"))) {
		m_type = CSSRule;
		int position{parsedLine.indexOf(QLatin1Char('#'))};

		if (!parsedLine.startsWith(QLatin1String("##"))) {
			QString domains{parsedLine.left(position)};
			parseDomains(domains, QLatin1Char(','));
		}

		m_isException = parsedLine[position + 1] == QLatin1Char('@');
		m_matchString = parsedLine.mid(m_isException ? position + 3 : position + 2);

		return;
	}

	if (parsedLine.startsWith(QLatin1String("@@"))) {
		m_isException = true;
		parsedLine = parsedLine.mid(2);
	}

	int optionsIndex{parsedLine.indexOf(QLatin1Char('$'))};

	if (optionsIndex >= 0) {
		const QStringList
				options{parsedLine.mid(optionsIndex + 1).split(QLatin1Char(','), QString::SkipEmptyParts)};
		int handledOptions{0};

				foreach (const QString& option, options) {
				if (option.startsWith(QLatin1String("domain="))) {
					parseDomains(option.mid(7), QLatin1Char('|'));
					++handledOptions;
				}
				else if (option == QLatin1String("match-case")) {
					m_caseSensitivity = Qt::CaseSensitive;
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("third-party"))) {
					setOption(ThirdPartyOption);
					setException(ThirdPartyOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("object"))) {
					setOption(ObjectOption);
					setException(ObjectOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("subdocument"))) {
					setOption(SubdocumentOption);
					setException(SubdocumentOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("xmlhttprequest"))) {
					setOption(XMLHttpRequestOption);
					setException(XMLHttpRequestOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("image"))) {
					setOption(ImageOption);
					setException(ImageOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("script"))) {
					setOption(ScriptOption);
					setException(ScriptOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("stylesheet"))) {
					setOption(StyleSheetOption);
					setException(StyleSheetOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option.endsWith(QLatin1String("object-subrequest"))) {
					setOption(ObjectSubrequestOption);
					setException(ObjectSubrequestOption, option.startsWith(QLatin1Char('~')));
					++handledOptions;
				}
				else if (option == QLatin1String("document") && m_isException) {
					setOption(DocumentOption);
					++handledOptions;
				}
				else if (option == QLatin1String("elemhide") && m_isException) {
					setOption(ElementHideOption);
					++handledOptions;
				}
				else if (option == QLatin1String("collapse")) {
					// Hiding placeholders of blocked elements is enabled by default
					++handledOptions;
				}
			}

		if (handledOptions != options.count()) {
			m_isInternalDisabled = true;
			m_type = Invalide;

			return;
		}

		parsedLine = parsedLine.left(optionsIndex);

	}

	if (parsedLine.startsWith(QLatin1Char('/')) && parsedLine.endsWith(QLatin1Char('/'))) {
		parsedLine = parsedLine.mid(1);
		parsedLine = parsedLine.left(parsedLine.size() - 1);

		m_type = RegExpMatchRule;

		m_regExp = new ADBRegExp;
		m_regExp->regExp = RegExp(parsedLine, m_caseSensitivity);
		m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));

		return;
	}

	if (parsedLine.startsWith(QLatin1Char('*')))
		parsedLine = parsedLine.mid(1);

	if (parsedLine.endsWith(QLatin1Char('*')))
		parsedLine = parsedLine.left(parsedLine.size() - 1);

	if (filterIsOnlyDomain(parsedLine)) {
		parsedLine = parsedLine.mid(2);
		parsedLine = parsedLine.left(parsedLine.size() - 1);

		m_type = DomainMatchRule;
		m_matchString = parsedLine;

		return;
	}

	if (filterIsOnlyEndsMatch(parsedLine)) {
		parsedLine = parsedLine.left(parsedLine.size() - 1);

		m_type = StringEndsMatchRule;
		m_matchString = parsedLine;

		return;
	}

	if (parsedLine.contains(QLatin1Char('*')) || parsedLine.contains(QLatin1Char('^'))
		|| parsedLine.contains(QLatin1Char('|'))) {
		m_type = RegExpMatchRule;
		m_regExp = new ADBRegExp;
		m_regExp->regExp = RegExp(createRegExpFromFilter(parsedLine), m_caseSensitivity);
		m_regExp->matchers = createStringMatchers(parseRegExpFilter(parsedLine));

		return;
	}

	m_type = StringContainsMatchRule;
	m_matchString = parsedLine;
}

void Rule::parseDomains(const QString& domains, const QChar& separator)
{
	QStringList domainsList = domains.split(separator, QString::SkipEmptyParts);

			foreach (const QString domain, domainsList) {
			if (domain.isEmpty())
				continue;
			if (domain.startsWith(QLatin1Char('~')))
				m_blockedDomains.append(domain.mid(1));
			else
				m_allowedDomains.append(domain);
		}

	if (!m_blockedDomains.isEmpty() || !m_allowedDomains.isEmpty())
		setOption(DomainRestrictedOption);
}

bool Rule::filterIsOnlyDomain(const QString& filter) const
{
	if (!filter.endsWith(QLatin1Char('^')) || !filter.startsWith(QLatin1String("||")))
		return false;

	for (int i = 0; i < filter.size(); ++i) {
		switch (filter.at(i).toLatin1()) {
		case '/':
		case ':':
		case '?':
		case '=':
		case '&':
		case '*':
			return false;
		default:
			break;
		}
	}

	return true;
}

bool Rule::filterIsOnlyEndsMatch(const QString& filter) const
{
	for (int i = 0; i < filter.size(); ++i) {
		switch (filter.at(i).toLatin1()) {
		case '^':
		case '*':
			return false;
		case '|':
			return i == filter.size() - 1;
		default:
			break;
		}
	}

	return false;
}

QString Rule::createRegExpFromFilter(const QString& filter) const
{
	QString parsed{};
	parsed.reserve(filter.size());

	bool hadWildcard{false};

	for (int i{0}; i < filter.size(); ++i) {
		const QChar c{filter[i]};

		switch (c.toLatin1()) {
		case '^':
			parsed.append(QLatin1String("(?:[^\\w\\d\\-.%]|$)"));
			break;
		case '*':
			if (!hadWildcard)
				parsed.append(QLatin1String(".*"));
			break;
		case '|':
			if (i == 0) {
				if (filter.size() > 1 && filter[1] == QLatin1Char('|')) {
					parsed.append(QLatin1String("^[\\w\\-]+:\\/+(?!\\/)(?:[^\\/]+\\.)?"));
					++i;
				}
				else
					parsed.append('^');
				break;
			}
			else if (i == filter.size() - 1) {
				parsed.append(QLatin1Char('$'));
				break;
			}
		default:
			if (!(c.isLetterOrNumber() || c.isMark() || c == QLatin1Char('-')))
				parsed.append(QLatin1Char('\\') + c);
			else
				parsed.append(c);
		}

		hadWildcard = c == QLatin1Char('*');
	}

	return parsed;
}

QList<QStringMatcher> Rule::createStringMatchers(const QStringList& filters) const
{
	QList<QStringMatcher> matchers;
	matchers.reserve(filters.size());

			foreach (const QString& filter, filters) matchers.append(QStringMatcher(filter, m_caseSensitivity));

	return matchers;
}

}
}