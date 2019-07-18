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

#include "AutoFill.hpp"

#include <QWebEngine/WebProfile.hpp>

#include <QSqlQuery>

#include <QMessageBox>

#include "Password/PasswordManager.hpp"

#include "Utils/Settings.hpp"

#include "Web/Scripts.hpp"
#include "Web/WebPage.hpp"
#include "Web/WebView.hpp"

#include "Application.hpp"
#include "AutoFillNotification.hpp"

namespace Sn {

AutoFill::AutoFill(QObject* parent) :
		QObject(parent),
		m_manager(new PasswordManager(this)),
		m_isStoring(false)
{
	loadSettings();

	QString source = QLatin1String("(function() {"
								   "function findUsername(inputs) {"
								   "    for (var i = 0; i < inputs.length; ++i)"
								   "        if (inputs[i].type == 'text' && inputs[i].value.length && inputs[i].name.indexOf('user') != -1)"
								   "            return inputs[i].value;"
								   "    for (var i = 0; i < inputs.length; ++i)"
								   "        if (inputs[i].type == 'text' && inputs[i].value.length && inputs[i].name.indexOf('name') != -1)"
								   "            return inputs[i].value;"
								   "    for (var i = 0; i < inputs.length; ++i)"
								   "        if (inputs[i].type == 'text' && inputs[i].value.length)"
								   "            return inputs[i].value;"
								   "    for (var i = 0; i < inputs.length; ++i)"
								   "        if (inputs[i].type == 'email' && inputs[i].value.length)"
								   "            return inputs[i].value;"
								   "    return '';"
								   "}"
								   ""
								   "function registerForm(form) {"
								   "    form.addEventListener('submit', function() {"
								   "        var form = this;"
								   "        var data = '';"
								   "        var password = '';"
								   "        var inputs = form.getElementsByTagName('input');"
								   "        for (var i = 0; i < inputs.length; ++i) {"
								   "            var input = inputs[i];"
								   "            var type = input.type.toLowerCase();"
								   "            if (type != 'text' && type != 'password' && type != 'email')"
								   "                continue;"
								   "            if (!password && type == 'password')"
								   "                password = input.value;"
								   "            data += encodeURIComponent(input.name);"
								   "            data += '=';"
								   "            data += encodeURIComponent(input.value);"
								   "            data += '&';"
								   "        }"
								   "        if (!password)"
								   "            return;"
								   "        data = data.substring(0, data.length - 1);"
								   "        var url = window.location.href;"
								   "        var username = findUsername(inputs);"
								   "        external.autoFill.formSubmitted(url, username, password, data);"
								   "    }, true);"
								   "}"
								   ""
								   "if (!document.documentElement) return;"
								   ""
								   "for (var i = 0; i < document.forms.length; ++i)"
								   "    registerForm(document.forms[i]);"
								   ""
								   "var observer = new MutationObserver(function(mutations) {"
								   "    for (var i = 0; i < mutations.length; ++i)"
								   "        for (var j = 0; j < mutations[i].addedNodes.length; ++j)"
								   "            if (mutations[i].addedNodes[j].tagName == 'form')"
								   "                registerForm(mutations[i].addedNodes[j]);"
								   "});"
								   "observer.observe(document.documentElement, { childList: true });"
								   ""
								   "})()");

	Application::instance()->webProfile()->insertScript(QStringLiteral("_sielo_autofill"),
														source,
														Engine::WebProfile::DocumentReady,
														Engine::WebProfile::MainWorld,
														true);
}

void AutoFill::loadSettings()
{
	Settings settings{};

	m_isStoring = settings.value("Settings/savePasswordsOnSites", true).toBool();
}

bool AutoFill::isStored(const QUrl& url)
{
	if (!isStoringEnabled(url))
		return false;

	return !m_manager->getEntries(url).isEmpty();
}

bool AutoFill::isStoringEnabled(const QUrl& url)
{
	if (!m_isStoring)
		return false;

	QString server{url.host()};

	if (server.isEmpty())
		server = url.toString();

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("SELECT count(id) FROM autofill_exceptions WHERE server=?");
	query.addBindValue(server);
	query.exec();

	if (!query.next())
		return false;

	return query.value(0).toInt() <= 0;
}

void AutoFill::blockStoringForUrl(const QUrl& url)
{
	QString server{url.host()};

	if (server.isEmpty())
		server = url.toString();

	QSqlQuery query{SqlDatabase::instance()->database()};
	query.prepare("INSERT INTO autofill_exceptions (server) VALUES (?)");
	query.addBindValue(server);
	query.exec();
}

QVector<PasswordEntry> AutoFill::getFormData(const QUrl& url)
{
	return m_manager->getEntries(url);
}

QVector<PasswordEntry> AutoFill::getAllFormData()
{
	return m_manager->getAllEntries();
}

void AutoFill::updateLastUsed(PasswordEntry& data)
{
	m_manager->updateLastUsed(data);
}

void AutoFill::addEntry(const QUrl& url, const QString& name, const QString& password)
{
	PasswordEntry entry{};

	entry.host = PasswordManager::createHost(url);
	entry.username = name;
	entry.password = password;

	m_manager->addEntry(entry);
}

void AutoFill::addEntry(const QUrl& url, const PageFormData& formData)
{
	PasswordEntry entry{};

	entry.host = PasswordManager::createHost(url);
	entry.username = formData.username;
	entry.password = formData.password;
	entry.data = formData.postData;

	m_manager->addEntry(entry);
}

void AutoFill::updateEntry(const QUrl& url, const QString& name, const QString& password)
{
	PasswordEntry entry{};

	entry.host = PasswordManager::createHost(url);
	entry.username = name;
	entry.password = password;

	m_manager->updateEntry(entry);
}

bool AutoFill::updateEntry(const PasswordEntry& entry)
{
	return m_manager->updateEntry(entry);
}

void AutoFill::removeEntry(const PasswordEntry& entry)
{
	m_manager->removeEntry(entry);
}

void AutoFill::removeAllEntries()
{
	m_manager->removeAllEntries();
}

void AutoFill::saveForm(WebPage* page, const QUrl& frameUrl, const PageFormData& formData)
{
	if (Application::instance()->privateBrowsing() || !page)
		return;

	if (!isStoringEnabled(frameUrl))
		return;

	PasswordEntry updateData{};

	if (isStored(frameUrl)) {
		const QVector<PasswordEntry>& list = getFormData(frameUrl);

				foreach (const PasswordEntry& data, list) {
				if (data.username == formData.username) {
					updateData = data;
					updateLastUsed(updateData);

					if (data.password == formData.password) {
						updateData.password.clear();
						return;
					}

					updateData.username = formData.username;
					updateData.password = formData.password;
					updateData.data = formData.postData;

					break;
				}
			}
	}

	AutoFillNotification* notification{new AutoFillNotification(frameUrl, formData, updateData)};
	page->view()->addNotification(notification);
}

QVector<PasswordEntry> AutoFill::completePage(WebPage* page, const QUrl& frameUrl)
{
	QVector<PasswordEntry> list;

	if (!page || !isStored(frameUrl))
		return list;

	list = getFormData(frameUrl);

	if (!list.isEmpty()) {
		// TODO: move this to scripts
		QString source = QLatin1String("(function() {"
									   "var data = '%1'.split('&');"
									   "var inputs = document.getElementsByTagName('input');"
									   ""
									   "for (var i = 0; i < data.length; ++i) {"
									   "    var pair = data[i].split('=');"
									   "    if (pair.length != 2)"
									   "        continue;"
									   "    var key = decodeURIComponent(pair[0]);"
									   "    var val = decodeURIComponent(pair[1]);"
									   "    for (var j = 0; j < inputs.length; ++j) {"
									   "        var input = inputs[j];"
									   "        var type = input.type.toLowerCase();"
									   "        if (type != 'text' && type != 'password' && type != 'email')"
									   "            continue;"
									   "        if (input.name == key)"
									   "            input.value = val;"
									   "    }"
									   "}"
									   ""
									   "})()");
		const PasswordEntry entry = list[0];
		QString data{entry.data};

		data.replace(QLatin1String("'"), QLatin1String("\\'"));

		page->runJavaScript(source.arg(data), Engine::WebProfile::ScriptWorldId::ApplicationWorld);
	}

	return list;
}

QByteArray AutoFill::exportPasswords()
{
	//TODO: do
	QMessageBox::critical(nullptr, tr("No"), tr("You can't export password yet"));

	return QByteArray();
}

bool AutoFill::importPasswords(const QByteArray& data)
{
	Q_UNUSED(data);
	//TODO: do
	QMessageBox::critical(nullptr, tr("No"), tr("You can't import password yet"));

	return false;
}

}
