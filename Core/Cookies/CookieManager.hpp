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
#ifndef SIELO_BROWSER_COOKIEMANAGER_HPP
#define SIELO_BROWSER_COOKIEMANAGER_HPP

#include "SharedDefines.hpp"

#include <QWidget>
#include <QDialog>

#include <QNetworkCookie>

#include <QGridLayout>
#include <QHBoxLayout>

#include <QTabWidget>
#include <QFrame>
#include <QTreeWidget>
#include <QListWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QLineEdit>

#include <QCloseEvent>
#include <QKeyEvent>

#include <QHash>

namespace Sn {
class EllipseLabel;

class SIELO_SHAREDLIB CookieManager: public QDialog {
Q_OBJECT

public:
	CookieManager();
	~CookieManager();

private slots:
	void currentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* parent);
	void remove();
	void removeAll();

	void addWhiteList();
	void removeWhiteList();
	void addBlackList();
	void removeBlackList();

	void deletePressed();

	void filterString(const QString& string);

	void addCookie(const QNetworkCookie& cookie);
	void removeCookie(const QNetworkCookie& cookie);

private:
	void setupUI();

	void closeEvent(QCloseEvent* event);
	void keyPressEvent(QKeyEvent* event);

	void addBlackList(const QString& server);
	QString cookieDomain(const QNetworkCookie& cookie) const;
	QTreeWidgetItem* cookieItem(const QNetworkCookie& cookie) const;

	QHBoxLayout* m_layout{nullptr};

	QGridLayout* m_storedCookieLayout{nullptr};
	QGridLayout* m_cookieDescLayout{nullptr};
	QHBoxLayout* m_storedCookieBtnLayout{nullptr};

	QGridLayout* m_cookieFilteringLayout{nullptr};
	QVBoxLayout* m_WhiteListBtnLayout{nullptr};
	QVBoxLayout* m_blackListBtnLayout{nullptr};

	QTabWidget* m_pages{nullptr};

	/*** STORED COOKIE TAB ***/
	QWidget* m_storedCookiePage{nullptr};
	QLabel* m_labelFind{nullptr};
	QLineEdit* m_storedCookieSearch{nullptr};
	QLabel* m_labelCookieStoredDesc{nullptr};
	QTreeWidget* m_cookieTree{nullptr};

	QFrame* m_storedCookieFrame{nullptr};
	QLabel* m_descName{nullptr};
	QLabel* m_descValue{nullptr};
	QLabel* m_descServer{nullptr};
	QLabel* m_descPath{nullptr};
	QLabel* m_descSecure{nullptr};
	QLabel* m_descExpiration{nullptr};
	EllipseLabel* m_name{nullptr};
	EllipseLabel* m_value{nullptr};
	EllipseLabel* m_server{nullptr};
	QLabel* m_path{nullptr};
	EllipseLabel* m_secure{nullptr};
	EllipseLabel* m_expiration{nullptr};

	QFrame* m_buttonFrame{nullptr};
	QPushButton* m_removeAllCookies{nullptr};
	QPushButton* m_removeCookie{nullptr};
	QDialogButtonBox* m_storedCloseDialogButtonBox{nullptr};

	/*** COOKIE FILTERING TAB ***/
	QWidget* m_cookieFilteringPage{nullptr};
	QLabel* m_descCookieWhiteList{nullptr};
	QLabel* m_labelCookieWhiteList{nullptr};
	QSpacerItem* m_whiteListSpacer{nullptr};
	QPushButton* m_whiteListAdd{nullptr};
	QPushButton* m_whiteListRemove{nullptr};
	QListWidget* m_whiteList{nullptr};

	QLabel* m_descCookieBlackList{nullptr};
	QLabel* m_labelCookieBlackList{nullptr};
	QListWidget* m_blackList{nullptr};
	QSpacerItem* m_blackListSpacer{nullptr};
	QPushButton* m_blackListAdd{nullptr};
	QPushButton* m_blackListRemove{nullptr};
	QDialogButtonBox* m_filteringCloseDialogButtonBox{nullptr};

	/*** COOKIE SETTINGS TAB ***/
	QWidget* m_settingsPage{nullptr};
	QGridLayout* m_settingsLayout{nullptr};
	QSpacerItem* m_settingsHSpacer{nullptr};
	QCheckBox* m_filter3rdParty{nullptr};
	QLabel* m_descSettings{nullptr};
	QCheckBox* m_saveCookies{nullptr};
	QCheckBox* m_filterTracking{nullptr};
	QCheckBox* m_deleteCookiesOnClose{nullptr};
	QDialogButtonBox* m_settingsCloseButtonBox{nullptr};
	QSpacerItem* m_settingsVSpacer{nullptr};

	QHash<QString, QTreeWidgetItem*> m_domainHash{};
	QHash<QTreeWidgetItem*, QNetworkCookie> m_itemHash{};
};

}

#endif //SIELO_BROWSER_COOKIEMANAGER_HPP
