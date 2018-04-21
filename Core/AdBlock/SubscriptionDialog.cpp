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

#include "SubscriptionDialog.hpp"

namespace Sn {
namespace ADB {

SubscriptionDialog::SubscriptionDialog(QWidget* parent) :
	QDialog(parent)
{
	setWindowTitle(tr("Add Subscription"));

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	m_knownSubscriptions
		<< ADBSubscription("EasyList (English)", "https://easylist-downloads.adblockplus.org/easylist.txt")
		<< ADBSubscription("BSI Lista Polska (Polish)", "http://www.bsi.info.pl/filtrABP.txt")
		<< ADBSubscription("Czech List (Czech)", "http://adblock.dajbych.net/adblock.txt")
		<< ADBSubscription("dutchblock (Dutch)", "http://groenewoudt.net/dutchblock/list.txt")
		<< ADBSubscription("Filtros Nauscopicos (Spanish)", "http://abp.mozilla-hispano.org/nauscopio/filtros.txt")
		<< ADBSubscription("IsraelList (Hebrew)", "http://secure.fanboy.co.nz/israelilist/IsraelList.txt")
		<< ADBSubscription("NLBlock (Dutch)", "http://www.verzijlbergh.com/adblock/nlblock.txt")
		<< ADBSubscription("Peter Lowe's list (English)",
						   "http://pgl.yoyo.org/adservers/serverlist.php?hostformat=adblockplus&mimetype=plaintext")
		<< ADBSubscription("PLgeneral (Polish)", "http://www.niecko.pl/adblock/adblock.txt")
		<< ADBSubscription("Schacks Adblock Plus liste (Danish)", "http://adblock.schack.dk/block.txt")
		<< ADBSubscription("Xfiles (Italian)", "http://mozilla.gfsolone.com/filtri.txt")
		<< ADBSubscription("EasyPrivacy (English)", "http://easylist-downloads.adblockplus.org/easyprivacy.txt")
		<< ADBSubscription("RU Adlist (Russian)", "https://easylist-downloads.adblockplus.org/advblock.txt")
		<< ADBSubscription("ABPindo (Indonesian)",
						   "https://raw.githubusercontent.com/heradhis/indonesianadblockrules/master/ADBSubscriptions/abpindo.txt")
		<< ADBSubscription("Easylist China (Chinese)", "https://easylist-downloads.adblockplus.org/easylistchina.txt")
		<< ADBSubscription("Anti-Adblock Killer",
						   "https://raw.githubusercontent.com/reek/anti-adblock-killer/master/anti-adblock-killer-filters.txt")
		<< ADBSubscription(tr("Other..."), QString());

		foreach (const ADBSubscription& subscription, m_knownSubscriptions) m_knownSubscriptionBox
				->addItem(subscription.title);

	indexChanged(0);

}

QString SubscriptionDialog::title() const
{
	return m_title->text();
}

QString SubscriptionDialog::address() const
{
	return m_address->text();
}

void SubscriptionDialog::indexChanged(int index)
{
	const ADBSubscription subscription{m_knownSubscriptions[index]};

	if (subscription.url.isEmpty()) {
		m_title->clear();
		m_address->clear();
	}
	else {
		int position{subscription.title.indexOf(QLatin1Char('('))};
		QString title{subscription.title};

		if (position > 0)
			title = title.left(position).trimmed();

		m_title->setText(title);
		m_title->setCursorPosition(0);

		m_address->setText(subscription.url);
		m_address->setCursorPosition(0);
	}
}

void SubscriptionDialog::setupUI()
{
	m_layout = new QFormLayout(this);

	m_desc = new QLabel(tr("Add new subscription to AdBlock:"), this);
	m_knownSubscriptionBox = new QComboBox(this);
	m_titleDesc = new QLabel(tr("Title:"), this);
	m_title = new QLineEdit(this);
	m_addressDesc = new QLabel(tr("Address:"), this);
	m_address = new QLineEdit(this);
	m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Ok, Qt::Horizontal, this);

	m_layout->setWidget(0, QFormLayout::SpanningRole, m_desc);
	m_layout->setWidget(1, QFormLayout::SpanningRole, m_knownSubscriptionBox);
	m_layout->setWidget(2, QFormLayout::LabelRole, m_titleDesc);
	m_layout->setWidget(2, QFormLayout::FieldRole, m_title);
	m_layout->setWidget(3, QFormLayout::LabelRole, m_addressDesc);
	m_layout->setWidget(3, QFormLayout::FieldRole, m_address);
	m_layout->setWidget(4, QFormLayout::FieldRole, m_buttonBox);

	connect(m_knownSubscriptionBox, SIGNAL(currentIndexChanged(int)), this, SLOT(indexChanged(int)));
	connect(m_buttonBox, &QDialogButtonBox::accepted, this, &SubscriptionDialog::accept);
	connect(m_buttonBox, &QDialogButtonBox::rejected, this, &SubscriptionDialog::reject);
}

}
}