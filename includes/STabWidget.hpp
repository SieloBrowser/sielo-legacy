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

	void createWebTab(QString title, SWebView* view = nullptr, QUrl url = QUrl("http://google.com"));

private:
	SMainWindow* m_parent;
};

#endif // STABWIDGET_H
