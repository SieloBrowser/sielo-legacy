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

#include "Utils/CommandLineOption.hpp"

#include <iostream>

#include <QCoreApplication>

#include <QFileInfo>

#include <QCommandLineOption>
#include <QCommandLineParser>

namespace Sn {

CommandLineOption::CommandLineOption()
{
	parseActions();
}

void CommandLineOption::parseActions()
{
	QCommandLineOption authorsOption{QStringList() << QStringLiteral("a") << QStringLiteral("authors")};
	authorsOption.setDescription(QStringLiteral("Display author information."));

	QCommandLineOption
		privateBrowsingOption(QStringList() << QStringLiteral("i") << QStringLiteral("private-browsing"));
	privateBrowsingOption.setDescription(QStringLiteral("Starts private browsing."));

	QCommandLineOption noRemoteOption{QStringList() << QStringLiteral("r") << QStringLiteral("no-remote")};
	noRemoteOption.setDescription(QStringLiteral("Starts new browser instance."));

	QCommandLineOption newTabOption{QStringList() << QStringLiteral("t") << QStringLiteral("new-tab")};
	newTabOption.setDescription(QStringLiteral("Opens new tab."));

	QCommandLineOption newWindowOption{QStringList() << QStringLiteral("w") << QStringLiteral("new-window")};
	newWindowOption.setDescription(QStringLiteral("Opens new window"));

	QCommandLineOption profileOption(QStringList({QStringLiteral("p"), QStringLiteral("profile")}));
	profileOption.setValueName(QStringLiteral("profileName"));
	profileOption.setDescription(QStringLiteral("Starts with specified profile."));

	QCommandLineOption currentTabOption{QStringList() << QStringLiteral("c") << QStringLiteral("current-tab")};
	currentTabOption.setValueName(QStringLiteral("URL"));
	currentTabOption.setDescription(QStringLiteral("Opens URL in current tab"));

	QCommandLineOption openWindowOption(QStringList() << QStringLiteral("u") << QStringLiteral("open-window"));
	openWindowOption.setValueName(QStringLiteral("URL"));
	openWindowOption.setDescription(QStringLiteral("Opens URL in new window."));

	QCommandLineParser parser{};
	parser.setApplicationDescription(QStringLiteral("A fast web browser in C++ with Qt"));

	QCommandLineOption helpOption{parser.addHelpOption()};
	QCommandLineOption versionOption{parser.addVersionOption()};

	parser.addOption(authorsOption);
	parser.addOption(privateBrowsingOption);
	parser.addOption(noRemoteOption);
	parser.addOption(newTabOption);
	parser.addOption(newWindowOption);
	parser.addOption(profileOption);
	parser.addOption(currentTabOption);
	parser.addOption(openWindowOption);

	parser.addPositionalArgument(QStringLiteral("URL"), QStringLiteral("URLs to open"), QStringLiteral("[URL...]"));

	parser.parse(QCoreApplication::arguments());

	if (parser.isSet(helpOption))
		parser.showHelp();

	if (parser.isSet(authorsOption)) {
		std::cout << "Victor DENIS <admin@feldrise.com>" << std::endl;

		ActionPair pair;
		pair.action = Application::CL_ExitAction;
		m_actions.append(pair);
		return;
	}

	if (parser.isSet(privateBrowsingOption)) {
		ActionPair pair;
		pair.action = Application::CL_StartPrivateBrowsing;

		m_actions.append(pair);
	}

	if (parser.isSet(noRemoteOption)) {
		ActionPair pair;
		pair.action = Application::CL_StartNewInstance;

		m_actions.append(pair);
	}

	if (parser.isSet(newTabOption)) {
		ActionPair pair;
		pair.action = Application::CL_NewTab;

		m_actions.append(pair);
	}

	if (parser.isSet(newWindowOption)) {
		ActionPair pair;
		pair.action = Application::CL_NewWindow;

		m_actions.append(pair);
	}

	if (parser.isSet(profileOption)) {
		const QString profileName{parser.value(profileOption)};
		
		ActionPair pair;
		pair.action = Application::CL_StartWithProfile;
		pair.text = profileName;
		
		m_actions.append(pair);
	}

	if (parser.isSet(currentTabOption)) {
		ActionPair pair;
		pair.action = Application::CL_OpenUrlInCurrentTab;
		pair.text = parser.value(currentTabOption);

		m_actions.append(pair);
	}

	if (parser.isSet(openWindowOption)) {
		ActionPair pair;
		pair.action = Application::CL_OpenUrlInNewWindow;
		pair.text = parser.value(openWindowOption);

		m_actions.append(pair);
	}

	if (parser.positionalArguments().isEmpty())
		return;

	QString url{parser.positionalArguments().last()};
	QFileInfo fileInfo{url};

	if (fileInfo.exists())
		url = fileInfo.absoluteFilePath();

	if (!url.isEmpty() && !url.startsWith(QLatin1Char('-'))) {
		ActionPair pair;
		pair.action = Application::CL_OpenUrl;
		pair.text = url;

		m_actions.append(pair);
	}

	// http://feldrise.com




}

}