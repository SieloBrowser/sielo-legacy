#include "..\includes\SMainWindow.h"

#include <QMessageBox>

QSettings * SMainWindow::SSettings = new QSettings("data/sdata.ini", QSettings::IniFormat);

SMainWindow::SMainWindow(QWidget* parent) : 
	QMainWindow(parent)
{
	SWebView* webView{ new SWebView(m_tabs, QUrl("http://google.com")) };
	m_tabs->createWebTab(webView->title(), webView);

	setCentralWidget(m_tabs);

	addToolBar(m_toolBar);
}


SMainWindow::~SMainWindow()
{
}
