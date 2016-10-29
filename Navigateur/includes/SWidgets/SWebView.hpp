#pragma once 

#include <QWidget>
#include <QtWebEngineWidgets>
#include <QContextMenuEvent>

class STabWidget;

class SWebView : public QWebEngineView
{
public:
	// Constructor and destructor
	SWebView(QWidget* parent = nullptr, STabWidget *parentTab = nullptr, QUrl url = QUrl("http://google.fr"));
	~SWebView();

	// For set parent and override m_parent
	void changeParent(QWidget *parent, STabWidget *parentTab);
protected:
	void contextMenuEvent(QContextMenuEvent *event); //< To add "Open in new tab" action and more in the futur
	SWebView * createWindow(QWebEnginePage::WebWindowType type); //< Called when a new tab/window is requested

public slots:
    void setFullScreen(QWebEngineFullScreenRequest request); //< Called whene full screen is requested (ex: YouTube videos)

private:
	QWidget* m_parent{ nullptr };
    STabWidget *m_parentTab{ nullptr };
    bool m_fullScreen{ false };
};
