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
#ifndef SIELOBROWSER_ADBLOCKPAGE_HPP
#define SIELOBROWSER_ADBLOCKPAGE_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QDialog>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QAction>

#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTabWidget>
#include <QSpacerItem>

#include "AdBlock/Manager.hpp"
#include "AdBlock/TreeWidget.hpp"
#include "AdBlock/Subscription.hpp"
#include "AdBlock/Rule.hpp"

namespace Sn {

class SIELO_SHAREDLIB AdBlockPage: public QWidget {
Q_OBJECT

public:
	AdBlockPage(QWidget* parent = nullptr);
	~AdBlockPage();

	void loadSettings();

	void showRule(const ADB::Rule* rule) const;

private slots:
	void addRule();
	void removeRule();

	void addSubscription();
	void removeSubscription();

	void currentChanged(int index);
	void filterString(const QString& string);
	void enableAdBlock(bool state);

	void aboutToShowMenu();
	void learnAboutRules();

	void loadSubscription();
	void load();
private:
	void setupUI();

	ADB::Manager* m_manager{nullptr};
	ADB::TreeWidget* m_currentTreeWidget{nullptr};
	ADB::Subscription* m_currentSubscription{nullptr};

	QAction* m_actionAddRule{nullptr};
	QAction* m_actionRemoveRule{nullptr};
	QAction* m_actionAddSubscription{nullptr};
	QAction* m_actionRemoveSubscription{nullptr};

	QVBoxLayout* m_layout{nullptr};
	QHBoxLayout* m_searchLayout{nullptr};
	QHBoxLayout* m_optionsLayout{nullptr};
	QGridLayout* m_adBlockWidgetLayout{nullptr};

	QCheckBox* m_enableAdBlock{nullptr};
	QLineEdit* m_search{nullptr};
	QWidget* m_adBlockWidget{nullptr};
	QTabWidget* m_tabWidget{nullptr};
	QPushButton* m_optionsButton{nullptr};
	QLabel* m_adbLabel{nullptr};

	QSpacerItem* m_searchSpacer{nullptr};
	QSpacerItem* m_optionsSpacer{nullptr};

	bool m_loaded{false};
};

}

#endif //SIELOBROWSER_ADBLOCKPAGE_HPP
