#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVector>
#include <QSettings>
#include <QLineEdit>
#include <QSharedPointer>
#include <QWebEngineDownloadItem>

#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/SToolBar.hpp"
#include "includes/SWidgets/SMenu.hpp"
#include "includes/SWidgets/SUrlArea.hpp"
#include "includes/SWidgets/SSearchArea.hpp"

#include "includes/SWindows/SDownload.hpp"
#include "includes/SWindows/SHistory.hpp"

struct SActions;

class SMainWindow : public QMainWindow
{
public:
	// Constructor and destructor
	SMainWindow(QWidget* parent = nullptr, SWebView *view = nullptr, bool isPrivateBrowsing = false);
	~SMainWindow();

	// Loading functions
	void loadMenus();
	bool loadToolBar(const QString& filePath);

	// Function to change tab title and update url area
	void changeTabTitle(const QString& newTitle);
	void changeTabUrl(const QUrl& newUrl);

    // Save states
    void saveTabs();
    void saveWinState();

    void setTabs(STabWidget *tabs) { m_tabs = tabs; }

	// Getters
	QSharedPointer<SActions> getActions() const { return m_actions; }
	SUrlArea* getUrlArea() const { return m_urlArea; }
	SSearchArea* getSearchArea() const { return m_searchArea; }
    QSplitter* getSplitter() { return m_splitter; }
	STabWidget* getTabs() { return m_tabs; }
	SWebView* currentPage();
	QVector<SMenu*>& getMenus() { return m_menus; }

	static SHistoryItem&  getLastHistoryItem() {
		if (SMainWindow::curSessionHistory.size() > 0)
			return SMainWindow::curSessionHistory[SMainWindow::curSessionHistory.size() - 1];
		else
			SHistoryItem();
	}
	static void addHistoryItem(QString title, QUrl url);

	static QSettings* SSettings;
	static QVector<SHistoryItem> curSessionHistory;
	static QVector<SDownloadItem*> dlItems;

	static QString dataPath;

	bool privateBrowsing{ false };
public slots:
	// Change title of tab and url in url area from a web view
	void changeTitle(const QString& newTitle);
	void changeUrl(const QUrl& newUrl);

	// Classical action
	void fullScreen();
	void addDownload(QWebEngineDownloadItem *download);
	void removeDownload();

	// Action of navigation
	void back();
	void next();
	void home();
	void refresh();
	void stop();

protected:
	virtual void closeEvent(QCloseEvent *event); //< Called when Sielo is stoped

private:
	// Private function
	void restoreTabs(); //< Restore tab from the previous session

	// Private attributs
	
	// Widgets of the window
	SUrlArea* m_urlArea{ new SUrlArea(this) };
	SSearchArea* m_searchArea{ nullptr };
    QSplitter* m_splitter{ new QSplitter(this) };
	STabWidget* m_tabs{ new STabWidget(this) };
	QVector<SToolBar*> m_toolsBars{};
	QVector<SMenu*> m_menus{};

	// Other
	QSharedPointer<SActions> m_actions{};
};

