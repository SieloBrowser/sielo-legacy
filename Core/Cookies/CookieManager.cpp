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

#include "CookieManager.hpp"

#include <QShortcut>

#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>

#include "Application.hpp"

#include "Utils/Settings.hpp"

#include "Cookies/CookieJar.hpp"

#include "Widgets/EllipseLabel.hpp"

namespace Sn {

CookieManager::CookieManager() :
	QDialog(nullptr)
{
	setAttribute(Qt::WA_DeleteOnClose);

	QIcon icon = windowIcon();
	Qt::WindowFlags flags = windowFlags();
	Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;

	flags = flags & (~helpFlag);
	setWindowFlags(flags);
	setWindowIcon(icon);

	setupUI();

	// Stored cookie
	connect(m_cookieTree, &QTreeWidget::currentItemChanged, this, &CookieManager::currentItemChanged);
	connect(m_removeAllCookies, &QPushButton::clicked, this, &CookieManager::removeAll);
	connect(m_removeCookie, &QPushButton::clicked, this, &CookieManager::remove);
	connect(m_storedCloseDialogButtonBox, &QDialogButtonBox::clicked, this, &CookieManager::close);
	connect(m_filteringCloseDialogButtonBox, &QDialogButtonBox::clicked, this, &CookieManager::close);
	connect(m_settingsCloseButtonBox, &QDialogButtonBox::clicked, this, &CookieManager::close);
	connect(m_storedCookieSearch, &QLineEdit::textChanged, this, &CookieManager::filterString);

	// Cookie filtering
	connect(m_whiteListAdd, &QPushButton::clicked, this, &CookieManager::addWhiteList);
	connect(m_whiteListRemove, &QPushButton::clicked, this, &CookieManager::removeWhiteList);
	connect(m_blackListAdd, SIGNAL(clicked()), this, SLOT(addBlackList()));
	connect(m_blackListRemove, &QPushButton::clicked, this, &CookieManager::removeBlackList);

	// Cookie settings
	Settings settings{};

	settings.beginGroup("Cookie-Settings");

	m_saveCookies->setChecked(settings.value("allowCookies", true).toBool());
	m_filter3rdParty->setChecked(settings.value("filterThirdPartyCookies", false).toBool());
	m_filterTracking->setChecked(settings.value("filterTrackingCookies", false).toBool());
	m_deleteCookiesOnClose->setChecked(settings.value("deleteCookiesOnClose", false).toBool());
	m_whiteList->addItems(settings.value("whiteList", QStringList()).toStringList());
	m_blackList->addItems(settings.value("blackList", QStringList()).toStringList());

	m_cookieTree->sortItems(0, Qt::AscendingOrder);
	m_cookieTree->setFocus();

	QShortcut* removeShortcut{new QShortcut(QKeySequence("Del"), this)};
	connect(removeShortcut, &QShortcut::activated, this, &CookieManager::deletePressed);

	connect(Application::instance()->cookieJar(), &CookieJar::cookieAdded, this, &CookieManager::addCookie);
	connect(Application::instance()->cookieJar(), &CookieJar::cookieRemoved, this, &CookieManager::removeCookie);

		foreach (const QNetworkCookie& cookie, Application::instance()->cookieJar()->getAllCookies()) addCookie(cookie);
}

CookieManager::~CookieManager()
{
	// Empty
}

void CookieManager::currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* parent)
{
	Q_UNUSED(parent);

	if (!current)
		return;

	if (current->text(1).isEmpty()) {
		m_name->setText(tr("<cookie not selected>"));
		m_value->setText(tr("<cookie not selected>"));
		m_server->setText(tr("<cookie not selected>"));
		m_path->setText(tr("<cookie not selected>"));
		m_secure->setText(tr("<cookie not selected>"));
		m_expiration->setText(tr("<cookie not selected>"));

		m_removeCookie->setText(tr("Remove cookies"));
		return;
	}

	const QNetworkCookie cookie = qvariant_cast<QNetworkCookie>(current->data(0, Qt::UserRole + 10));

	m_name->setText(cookie.name());
	m_value->setText(cookie.value());
	m_server->setText(cookie.domain());
	m_path->setText(cookie.path());

	cookie.isSecure() ? m_secure->setText(tr("Secure only")) : m_secure->setText(tr("All connections"));
	cookie.isSessionCookie() ? m_expiration->setText(tr("Session cookie")) : m_expiration
		->setText(QDateTime(cookie.expirationDate()).toString("hh:mm::ss dddd d. MMMM yyyy"));

	m_removeCookie->setText(tr("Remove cookie"));
}

