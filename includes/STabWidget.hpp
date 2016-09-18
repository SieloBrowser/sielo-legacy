#ifndef STABWIDGET_H
#define STABWIDGET_H

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
	void createWebTab();
	void createPlusTab();

public slots:
	void tabChanged(int index);
	void tabClosed(int index = -1);

private:
	SMainWindow* m_parent;
};

#endif // STABWIDGET_H
