#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/STabWidget.hpp"

#include <QMessageBox>
#include <QVBoxLayout>

SWebView::SWebView(QWidget * parent, QUrl url) :
	QWebEngineView(parent),
	m_parent(parent)
{

    connect(this->page(), &QWebEnginePage::fullScreenRequested, this, &SWebView::setFullScreen);

    load(url);
}

SWebView::~SWebView()
{
	// Empty
}

void SWebView::setFullScreen(QWebEngineFullScreenRequest request)
{
    if(!m_fullScreen) {
        setParent(nullptr);
        showFullScreen();
        m_parent->layout()->removeWidget(this);
        m_fullScreen = true;
    }
    else {
        setParent(m_parent);
        show();
        m_parent->layout()->addWidget(this);
        m_fullScreen = false;
    }

    request.accept();
}
