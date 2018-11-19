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
#ifndef SIELOBROWSER_PREFERENCESDIALOG_HPP
#define SIELOBROWSER_PREFERENCESDIALOG_HPP

#include "SharedDefines.hpp"

#include <QDialog>

#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QDialogButtonBox>
#include <QLabel>
#include <QListWidget>
#include <QStackedWidget>
#include <QScrollArea>
#include <QTabWidget>

namespace Sn {
class GeneralPage;
class AppearancePage;
class WebConfigPage;
class LocalStoragePage;
class ProxyConfigPage;
class PasswordPage;
class PrivacyPage;
class PluginsManager;
class DownloadPage;
class AdBlockPage;
class CurrentTabsSpacePage;

class TabWidget;

class SIELO_SHAREDLIB PreferencesDialog: public QDialog {
Q_OBJECT

public:
	PreferencesDialog(TabWidget* tabWidget, QWidget* parent = nullptr);
	~PreferencesDialog();

	void reloadTheme();

private slots:
	void saveSettings();

	void buttonClicked(QAbstractButton* button);
	void showStackedPage(QListWidgetItem* item);

private:
	void addPage(QWidget* page, const QString& name, const QIcon& icon = QIcon());
	void setupUI();

	QGridLayout* m_layout{nullptr};

	QListWidget* m_list{nullptr};
	QLabel* m_caption{nullptr};
	QScrollArea* m_scrollArea{nullptr};
	QStackedWidget* m_pages{nullptr};
	QDialogButtonBox* m_buttonBox{nullptr};
	QFrame* m_topLine{nullptr};
	QFrame* m_bottomLine{nullptr};

	GeneralPage* m_generalPage{nullptr};
	AppearancePage* m_appearancePage{nullptr};
	WebConfigPage* m_webConfigPage{nullptr};
	LocalStoragePage* m_localStoragePage{nullptr};
	ProxyConfigPage* m_proxyConfigPage{nullptr};
	PasswordPage* m_passwordPage{nullptr};
	PrivacyPage* m_privacyPage{nullptr};
	PluginsManager* m_pluginsPage{nullptr};
	DownloadPage* m_downloadPage{nullptr};
	AdBlockPage* m_pageAdBlock{nullptr};
	CurrentTabsSpacePage* m_currentTabsSpacePage{nullptr};

	TabWidget* m_tabWidget{nullptr};
};

}

#endif //SIELOBROWSER_PREFERENCESDIALOG_HPP
