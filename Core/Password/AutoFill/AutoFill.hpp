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
#ifndef SIELO_BROWSER_AUTOFILL_HPP
#define SIELO_BROWSER_AUTOFILL_HPP

#include "SharedDefines.hpp"

#include <QObject>

#include <QUrl>

#include "Database/SqlDatabase.hpp"

namespace Sn {
class PasswordManager;

class WebPage;

struct PasswordEntry;

struct PageFormData {
	QString username{};
	QString password{};
	QByteArray postData{};

	bool isValid() const { return !password.isEmpty(); }
};

class SIELO_SHAREDLIB AutoFill: public QObject {
Q_OBJECT

public:
	AutoFill(QObject* parent = nullptr);

	PasswordManager* passwordManager() const { return m_manager; }

	void loadSettings();

	bool isStored(const QUrl& url);
	bool isStoringEnabled(const QUrl& url);
	void blockStoringForUrl(const QUrl& url);

	QVector<PasswordEntry> getFormData(const QUrl& url);
	QVector<PasswordEntry> getAllFormData();

	void updateLastUsed(PasswordEntry& data);

	void addEntry(const QUrl& url, const QString& name, const QString& password);
	void addEntry(const QUrl& url, const PageFormData& formData);

	void updateEntry(const QUrl& url, const QString& name, const QString& password);
	bool updateEntry(const PasswordEntry& entry);

	void removeEntry(const PasswordEntry& entry);
	void removeAllEntries();

	void saveForm(WebPage* page, const QUrl& frameUrl, const PageFormData& formData);
	QVector<PasswordEntry> completePage(WebPage* page, const QUrl& frameUrl);

	QByteArray exportPasswords();
	bool importPasswords(const QByteArray& data);


private:
	PasswordManager* m_manager{nullptr};
	bool m_isStoring{false};
};

}

#endif //SIELO_BROWSER_AUTOFILL_HPP
