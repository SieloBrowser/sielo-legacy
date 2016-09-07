#ifndef STABWIDGET_H
#define STABWIDGET_H

#include <QTabWidget>
#include <QUrl>
#include <QWidget>

class SWebView;

class STabWidget : public QTabWidget
{
public:
	STabWidget(QWidget* parent = nullptr);
	~STabWidget();

	void createWebTab(QString title, SWebView* view = nullptr, QUrl url = QUrl("http://google.com"));

private:

};

#endif // STABWIDGET_H
