#include "includes/SWidgets/SWebView.hpp"
#include "includes/SWidgets/STabWidget.hpp"
#include "includes/SMainWindow.hpp"

#include <QMessageBox>
#include <QVBoxLayout>

SWebView::SWebView(QWidget * parent, STabWidget *parentTab, QUrl url) :
	QWebEngineView(parent),
	m_parent(parent),
	m_parentTab(parentTab)
{
    connect(this->page(), &QWebEnginePage::fullScreenRequested, this, &SWebView::setFullScreen);

    load(url);
}

SWebView::~SWebView()
{
	// Empty
}

void SWebView::changeParent(QWidget * parent, STabWidget *parentTab)
{
	setParent(parent);
	m_parent = parent;
	m_parentTab = parentTab;
}

SWebView * SWebView::createWindow(QWebEnginePage::WebWindowType type)
{
	if (type == QWebEnginePage::WebBrowserWindow) {
		SWebView *newView{ new SWebView(nullptr) };
		SMainWindow *newWindow{ new SMainWindow(nullptr, newView) };
		return newView;
	}
	else if (type == QWebEnginePage::WebDialog) {
		SWebView *newView{ new SWebView(nullptr) };
		newView->show();
		return newView;
	}
	else if (type == QWebEnginePage::WebBrowserBackgroundTab) {
		SWebView *view{ new SWebView(nullptr, m_parentTab) };
		m_parentTab->createWebTab("Nouvelle onglet", view);
		m_parentTab->createPlusTab();
		m_parentTab->removeTab(m_parentTab->count() - 3);
		return view;
	}
	else if (type == QWebEnginePage::WebBrowserTab) {
		SWebView *view{ new SWebView(nullptr, m_parentTab) };
		m_parentTab->createWebTab("Nouvelle onglet", view);
		m_parentTab->createPlusTab();
		m_parentTab->removeTab(m_parentTab->count() - 3);
		m_parentTab->setCurrentIndex(m_parentTab->count() - 2);
		return view;
	}

	return nullptr;
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
