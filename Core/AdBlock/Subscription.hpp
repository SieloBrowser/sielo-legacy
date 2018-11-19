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
#ifndef SIELOBROWSER_ADBSUBSCRIPTION_HPP
#define SIELOBROWSER_ADBSUBSCRIPTION_HPP

#include "SharedDefines.hpp"

#include <QVector>

#include <QUrl>
#include <QNetworkReply>

namespace Sn {
namespace ADB {
class Rule;

class SIELO_SHAREDLIB Subscription : public QObject {
Q_OBJECT

public:
	Subscription(const QString& title, QObject* parent = nullptr);
	~Subscription();

	QString title() const { return m_title; }

	QString filePath() const { return m_filePath; }

	void setFilePath(const QString& path);

	QUrl url() const { return m_url; }

	void setUrl(const QUrl& url);

	virtual void loadSubscription(const QStringList& disabledRules);
	virtual void saveSubscription();

	const Rule* rule(int offset) const;
	QVector<Rule*> allRulles() const;

	const Rule* enableRule(int offset);
	const Rule* disableRule(int offset);

	virtual bool canEditRules() const { return false; }

	virtual bool canBeRemoved() const { return true; }

	virtual int addRule(Rule* rule);
	virtual bool removeRule(int offset);
	virtual const Rule* replaceRule(Rule* rule, int offset);

signals:
	void subscriptionChanged();
	void subscriptionUpdated();
	void subscriptionError(const QString& msg);

public slots:
	void updateSubscription();

protected:
	virtual bool saveDownloadedData(const QByteArray& data);

	QNetworkReply* m_reply{nullptr};
	QVector<Rule*> m_rules;

protected slots:
	void subscriptionDownloaded();

private:
	QString m_title{};
	QString m_filePath{};

	QUrl m_url{};

	bool m_updated{false};
};

}
}

#endif //SIELOBROWSER_ADBSUBSCRIPTION_HPP
