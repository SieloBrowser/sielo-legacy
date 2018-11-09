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

#include "AdBlockPage.hpp"

#include <QTimer>

#include <QMenu>
#include <QMessageBox>

#include "AdBlock/SubscriptionDialog.hpp"

#include "Utils/Settings.hpp"

#include "Application.hpp"

namespace Sn {

AdBlockPage::AdBlockPage(QWidget* parent) :
	QWidget(parent),
	m_manager(ADB::Manager::instance())
{
	setupUI();
	loadSettings();

	connect(m_enableAdBlock, &QCheckBox::toggled, this, &AdBlockPage::enableAdBlock);
	connect(m_search, &QLineEdit::textChanged, this, &AdBlockPage::filterString);
	connect(m_tabWidget, &QTabWidget::currentChanged, this, &AdBlockPage::currentChanged);

	load();
}

AdBlockPage::~AdBlockPage()
{
	// Empty
}

void AdBlockPage::loadSettings()
{
	Settings settings{};

	settings.beginGroup("AdBlock-Settings");

	m_enableAdBlock->setChecked(settings.value(QLatin1String("enabled"), false).toBool());

	settings.endGroup();
}

void AdBlockPage::showRule(const ADB::Rule* rule) const
{
	ADB::Subscription* subscription{rule->subscriptions()};

	if (!subscription)
		return;

	for (int i{0}; i < m_tabWidget->count(); ++i) {
		ADB::TreeWidget* treeWidget = qobject_cast<ADB::TreeWidget*>(m_tabWidget->widget(i));

		if (subscription == treeWidget->subscription()) {
			treeWidget->showRule(rule);
			m_tabWidget->setCurrentIndex(i);
			break;
		}
	}
}

void AdBlockPage::addRule()
{
	m_currentTreeWidget->addRule();
}

void AdBlockPage::removeRule()
{
	m_currentTreeWidget->removeRule();
}

void AdBlockPage::addSubscription()
{
	ADB::SubscriptionDialog dialog{this};

	if (dialog.exec() != QDialog::Accepted)
		return;

	QString title{dialog.title()};
	QString url{dialog.address()};

	if (ADB::Subscription* subscription = m_manager->addSubscription(title, url)) {
		ADB::TreeWidget* tree{new ADB::TreeWidget(subscription, m_tabWidget)};
		int index{m_tabWidget->insertTab(m_tabWidget->count() - 1, tree, subscription->title())};

		m_tabWidget->setCurrentIndex(index);
	}
}

void AdBlockPage::removeSubscription()
{
	if (m_manager->removeSubscription(m_currentSubscription))
		delete m_currentTreeWidget;
}

void AdBlockPage::currentChanged(int index)
{
	if (index != -1) {
		m_currentTreeWidget = qobject_cast<ADB::TreeWidget*>(m_tabWidget->widget(index));
		m_currentSubscription = m_currentTreeWidget->subscription();
	}
}

void AdBlockPage::filterString(const QString& string)
{
	Q_UNUSED(string);
	QMessageBox::critical(this, tr("Error"), tr("The feature you request is not implemented yet."));
}

void AdBlockPage::enableAdBlock(bool state)
{
	Settings settings{};
	settings.setValue(QLatin1String("AdBlock-Settings/enabled"), state);

	m_manager->setEnabled(state);

	if (state)
		load();
}

void AdBlockPage::aboutToShowMenu()
{
	bool subscriptionEditable{m_currentSubscription && m_currentSubscription->canEditRules()};
	bool subscriptionRemovable{m_currentSubscription && m_currentSubscription->canBeRemoved()};

	m_actionAddRule->setEnabled(subscriptionEditable);
	m_actionRemoveRule->setEnabled(subscriptionEditable);
	m_actionRemoveSubscription->setEnabled(subscriptionRemovable);
}

void AdBlockPage::learnAboutRules()
{
	Application::instance()->addNewTab(QUrl("http://adblockplus.org/en/filters"));
}

void AdBlockPage::loadSubscription()
{
	for (int i{0}; i < m_tabWidget->count(); ++i) {
		ADB::TreeWidget* treeWidget = qobject_cast<ADB::TreeWidget*>(m_tabWidget->widget(i));
		treeWidget->refresh();
	}
}

void AdBlockPage::load()
{
	if (m_loaded || !m_enableAdBlock->isChecked())
		return;

		foreach (ADB::Subscription* subscription, m_manager->subscriptions()) {
			ADB::TreeWidget* tree{new ADB::TreeWidget(subscription, m_tabWidget)};
			m_tabWidget->addTab(tree, subscription->title());
		}

	m_loaded = true;

	QTimer::singleShot(50, this, &AdBlockPage::loadSubscription);
}

void AdBlockPage::setupUI()
{
	m_adBlockWidget = new QWidget(this);

	m_layout = new QVBoxLayout(this);
	m_searchLayout = new QHBoxLayout();
	m_adBlockWidgetLayout = new QGridLayout(m_adBlockWidget);
	m_optionsLayout = new QHBoxLayout();

	m_adBlockWidgetLayout->setContentsMargins(0, 0, 0, 0);

	m_enableAdBlock = new QCheckBox(tr("Enable AdBlock (Be aware, it is an experimental version!)"), this);

	m_search = new QLineEdit(this);
	m_search->setPlaceholderText(tr("Search..."));
	m_searchSpacer = new QSpacerItem(50, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

	m_tabWidget = new QTabWidget(this);
	m_tabWidget->setDocumentMode(true);

	m_optionsButton = new QPushButton(tr("Options"), this);

	QMenu* menu{new QMenu(m_optionsButton)};
	m_actionAddRule = menu->addAction(tr("Add Rule"), this, &AdBlockPage::addRule);
	m_actionRemoveRule = menu->addAction(tr("Remove Rule"), this, &AdBlockPage::removeRule);
	menu->addSeparator();
	m_actionAddSubscription = menu->addAction(tr("Add Subscription"), this, &AdBlockPage::addSubscription);
	m_actionRemoveSubscription = menu->addAction(tr("Remove Subscription"), this, &AdBlockPage::removeSubscription);
	menu->addAction(tr("Update Subscription"), m_manager, &ADB::Manager::updateAllSubscriptions);
	menu->addSeparator();
	menu->addAction(tr("Learn about writing rules..."), this, &AdBlockPage::learnAboutRules);

	m_optionsButton->setMenu(menu);
	connect(menu, &QMenu::aboutToShow, this, &AdBlockPage::aboutToShowMenu);

	m_optionsSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	m_adbLabel = new QLabel(tr("AdBlock"));

	m_optionsLayout->addWidget(m_optionsButton);
	m_optionsLayout->addItem(m_optionsSpacer);
	m_optionsLayout->addWidget(m_adbLabel);

	m_adBlockWidgetLayout->addWidget(m_tabWidget, 0, 0, 1, 1);
	m_adBlockWidgetLayout->addLayout(m_optionsLayout, 1, 0, 1, 1);

	m_searchLayout->addWidget(m_search);
	m_searchLayout->addItem(m_searchSpacer);

	m_layout->addWidget(m_enableAdBlock);
	m_layout->addLayout(m_searchLayout);
	m_layout->addWidget(m_adBlockWidget);
}
}