void CookieManager::remove()
{
	QTreeWidgetItem* current{m_cookieTree->currentItem()};

	if (!current)
		return;

	QList<QNetworkCookie> cookies{};

	if (current->childCount()) {
		for (int i{0}; i < current->childCount(); ++i) {
			QTreeWidgetItem* item{current->child(i)};

			if (item && m_itemHash.contains(item))
				cookies.append(m_itemHash.value(item));
		}
	}
	else if (m_itemHash.contains(current))
		cookies.append(m_itemHash.value(current));

		foreach (const QNetworkCookie& cookie, cookies) Application::instance()->cookieJar()->deleteCookie(cookie);
}

void CookieManager::addWhiteList()
{
	const QString server{QInputDialog::getText(this, tr("Add to whitelist"), tr("Server:"))};

	if (server.isEmpty())
		return;

	if (!m_blackList->findItems(server, Qt::MatchFixedString).isEmpty()) {
		QMessageBox::information(this,
								 tr("Already blacklisted!"),
								 tr("The server \"%1\" is already in blacklist, please remove it first.").arg(server));
		return;
	}

	if (m_whiteList->findItems(server, Qt::MatchFixedString).isEmpty())
		m_whiteList->addItem(server);
}

void CookieManager::removeWhiteList()
{
	delete m_whiteList->currentItem();
}

void CookieManager::addBlackList()
{
	const QString server{QInputDialog::getText(this, tr("Add to blacklist"), tr("Server:"))};
	addBlackList(server);
}

void CookieManager::removeAll()
{
	QMessageBox::StandardButton button = QMessageBox::warning(this,
															  tr("Confirmation"),
															  tr("Are you sure you want to delete all cookies on your computer?"),
															  QMessageBox::Yes | QMessageBox::No);

	if (button != QMessageBox::Yes)
		return;

	Application::instance()->cookieJar()->deleteAllCookies();

	m_itemHash.clear();
	m_domainHash.clear();
	m_cookieTree->clear();
}

void CookieManager::removeBlackList()
{
	delete m_blackList->currentItem();
}

void CookieManager::deletePressed()
{
	if (m_cookieTree->hasFocus())
		remove();
	else if (m_whiteList->hasFocus())
		removeWhiteList();
	else if (m_blackList->hasFocus())
		removeBlackList();
}

void CookieManager::filterString(const QString& string)
{
	if (string.isEmpty()) {
		for (int i{0}; i < m_cookieTree->topLevelItemCount(); ++i) {
			m_cookieTree->topLevelItem(i)->setHidden(false);
			m_cookieTree->topLevelItem(i)->setExpanded(false);
		}
	}
	else {
		for (int i{0}; i < m_cookieTree->topLevelItemCount(); ++i) {
			QString text{"." + m_cookieTree->topLevelItem(i)->text(0)};
			m_cookieTree->topLevelItem(i)->setHidden(!text.contains(string, Qt::CaseInsensitive));
			m_cookieTree->topLevelItem(i)->setExpanded(true);
		}
	}
}

