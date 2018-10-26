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

#include "AdBlock/CustomList.hpp"

#include "Application.hpp"

#include "AdBlock/Manager.hpp"
#include "AdBlock/Rule.hpp"

#include "Utils/DataPaths.hpp"

namespace Sn {
namespace ADB {

CustomList::CustomList(QObject* parent) :
		Subscription(tr("Custom Rules"), parent)
{
	setFilePath(DataPaths::currentProfilePath() + QLatin1String("/adblock/customlist.txt"));
}

void CustomList::loadSubscription(const QStringList& disabledRules)
{
	QFile file(filePath());
	if (!file.exists()) {
		saveSubscription();
	}

	//TODO: Why not add own Sielo subscription ?
	Subscription::loadSubscription(disabledRules);
}

void CustomList::saveSubscription()
{
	QFile file{filePath()};

	if (!file.open(QFile::ReadWrite | QFile::Truncate)) {
		qWarning() << "ADB::Subscription: " << __FUNCTION__ << " Unable to open adblock file for writing "
				   << filePath();
		return;
	}

	QTextStream textStream{&file};

	textStream.setCodec("UTF-8");
	textStream << "Title: " << title() << endl;
	textStream << "Url: " << url().toString() << endl;
	textStream << "[Adblock Plus 2.0]" << endl;

			foreach (const Rule* rule, m_rules) textStream << rule->filter() << endl;

	file.close();
}

bool CustomList::containsFilter(const QString& filter) const
{
			foreach (const Rule* rule, m_rules) {
			if (rule->filter() == filter)
				return true;
		}

	return false;
}

bool CustomList::removeFilter(const QString& filter)
{
	for (int i{0}; i < m_rules.count(); ++i) {
		const Rule* rule{m_rules[i]};

		if (rule->filter() == filter)
			return removeRule(i);
	}

	return false;
}

int CustomList::addRule(Rule* rule)
{
	m_rules.append(rule);

	emit subscriptionChanged();

	if (rule->isCSSRule())
		Application::instance()->reloadUserStyleSheet();

	return m_rules.count() - 1;
}

bool CustomList::removeRule(int offset)
{
	if (!(offset >= 0 && m_rules.count() > offset))
		return false;

	Rule* rule{m_rules[offset]};
	const QString filter{rule->filter()};

	m_rules.remove(offset);

	emit subscriptionChanged();

	if (rule->isCSSRule())
		Application::instance()->reloadUserStyleSheet();

	Manager::instance()->removeDisabledRule(filter);

	delete rule;

	return true;
}

const Rule* CustomList::replaceRule(Rule* rule, int offset)
{
	if (!(offset >= 0 && m_rules.count() > offset))
		return nullptr;

	Rule* oldRule{m_rules[offset]};
	m_rules[offset] = rule;

	emit subscriptionChanged();

	if (rule->isCSSRule())
		Application::instance()->reloadUserStyleSheet();

	delete oldRule;

	return m_rules[offset];
}

}
}