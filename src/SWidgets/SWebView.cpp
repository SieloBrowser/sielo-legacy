#include "../includes/SWidgets/SWebView.hpp"

#include <QMessageBox>

SWebView::SWebView(QWidget * parent, QUrl url) :
	QWebEngineView(parent),
	m_parent(parent)
{
	connect(this->page(), SIGNAL(fullScreenRequested(QWebEngineFullScreenRequest)), this, SLOT(setFullScreen(QWebEngineFullScreenRequest)));

	load(url);
}

SWebView::~SWebView()
{
	// Empty
}

void SWebView::setFullScreen(QWebEngineFullScreenRequest* request)
{
	QMessageBox::information(this, "DEBUG", "Full screen requested");
}