void CookieManager::addCookie(const QNetworkCookie& cookie)
{
	QTreeWidgetItem* item{nullptr};
	const QString domain{cookieDomain(cookie)};
	QTreeWidgetItem* findParent{m_domainHash.value(domain)};

	if (findParent)
		item = new QTreeWidgetItem(findParent);
	else {
		QTreeWidgetItem* newParent{new QTreeWidgetItem(m_cookieTree)};
		newParent->setText(0, domain);
		newParent->setIcon(0, QApplication::style()->standardIcon(QStyle::SP_DirIcon));
		newParent->setData(0, Qt::UserRole + 10, cookie.domain());

		m_cookieTree->addTopLevelItem(newParent);
		m_domainHash[domain] = newParent;

		item = new QTreeWidgetItem(newParent);
	}

	item->setText(0, "." + domain);
	item->setText(1, cookie.name());
	item->setData(0, Qt::UserRole + 10, QVariant::fromValue(cookie));

	m_cookieTree->addTopLevelItem(item);
	m_itemHash[item] = cookie;
}

void CookieManager::removeCookie(const QNetworkCookie& cookie)
{
	QTreeWidgetItem* item{cookieItem(cookie)};

	if (!item)
		return;

	m_itemHash.remove(item);

	if (item->parent() && item->parent()->childCount() == 1) {
		m_domainHash.remove(cookieDomain(cookie));
		delete item->parent();
		item = nullptr;
	}

	delete item;
}

void CookieManager::closeEvent(QCloseEvent* event)
{
	QStringList whiteList{};
	QStringList blackList{};

	for (int i{0}; i < m_whiteList->count(); ++i)
		whiteList.append(m_whiteList->item(i)->text());

	for (int i{0}; i < m_blackList->count(); ++i)
		blackList.append(m_blackList->item(i)->text());

	Settings settings;

	settings.beginGroup("Cookie-Settings");

	settings.setValue("allowCookies", m_saveCookies->isChecked());
	settings.setValue("filterThirdPartyCookies", m_filter3rdParty->isChecked());
	settings.setValue("filterTrackingCookies", m_filterTracking->isChecked());
	settings.setValue("deleteCookiesOnClose", m_deleteCookiesOnClose->isChecked());
	settings.setValue("whiteList", whiteList);
	settings.setValue("blackList", blackList);

	settings.endGroup();

	Application::instance()->cookieJar()->loadSettings();

	event->accept();
}

void CookieManager::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Escape)
		close();

	QWidget::keyPressEvent(event);
}

