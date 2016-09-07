#ifndef SWEBVIEW_H
#define SWEBVIEW_H

#include <QWidget>
#include <QtWebEngineWidgets>

#include "STabWidget.h"

class SWebView : public QWebEngineView
{
public:
	SWebView(QWidget* parent = nullptr, QUrl url = QUrl("http://google.fr"));
	~SWebView();

public slots:
	void setFullScreen(QWebEngineFullScreenRequest* request);

private:
	QWidget* m_parent{ nullptr };
};

#endif // SWEBVIEW_H
