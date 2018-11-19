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
#ifndef SIELOBROWSER_ADBCUSTOMLIST_HPP
#define SIELOBROWSER_ADBCUSTOMLIST_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QStringList>

#include "AdBlock/Subscription.hpp"

namespace Sn {
namespace ADB {
class Rule;

class SIELO_SHAREDLIB CustomList : public Subscription {
Q_OBJECT

public:
	CustomList(QObject* parent = nullptr);

	void loadSubscription(const QStringList& disabledRules);
	void saveSubscription();

	bool canEditRules() const { return true; }

	bool canBeRemoved() const { return false; }

	bool containsFilter(const QString& filter) const;
	bool removeFilter(const QString& filter);

	int addRule(Rule* rule);
	bool removeRule(int offset);
	const Rule* replaceRule(Rule* rule, int offset);
};
}
}

#endif //SIELOBROWSER_ADBCUSTOMLIST_HPP
