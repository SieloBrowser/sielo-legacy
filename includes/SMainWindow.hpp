#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QVector>
#include <QSettings>
#include <QLineEdit>

#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/SToolBar.hpp"
#include "includes/SWidgets/SMenu.hpp"
#include "includes/SWidgets/SUrlArea.hpp"
#include "includes/SWidgets/SSearchArea.hpp"

#include "includes/SWindows/SHistory.hpp"


struct Actions;

class SMainWindow : public QMainWindow
{
public:
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

	void loadMenus();
	bool loadToolBar(const QString& filePath);
	void changeTabTitle(const QString& newTitle);
	void changeTabUrl(const QUrl& newUrl);

    void addHistoryItem(QString title, QUrl url);

	// Getters
	Actions* getActions() const { return m_actions; }
	SUrlArea* getUrlArea() const { return m_urlArea; }
	SSearchArea* getSearchArea() const { return m_searchArea; }
	STabWidget* getTabs() { return m_tabs; }
	SWebView* currentPage();

	QVector<SHistoryItem> getCurSessionHistory() { return m_curSessionHistory; }
	SHistoryItem  getLastHistoryItem() { if(m_curSessionHistory.size() > 0) return m_curSessionHistory[m_curSessionHistory.size() - 1]; }

    static QSettings* SSettings;
public slots:
	void changeTitle(const QString& newTitle);
	void changeUrl(const QUrl& newUrl);

	void fullScreen();

	void back();
	void next();
	void home();
	void refresh();
	void stop();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
	// Private member
	// Widgets
	SUrlArea* m_urlArea{ new SUrlArea(this) };
	SSearchArea* m_searchArea{ nullptr };
	STabWidget* m_tabs{ new STabWidget(this) };
	QVector<SToolBar*> m_toolsBars{};
	QVector<SMenu*> m_menus{};

	// Windows
	SHistoryWindow* m_historyWindow{ nullptr };

	// Other
	Actions* m_actions{ nullptr };
	QVector<SHistoryItem> m_curSessionHistory{};
};

