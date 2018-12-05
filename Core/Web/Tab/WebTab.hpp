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
#ifndef SIELOBROWSER_WEBTAB_HPP
#define SIELOBROWSER_WEBTAB_HPP

#include "SharedDefines.hpp"

#include <QVBoxLayout>
#include <QGridLayout>

#include <QWidget>
#include <QIcon>
#include <QPushButton>
#include <QSplitter>

#include <QMenu>
#include <QToolBar>

#include <QWebEngineHistory>

#include <QUrl>

#include <QShowEvent>

#include <QPointer>

#include "Web/LoadRequest.hpp"
#include "Web/WebInspector.hpp"

namespace Sn
{
class TabWidget;

class TabbedWebView;

class TabIcon;
class MainTabBar;

class FloatingButton;
class AddressBar;

class SIELO_SHAREDLIB WebTab: public QWidget {
	Q_OBJECT

public:
	struct SavedTab {
		QString title{};
		QUrl url{};
		QIcon icon{};
		QByteArray history{};
		bool isPinned{false};
		int zoomLevel{};
		int parentTab{-1};
		QVector<int> childTabs{};
		QHash<QString, QVariant> sessionData{};

		SavedTab();
		SavedTab(WebTab* webTab);

		bool isValide() const;
		void clear();

		friend QDataStream& operator<<(QDataStream& stream, const SavedTab& tab);
		friend QDataStream& operator>>(QDataStream& stream, SavedTab& tab);
	};

	enum AddChildBehavior {
		AppendChild = 0,
		PrependChild
	};


	WebTab(TabWidget* tabWidget);
	~WebTab();

	TabWidget* tabWidget() const { return m_tabWidget; }
	WebInspector* inspector() const { return m_inspector; }
	TabbedWebView* webView() const { return m_webView; }
	QWidget* application() const { return m_application; }
	TabIcon* tabIcon() const { return m_tabIcon; }
	AddressBar* addressBar() const { return m_addressBar; }

	void takeAddressBar();

	WebTab* parentTab() const { return m_parentTab; }
	void setParentTab(WebTab* tab);
	QVector<WebTab*> childTabs() const { return m_childTabs; }
	void addChildTab(WebTab* tab, int index = -1);

	QHash<QString, QVariant> sessionData() const { return m_sessionData; }
	void setSessionData(const QString& key, const QVariant& value);

	QUrl url() const;
	QString title() const;
	QIcon icon(bool allowNull = false) const;
	QWebEngineHistory* history() const;
	int zoomLevel() const;
	void setZoomLevel(int level);

	void detach();
	void attach(TabWidget* tabWidget);

	void addToolBar(QToolBar* toolBar);
	void removeToolBar(QToolBar* toolBar);

	QByteArray historyData() const;
	void setHistoryData(const QByteArray& data);

	void stop();
	void reload();
	void load(const LoadRequest& request);
	void loadApplication(QWidget* application);
	void unload();
	bool isLoading() const;

	bool isPinned() const;
	void setPinned(bool state);
	void togglePinned();

	bool isMuted() const;
	bool isPlaying() const;
	void setMuted(bool muted);
	void toggleMuted();

	bool backgroundActivity() const;

	int tabIndex() const;

	bool isCurrentTab() const { return m_isCurrentTab; }
	void makeCurrentTab() const;
	void closeTab() const;
	void moveTab(int to) const;

	bool haveInspector() const;
	void showWebInspector(bool inspectElement = false);
	void toggleWebInspector();

	void showSearchToolBar();

	bool isRestored() const;
	void restoreTab(const SavedTab& tab);
	void p_restoreTab(const SavedTab& tab);
	void p_restoreTab(const QUrl& url, const QByteArray& history, int zoomLevel);

	void tabActivated();

	static AddChildBehavior addChildBehavior();
	static void setAddChildBehavior(AddChildBehavior behavior);

signals:
	void titleChanged(const QString& title);
	void iconChanged(const QIcon& icon);
	void pinnedChanged(bool pinned);
	void restoredChanged(bool restored);
	void currentTabChanged(bool current);
	void loadingChanged(bool loading);
	void mutedChanged(bool muted);
	void playingChanged(bool playing);
	void backgroundActivityChanged(bool activity);
	void parentTabChanged(WebTab* tab);
	void childTabAdded(WebTab* tab, int index);
	void childTabRemoved(WebTab* tab, int index);

public slots:
	void sNewWindow();
	void sNewTab();
	void sGoHome();

private slots:
	void showNotification(QWidget* notif);
	void loadStarted();
	void loadFinished();

private:
	void titleWasChanged(const QString& title);
	void resizeEvent(QResizeEvent* event) override;
	void removeFromTabTree();

	QVBoxLayout* m_layout{nullptr};
	QSplitter* m_splitter{nullptr};

	QMenu* m_menuForward{nullptr};

	TabWidget* m_tabWidget{nullptr};
	QPointer<WebInspector> m_inspector{};
	TabbedWebView* m_webView{nullptr};
	QPointer<QWidget> m_application{};
	TabIcon* m_tabIcon{nullptr};
	QWidget* m_notificationWidget{nullptr};

	AddressBar* m_addressBar{nullptr};

	WebTab* m_parentTab{nullptr};
	QVector<WebTab*> m_childTabs{};
	QHash<QString, QVariant> m_sessionData{};

	SavedTab m_savedTab{};
	bool m_isPinned{false};
	bool m_isCurrentTab{false};
};
}
#endif //SIELOBROWSER_WEBTAB_HPP

