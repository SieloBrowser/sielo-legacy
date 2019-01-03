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

#include "AdBlock/Matcher.hpp"


#include "AdBlock/Manager.hpp"
#include "AdBlock/Subscription.hpp"
#include "AdBlock/Rule.hpp"

namespace Sn {
namespace ADB {

Matcher::Matcher(Manager* manager) :
		QObject(manager),
		m_manager(manager)
{
	connect(manager, &Manager::enabledChanged, this, &Matcher::enabledChanged);
}

Matcher::~Matcher()
{
	clear();
}

const Rule* Matcher::match(const Engine::UrlRequestInfo& request, const QString& urlDomain,
						   const QString& urlString) const
{
	if (m_networkExceptionTree.find(request, urlDomain, urlString))
		return nullptr;

			foreach (const Rule* rule, m_networkExceptionRules) {
			if (rule->networkMatch(request, urlDomain, urlString))
				return nullptr;
		}

	if (const Rule* rule = m_networkBlockTree.find(request, urlDomain, urlString))
		return rule;

			foreach (const Rule* rule, m_networkBlockRules) {
			if (rule->networkMatch(request, urlDomain, urlString))
				return rule;
		}

	return nullptr;
}

bool Matcher::adBlockDisabledForUrl(const QUrl& url) const
{
	int count{m_documentRules.count()};

	for (int i{0}; i < count; ++i)
		if (m_documentRules[i]->urlMatch(url))
			return true;

	return false;
}

bool Matcher::elementHideDisabledForUrl(const QUrl& url) const
{
	if (adBlockDisabledForUrl(url))
		return true;

	int count{m_elementHideRules.count()};

	for (int i{0}; i < count; ++i)
		if (m_elementHideRules[i]->urlMatch(url))
			return true;

	return false;
}

QString Matcher::elementHidingRules() const
{
	return m_elementHidingRules;
}

QString Matcher::elementHidingRulesForDomain(const QString& domain) const
{
	QString rules{};
	int addedRulesCount{0};

			foreach(const Rule* rule, m_domainRestrictedCssRules) {
			if (!rule->matchDomain(domain))
				continue;

			if (Q_UNLIKELY(addedRulesCount == 1000)) {
				rules.append(rule->CSSSelector());
				rules.append(QLatin1String("{display:none !important;}\n"));

				addedRulesCount = 0;
			}
			else {
				rules.append(rule->CSSSelector() + QLatin1Char(','));
				++addedRulesCount;
			}
		}

	if (addedRulesCount != 0) {
		rules = rules.left(rules.size() - 1);
		rules.append(QLatin1String("{display:none !important;}\n"));
	}

	return rules;

}

void Matcher::update()
{
	clear();

	QHash<QString, const Rule*> CSSRulesHash;
	QVector<const Rule*> exceptionCSSRules;

			foreach (Subscription* subscription, m_manager->subscriptions()) {
					foreach (const Rule* rule, subscription->allRulles()) {
					if (rule->isInternalDisabled())
						continue;

					if (rule->isCSSRule()) {
						if (!rule->isEnabled())
							continue;

						if (rule->isException())
							exceptionCSSRules.append(rule);
						else
							CSSRulesHash.insert(rule->CSSSelector(), rule);
					}
					else if (rule->isDocument())
						m_documentRules.append(rule);
					else if (rule->isElementHide())
						m_elementHideRules.append(rule);
					else if (rule->isException()) {
						if (!m_networkExceptionTree.add(rule))
							m_networkExceptionRules.append(rule);
					}
					else {
						if (!m_networkBlockTree.add(rule))
							m_networkBlockRules.append(rule);
					}
				}
		}

			foreach (const Rule* rule, exceptionCSSRules) {
			const Rule* originalRule{CSSRulesHash.value(rule->CSSSelector())};

			if (!originalRule)
				continue;

			Rule* copiedRule{originalRule->copy()};

			copiedRule->m_options |= Rule::DomainRestrictedOption;
			copiedRule->m_blockedDomains.append(rule->m_allowedDomains);

			CSSRulesHash[rule->CSSSelector()] = copiedRule;

			m_createdRules.append(copiedRule);
		}

	int hidingRulesCount{0};

	QHashIterator<QString, const Rule*> it{CSSRulesHash};

	while (it.hasNext()) {
		it.next();

		const Rule* rule{it.value()};

		if (rule->isDomainRestricted())
			m_domainRestrictedCssRules.append(rule);
		else if (Q_UNLIKELY(hidingRulesCount == 1000)) {
			m_elementHidingRules.append(rule->CSSSelector());
			m_elementHidingRules.append(QLatin1String("{display:none !important;} "));

			hidingRulesCount = 0;
		}
		else {
			m_elementHidingRules.append(rule->CSSSelector() + QLatin1Char(','));

			++hidingRulesCount;
		}
	}

	if (hidingRulesCount != 0) {
		m_elementHidingRules = m_elementHidingRules.left(m_elementHidingRules.size() - 1);
		m_elementHidingRules.append(QLatin1String("{display:none !important;} "));
	}
}

void Matcher::clear()
{
	qDeleteAll(m_createdRules);
	m_createdRules.clear();
	m_networkExceptionRules.clear();
	m_networkBlockRules.clear();
	m_domainRestrictedCssRules.clear();
	m_documentRules.clear();
	m_elementHideRules.clear();

	m_elementHidingRules.clear();
	m_networkBlockTree.clear();
	m_networkExceptionTree.clear();
}

void Matcher::enabledChanged(bool enabled)
{
	if (enabled)
		update();
	else
		clear();
}

}
}