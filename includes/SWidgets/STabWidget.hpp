#pragma once 

#include <QTabWidget>
#include <QUrl>
#include <QWidget>

class SWebView;
class SMainWindow;

class STabWidget : public QTabWidget
{
public:
	// Constructor and destructor
	STabWidget(SMainWindow* parent = nullptr);
	~STabWidget();

	// Fontion for create tabs in the tab widget
    void createWebTab(QString title, SWebView* view);
    void createWebTab(QString title, QUrl url);
	void createPlusTab();

	// Create the default web tab (home page and "+" tabs)
    void createDefaultWebTab(); 
public slots:
	void tabChanged(int index); //< If tab index is changed
	void tabClosed(int index = -1); //< If tab is closed

private:
	SMainWindow* m_parent;
};
