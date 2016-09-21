#pragma once 

#include <QTabWidget>
#include <QUrl>
#include <QWidget>

class SWebView;
class SMainWindow;

class STabWidget : public QTabWidget
{
public:
	STabWidget(SMainWindow* parent = nullptr);
	~STabWidget();

	void createWebTab(QString& title, SWebView* view);
	void createWebTab(QString& title, QUrl& url);
	void createPlusTab();

public slots:
	void tabChanged(int index);
	void tabClosed(int index = -1);
	void createDefaultWebTab();

private:
	SMainWindow* m_parent;
};