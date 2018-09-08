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

#include "TreeWidget.hpp"

#include <QInputDialog>

#include <QMenu>
#include <QAction>

#include <QApplication>
#include <QClipboard>

#include "AdBlock/Rule.hpp"
#include "AdBlock/Subscription.hpp"

namespace Sn {
namespace ADB {

TreeWidget::TreeWidget(Subscription* subscription, QWidget* parent) :
	QTreeWidget(parent),
	m_subscription(subscription),
	m_itemChangingBlock(false)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setHeaderHidden(true);
	setAlternatingRowColors(true);
	setLayoutDirection(Qt::LeftToRight);

	connect(this, &QTreeWidget::customContextMenuRequested, this, &TreeWidget::contextMenuRequested);
	connect(this, &QTreeWidget::itemChanged, this, &TreeWidget::itemChanged);
	connect(m_subscription, &Subscription::subscriptionUpdated, this, &TreeWidget::subscriptionUpdated);
	connect(m_subscription, &Subscription::subscriptionError, this, &TreeWidget::subscriptionError);
}

void TreeWidget::showRule(const Rule* rule)
{
	if (!m_topItem && rule)
		m_ruleToBeSelected = rule->filter();
	else if (!m_ruleToBeSelected.isEmpty()) {
		QList<QTreeWidgetItem*> items = findItems(m_ruleToBeSelected, Qt::MatchRecursive);

		if (!items.isEmpty()) {
			QTreeWidgetItem* item = items[0];

			setCurrentItem(item);
			scrollToItem(item, QAbstractItemView::PositionAtCenter);
		}

		m_ruleToBeSelected.clear();
	}
}

void TreeWidget::refresh()
{
	m_itemChangingBlock = true;
	clear();

	QFont boldFont{};
	boldFont.setBold(true);

	m_topItem = new QTreeWidgetItem(this);
	m_topItem->setText(0, m_subscription->title());
	m_topItem->setFont(0, boldFont);
	addTopLevelItem(m_topItem);

	const QVector<Rule*>& allRules = m_subscription->allRulles();
	int index = 0;

		foreach (const Rule* rule, allRules) {
			QTreeWidgetItem* item = new QTreeWidgetItem(m_topItem);
			item->setText(0, rule->filter());
			item->setData(0, Qt::UserRole + 10, index);

			if (m_subscription->canEditRules())
				item->setFlags(item->flags() | Qt::ItemIsEditable);

			adjustItemFeature(item, rule);
			++index;
		}

	showRule(nullptr);
	m_itemChangingBlock = false;
}

void TreeWidget::addRule()
{
	if (!m_subscription->canEditRules())
		return;

	QString newRule{QInputDialog::getText(this, tr("Add Custom Rule"), tr("Please write your rule here:"))};

	if (newRule.isEmpty())
		return;

	Rule* rule{new Rule(newRule, m_subscription)};
	int offset = m_subscription->addRule(rule);
	QTreeWidgetItem* item{new QTreeWidgetItem()};

	item->setText(0, newRule);
	item->setData(0, Qt::UserRole + 10, offset);
	item->setFlags(item->flags() | Qt::ItemIsEditable);

	m_itemChangingBlock = true;
	m_topItem->addChild(item);
	m_itemChangingBlock = false;

	adjustItemFeature(item, rule);
}

void TreeWidget::removeRule()
{
	QTreeWidgetItem* item{currentItem()};

	if (!item || !m_subscription->canEditRules() || item == m_topItem)
		return;

	int offset{item->data(0, Qt::UserRole + 10).toInt()};

	m_subscription->removeRule(offset);
	m_refreshAllItemsNeeded = true;
	delete item;
}

void TreeWidget::contextMenuRequested(const QPoint& pos)
{
	if (!m_subscription->canEditRules())
		return;

	QTreeWidgetItem* item = itemAt(pos);
	if (!item)
		return;

	QMenu menu{};
	menu.addAction(tr("Add Rule"), this, &TreeWidget::addRule);
	menu.addSeparator();

	QAction* deleteAction{menu.addAction(tr("Remove Rule"), this, &TreeWidget::removeRule)};

	if (!item->parent())
		deleteAction->setDisabled(true);

	menu.exec(viewport()->mapToGlobal(pos));
}

void TreeWidget::itemChanged(QTreeWidgetItem* item)
{
	if (!item || m_itemChangingBlock)
		return;

	m_itemChangingBlock = true;
	m_refreshAllItemsNeeded = true;

	int offset{item->data(0, Qt::UserRole + 10).toInt()};
	const Rule* oldRule{m_subscription->rule(offset)};

	// Disable the rule
	if (item->checkState(0) == Qt::Unchecked && oldRule->isEnabled()) {
		const Rule* rule = m_subscription->disableRule(offset);
		adjustItemFeature(item, rule);
	} // Enable the rule
	else if (item->checkState(0) == Qt::Checked && !oldRule->isEnabled()) {
		const Rule* rule{m_subscription->enableRule(offset)};
		adjustItemFeature(item, rule);
	} // Custom rule has been chanded
	else if (m_subscription->canEditRules()) {
		Rule* newRule{new Rule(item->text(0), m_subscription)};
		const Rule* rule{m_subscription->replaceRule(newRule, offset)};
		adjustItemFeature(item, rule);
	}

	m_itemChangingBlock = false;
}

void TreeWidget::copyFilter()
{
	QTreeWidgetItem* item{currentItem()};

	if (!item)
		return;

	QApplication::clipboard()->setText(item->text(0));
}

void TreeWidget::subscriptionUpdated()
{
	refresh();

	m_itemChangingBlock = true;
	m_topItem->setText(0, tr("%1 (recently updated").arg(m_subscription->title()));
	m_itemChangingBlock = false;
}

void TreeWidget::subscriptionError(const QString& message)
{
	refresh();

	m_itemChangingBlock = true;
	m_topItem->setText(0, tr("%1 (Error: %2)").arg(m_subscription->title(), message));
	m_itemChangingBlock = false;
}

void TreeWidget::adjustItemFeature(QTreeWidgetItem* item, const Rule* rule)
{
	if (!rule->isEnabled()) {
		QFont font{};

		font.setItalic(true);
		item->setForeground(0, QColor(Qt::gray));

		if (!rule->isComment()) {
			item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
			item->setCheckState(0, Qt::Unchecked);
			item->setFont(0, font);
		}

		return;
	}

	item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
	item->setCheckState(0, Qt::Checked);

	if (rule->isException()) {
		item->setForeground(0, QColor(Qt::darkGreen));
		item->setFont(0, QFont());
	}
	else if (rule->isCSSRule()) {
		item->setForeground(0, QColor(Qt::darkBlue));
		item->setFont(0, QFont());
	}
}

void TreeWidget::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_C && (event->modifiers() & Qt::ControlModifier))
		copyFilter();

	if (event->key() == Qt::Key_Delete)
		removeRule();

	QTreeWidget::keyPressEvent(event);
}
}
}