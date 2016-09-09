#include "..\includes\SMainWindow.h"

#include <QMessageBox>

SMainWindow::SMainWindow(QWidget* parent) : 
	QMainWindow(parent)
{
	SWebView* webView{ new SWebView(m_tabs, QUrl("http://google.com")) };
	m_tabs->createWebTab("Teste", webView);

	setCentralWidget(m_tabs);

	addToolBar(m_toolBar);
}


SMainWindow::~SMainWindow()
{
}
