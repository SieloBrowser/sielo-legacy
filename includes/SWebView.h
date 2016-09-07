#ifndef SWEBVIEW_H
#define SWEBVIEW_H

#include <QWidget>
#include <QtWebEngineWidgets>

class SWebView : public QWebEngineView
{
public:
	SWebView(QWidget* parent = nullptr, QUrl url = QUrl("http://google.fr"));
	~SWebView();

public slots:
	void setFullScreen(QWebEngineFullScreenRequest* request);

private:

};

#endif // SWEBVIEW_H
