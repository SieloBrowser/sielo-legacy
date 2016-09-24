#pragma once

#include <QMainWindow>
#include <QWidget>
#include <QSettings>
#include <QLineEdit>

#include "../includes/SWidgets/STabWidget.hpp"
#include "../includes/SWidgets/SWebView.hpp"
#include "../includes/SWidgets/SToolBar.hpp"
#include "../includes/SWidgets/SMenu.hpp"
#include "../includes/SWidgets/SUrlArea.hpp"
#include "../includes/SWidgets/SSearchArea.hpp"


struct Actions;

class SMainWindow : public QMainWindow
{
public:
	static QSettings* SSettings;
	
	SMainWindow(QWidget* parent = nullptr);
	~SMainWindow();

	void loadMenus();
	bool loadToolBar(const QString& filePath);
	void changeTabTitle(const QString& newTitle);
	void changeTabUrl(const QUrl& newUrl);

	// Getters
	Actions* getActions() const { return m_actions; }
	SUrlArea* getUrlArea() const { return m_urlArea; }
	SSearchArea* getSearchArea() const { return m_searchArea; }
	STabWidget* getTabs() { return m_tabs; }

	SWebView* currentPage();

public slots:
	void changeTitle(const QString& newTitle);
	void changeUrl(const QUrl& newUrl);

	void fullScreen();

	void back();
	void next();
	void home();
	void refresh();
	void stop();
private:
	// Private member
	SUrlArea* m_urlArea{ new SUrlArea(this) };
	SSearchArea* m_searchArea{ nullptr };
	Actions* m_actions{ nullptr };
	STabWidget* m_tabs{ new STabWidget(this) };
	QVector<SToolBar*> m_toolsBars{};
	QVector<SMenu*> m_menus{};
};

