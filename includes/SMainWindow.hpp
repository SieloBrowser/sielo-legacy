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
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

	void loadMenus();
	bool loadToolBar(const QString& filePath);
	void changeTabTitle(const QString& newTitle);
	void changeTabUrl(const QUrl& newUrl);

	// Getters
    QSharedPointer<SActions> getActions() const { return m_actions; }
	SUrlArea* getUrlArea() const { return m_urlArea; }
	SSearchArea* getSearchArea() const { return m_searchArea; }
	STabWidget* getTabs() { return m_tabs; }
	SWebView* currentPage();
    QVector<SMenu*>& getMenus() { return m_menus; }

    static SHistoryItem&  getLastHistoryItem() {
        if(SMainWindow::curSessionHistory.size() > 0)
            return SMainWindow::curSessionHistory[SMainWindow::curSessionHistory.size() - 1];
    }
    static void addHistoryItem(QString title, QUrl url);

    static QSettings* SSettings;
    static QVector<SHistoryItem> curSessionHistory;
    static QVector<SDownloadItem*> dlItems;

    bool privateBrowsing{ false };
public slots:
	void changeTitle(const QString& newTitle);
	void changeUrl(const QUrl& newUrl);

	void fullScreen();
    void addDownload(QWebEngineDownloadItem *download);
    void removeDownload();

	void back();
	void next();
	void home();
	void refresh();
	void stop();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
    void restoreTabs();

	// Private member
	// Widgets
	SUrlArea* m_urlArea{ new SUrlArea(this) };
	SSearchArea* m_searchArea{ nullptr };
	STabWidget* m_tabs{ new STabWidget(this) };
	QVector<SToolBar*> m_toolsBars{};
	QVector<SMenu*> m_menus{};

	// Other
    QSharedPointer<SActions> m_actions{};
};