void CookieManager::setupUI()
{
	resize(617, 415);
	setWindowTitle(tr("Cookies"));

	m_layout = new QHBoxLayout(this);
	m_layout->setContentsMargins(4, 4, 4, 4);

	m_pages = new QTabWidget(this);

	m_storedCookiePage = new QWidget(this);
	m_cookieFilteringPage = new QWidget(this);
	m_settingsPage = new QWidget(this);

	/*** STORED COOKIE TAB ***/
	m_storedCookieFrame = new QFrame(m_storedCookiePage);
	m_storedCookieFrame->setFrameShape(QFrame::StyledPanel);
	m_storedCookieFrame->setFrameShadow(QFrame::Raised);

	m_buttonFrame = new QFrame(m_storedCookiePage);
	m_buttonFrame->setFrameShape(QFrame::NoFrame);
	m_buttonFrame->setFrameShadow(QFrame::Raised);

	m_storedCookieLayout = new QGridLayout(m_storedCookiePage);
	m_cookieDescLayout = new QGridLayout(m_storedCookieFrame);
	m_storedCookieBtnLayout = new QHBoxLayout(m_buttonFrame);

	m_cookieDescLayout->setHorizontalSpacing(6);
	m_cookieDescLayout->setVerticalSpacing(3);
	m_cookieDescLayout->setContentsMargins(-1, 0, 0, 0);

	m_storedCookieBtnLayout->setObjectName(QStringLiteral("m_storedCookieBtnLayout"));
	m_storedCookieBtnLayout->setContentsMargins(0, 0, 0, 0);

	m_labelFind = new QLabel(m_storedCookiePage);
	m_labelFind->setText(tr("Find:"));

	m_storedCookieSearch = new QLineEdit(m_storedCookiePage);
	m_storedCookieSearch->setPlaceholderText(tr("Search"));

	m_labelCookieStoredDesc = new QLabel(m_storedCookiePage);
	m_labelCookieStoredDesc->setText(tr("These cookies are stored on your computer:"));

	m_cookieTree = new QTreeWidget(m_storedCookiePage);
	m_cookieTree->setSortingEnabled(true);
	m_cookieTree->header()->setDefaultSectionSize(220);

	QTreeWidgetItem* cookieTreeHeaders = m_cookieTree->headerItem();
	cookieTreeHeaders->setText(0, tr("Server"));
	cookieTreeHeaders->setText(1, tr("Cookie name"));

	m_descName = new QLabel(m_storedCookieFrame);
	m_descName->setText(tr("Name:"));
	QSizePolicy descNameSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
	descNameSizePolicy.setHorizontalStretch(0);
	descNameSizePolicy.setVerticalStretch(0);
	descNameSizePolicy.setHeightForWidth(m_descName->sizePolicy().hasHeightForWidth());
	m_descName->setSizePolicy(descNameSizePolicy);
	//m_descName->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);

	m_descValue = new QLabel(m_storedCookieFrame);
	m_descValue->setText(tr("Value:"));

	m_descServer = new QLabel(m_storedCookieFrame);
	m_descServer->setText(tr("Server:"));

	m_descPath = new QLabel(m_storedCookieFrame);
	m_descPath->setText(tr("Path:"));

	m_descSecure = new QLabel(m_storedCookieFrame);
	m_descSecure->setText(tr("Secure:"));

	m_descExpiration = new QLabel(m_storedCookieFrame);
	m_descExpiration->setText(tr("Expiration:"));

	m_name = new EllipseLabel(m_storedCookieFrame);
	m_name->setText(tr("<cookie not selected>"));
	//m_name->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_value = new EllipseLabel(m_storedCookieFrame);
	m_value->setText(tr("<cookie not selected>"));
	//m_value->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_server = new EllipseLabel(m_storedCookieFrame);
	m_server->setText(tr("<cookie not selected>"));
	//m_server->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_path = new QLabel(m_storedCookieFrame);
	m_path->setText(tr("<cookie not selected>"));
	//m_path->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_secure = new EllipseLabel(m_storedCookieFrame);
	m_secure->setText(tr("<cookie not selected>"));
	//m_secure->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_expiration = new EllipseLabel(m_storedCookieFrame);
	m_expiration->setText(tr("<cookie not selected>"));
	//m_expiration->setTextInteractionFlags(Qt::LinksAccessibleByMouse | Qt::TextSelectableByMouse);

	m_removeAllCookies = new QPushButton(m_buttonFrame);
	m_removeAllCookies->setText(tr("Remove all cookies"));

	m_removeCookie = new QPushButton(m_buttonFrame);
	m_removeCookie->setText(tr("Remove cookies"));

	m_storedCloseDialogButtonBox = new QDialogButtonBox(m_buttonFrame);
	QSizePolicy storedCloseButtonBoxSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	storedCloseButtonBoxSizePolicy.setHorizontalStretch(0);
	storedCloseButtonBoxSizePolicy.setVerticalStretch(0);
	storedCloseButtonBoxSizePolicy.setHeightForWidth(m_storedCloseDialogButtonBox->sizePolicy().hasHeightForWidth());
	m_storedCloseDialogButtonBox->setSizePolicy(storedCloseButtonBoxSizePolicy);
	m_storedCloseDialogButtonBox->setStandardButtons(QDialogButtonBox::Close);

	m_cookieDescLayout->addWidget(m_descName, 0, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_descValue, 1, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_descServer, 2, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_descPath, 3, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_descSecure, 4, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_descExpiration, 5, 0, 1, 1);
	m_cookieDescLayout->addWidget(m_name, 0, 1, 1, 1);
	m_cookieDescLayout->addWidget(m_value, 1, 1, 1, 1);
	m_cookieDescLayout->addWidget(m_server, 2, 1, 1, 1);
	m_cookieDescLayout->addWidget(m_path, 3, 1, 1, 1);
	m_cookieDescLayout->addWidget(m_secure, 4, 1, 1, 1);
	m_cookieDescLayout->addWidget(m_expiration, 5, 1, 1, 1);

	m_storedCookieBtnLayout->addWidget(m_removeAllCookies);
	m_storedCookieBtnLayout->addWidget(m_removeCookie);
	m_storedCookieBtnLayout->addWidget(m_storedCloseDialogButtonBox);

	m_storedCookieLayout->addWidget(m_labelFind, 0, 0, 1, 1);
	m_storedCookieLayout->addWidget(m_storedCookieSearch, 0, 1, 1, 1);
	m_storedCookieLayout->addWidget(m_labelCookieStoredDesc, 1, 0, 1, 2);
	m_storedCookieLayout->addWidget(m_cookieTree, 2, 0, 1, 2);
	m_storedCookieLayout->addWidget(m_storedCookieFrame, 3, 0, 1, 2);
	m_storedCookieLayout->addWidget(m_buttonFrame, 4, 0, 1, 2);

	/*** COOKIE FILTERING PAGE ***/
	m_cookieFilteringLayout = new QGridLayout(m_cookieFilteringPage);
	m_WhiteListBtnLayout = new QVBoxLayout();
	m_blackListBtnLayout = new QVBoxLayout();

	m_descCookieWhiteList = new QLabel(m_cookieFilteringPage);
	m_descCookieWhiteList->setText(tr("<b>Cookie whitelist</b>"));

	m_labelCookieWhiteList = new QLabel(m_cookieFilteringPage);
	m_labelCookieWhiteList
		->setText(tr("Cookies from these servers will ALWAYS be accepted (even if you have disabled saving cookies)"));
	m_labelCookieWhiteList->setWordWrap(true);

	m_whiteListSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_whiteListAdd = new QPushButton(m_cookieFilteringPage);
	m_whiteListAdd->setText(tr("Add"));

	m_whiteListRemove = new QPushButton(m_cookieFilteringPage);
	m_whiteListRemove->setText(tr("Remove"));

	m_whiteList = new QListWidget(m_cookieFilteringPage);

	m_descCookieBlackList = new QLabel(m_cookieFilteringPage);
	m_descCookieBlackList->setText(tr("<b>Cookie blacklist</b>"));

	m_labelCookieBlackList = new QLabel(m_cookieFilteringPage);
	m_labelCookieBlackList->setText("Cookies from these servers will NEVER be accepted");
	m_labelCookieBlackList->setWordWrap(true);

	m_blackListBtnLayout->setObjectName(QStringLiteral("m_blackListBtnLayout"));
	m_blackListSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_blackListAdd = new QPushButton(m_cookieFilteringPage);
	m_blackListAdd->setText(tr("Add"));

	m_blackListRemove = new QPushButton(m_cookieFilteringPage);
	m_blackListRemove->setText(tr("Remove"));

	m_blackList = new QListWidget(m_cookieFilteringPage);

	m_filteringCloseDialogButtonBox = new QDialogButtonBox(m_cookieFilteringPage);
	m_filteringCloseDialogButtonBox->setStandardButtons(QDialogButtonBox::Close);

	m_WhiteListBtnLayout->addItem(m_whiteListSpacer);
	m_WhiteListBtnLayout->addWidget(m_whiteListAdd);
	m_WhiteListBtnLayout->addWidget(m_whiteListRemove);

	m_blackListBtnLayout->addItem(m_blackListSpacer);
	m_blackListBtnLayout->addWidget(m_blackListAdd);
	m_blackListBtnLayout->addWidget(m_blackListRemove);

	m_cookieFilteringLayout->addWidget(m_descCookieWhiteList, 0, 0, 1, 1);
	m_cookieFilteringLayout->addWidget(m_labelCookieWhiteList, 1, 0, 1, 2);
	m_cookieFilteringLayout->addLayout(m_WhiteListBtnLayout, 2, 1, 1, 1);
	m_cookieFilteringLayout->addWidget(m_whiteList, 2, 0, 1, 1);
	m_cookieFilteringLayout->addWidget(m_descCookieBlackList, 3, 0, 1, 1);
	m_cookieFilteringLayout->addWidget(m_labelCookieBlackList, 4, 0, 1, 2);
	m_cookieFilteringLayout->addLayout(m_blackListBtnLayout, 5, 1, 1, 1);
	m_cookieFilteringLayout->addWidget(m_blackList, 5, 0, 1, 1);
	m_cookieFilteringLayout->addWidget(m_filteringCloseDialogButtonBox, 6, 0, 1, 2);

	/*** COOKIE SETTINGS TAB ***/
	m_settingsLayout = new QGridLayout(m_settingsPage);

	m_settingsHSpacer = new QSpacerItem(10, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

	m_descSettings = new QLabel(m_settingsPage);
	m_descSettings->setText(tr("<b>Cookie Settings</b>"));

	m_saveCookies = new QCheckBox(m_settingsPage);
	m_saveCookies->setText(tr("Allow storing of cookies"));

	m_filterTracking = new QCheckBox(m_settingsPage);
	m_filterTracking->setText(tr("Filter tracking cookies"));

	m_filter3rdParty = new QCheckBox(m_settingsPage);
	m_filter3rdParty->setText(tr("Filter 3rd party cookies"));

	m_deleteCookiesOnClose = new QCheckBox(m_settingsPage);
	m_deleteCookiesOnClose->setText(tr("Delete cookies on close"));

	m_settingsVSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

	m_settingsCloseButtonBox = new QDialogButtonBox(m_settingsPage);
	m_settingsCloseButtonBox->setStandardButtons(QDialogButtonBox::Close);

	m_settingsLayout->addWidget(m_descSettings, 0, 0, 1, 3);
	m_settingsLayout->addItem(m_settingsHSpacer, 1, 0, 1, 1);
	m_settingsLayout->addWidget(m_saveCookies, 1, 1, 1, 2);
	m_settingsLayout->addWidget(m_filterTracking, 2, 1, 1, 2);
	m_settingsLayout->addWidget(m_filter3rdParty, 3, 1, 1, 1);
	m_settingsLayout->addWidget(m_deleteCookiesOnClose, 4, 1, 1, 2);
	m_settingsLayout->addItem(m_settingsVSpacer, 5, 3, 1, 1);
	m_settingsLayout->addWidget(m_settingsCloseButtonBox, 6, 3, 1, 1);

	m_pages->addTab(m_storedCookiePage, tr("Stored Cookie"));
	m_pages->addTab(m_cookieFilteringPage, tr("Cookie Filtering"));
	m_pages->addTab(m_settingsPage, "Settings");

	m_layout->addWidget(m_pages);

	if (isRightToLeft()) {
		m_cookieTree->headerItem()->setTextAlignment(0, Qt::AlignRight | Qt::AlignVCenter);
		m_cookieTree->headerItem()->setTextAlignment(1, Qt::AlignRight | Qt::AlignVCenter);
		m_cookieTree->setLayoutDirection(Qt::LeftToRight);
		m_whiteList->setLayoutDirection(Qt::LeftToRight);
		m_blackList->setLayoutDirection(Qt::LeftToRight);
	}
}

void CookieManager::addBlackList(const QString& server)
{
	if (server.isEmpty())
		return;

	if (!m_whiteList->findItems(server, Qt::MatchFixedString).isEmpty()) {
		QMessageBox::information(this,
								 tr("Already whitelisted!"),
								 tr("The server \"%1\" is already in whitelist, please remove it first.").arg(server));
		return;
	}

	if (m_blackList->findItems(server, Qt::MatchFixedString).isEmpty())
		m_blackList->addItem(server);
}

QString CookieManager::cookieDomain(const QNetworkCookie& cookie) const
{
	QString domain{cookie.domain()};

	if (domain.startsWith(QLatin1Char('.')))
		domain = domain.mid(1);

	return domain;
}

QTreeWidgetItem* CookieManager::cookieItem(const QNetworkCookie& cookie) const
{
	QHashIterator<QTreeWidgetItem*, QNetworkCookie> it(m_itemHash);

	while (it.hasNext()) {
		it.next();

		if (it.value() == cookie)
			return it.key();
	}

	return nullptr;
}

}